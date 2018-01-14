//
// Created by Gijs Sijpesteijn on 18/11/2017.
//

#include "race_resource.h"
#include "../util/util.h"
#include "../util/log.h"
#include "../util/race_builder.h"

using namespace std;
using namespace restbed;

#define RACE_ALL "/race"
#define RACE_SINGLE "/race/{name: .*}"
#define RACE_SELECT "/race/select/{name: .*}"
#define RACE_OBSERVER_PREVIEW "/race/{name: .*}/{group: .*}/{observer: .*}/{stage: .*}/{timestamp: .*}"
#define RACE_STATUS "/status"

static map< string, shared_ptr< WebSocket > > sockets = { };
static pthread_mutex_t checker_lock = PTHREAD_MUTEX_INITIALIZER;
static race_resource *resource;


void* checkRaceStatus(void* params) {
    race_resource *r = (race_resource*) params;
    bool run = true;
    while(run) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (r->selected_race && r->car->getMode() != car_mode::autonomous) {
            r->selected_race->setSelected(false);
            r->selected_race->setRunning(false);
            r->selected_race = NULL;
        }
        if (sockets.size() > 0 && r->selected_race && r->selected_race->isSelected()) {
            json_t* json = r->selected_race->getJson(true);
            string body = json_dumps(json, 0);
            json_decref(json);
            for (map<string, shared_ptr<WebSocket>>::iterator iter = sockets.begin();
                 iter != sockets.end(); ++iter) {
                shared_ptr<WebSocket> socket = iter->second;

                auto response = make_shared<WebSocketMessage>(WebSocketMessage::TEXT_FRAME, body);
                socket->send(response);
            }
        }
    }
    return NULL;
}

void status_race_close_handler( const shared_ptr< WebSocket > socket )
{
    log::debug("Race status close handler");
    if ( socket->is_open( ) )
    {
        auto response = make_shared< WebSocketMessage >( WebSocketMessage::CONNECTION_CLOSE_FRAME, Bytes( { 10, 00 } ) );
        socket->send( response );
    }

    const auto key = socket->get_key( );
    sockets.erase( key );

    log::debug(string("Connection closed with ").append(key.data()));
}

void status_race_error_handler( const shared_ptr< WebSocket > socket, const error_code error )
{
    const auto key = socket->get_key( );
    log::debug(string("WebSocket Errored ").append(error.message().data()).append(" for").append(key.data()));
}

void status_race_message_handler( const shared_ptr< WebSocket > source, const shared_ptr< WebSocketMessage > message )
{
    const auto opcode = message->get_opcode( );

    if ( opcode == WebSocketMessage::CONNECTION_CLOSE_FRAME )
    {
        source->close( );
    }
    else if ( opcode == WebSocketMessage::BINARY_FRAME )
    {
        //We don't support binary data.
        auto response = make_shared< WebSocketMessage >( WebSocketMessage::CONNECTION_CLOSE_FRAME, Bytes( { 10, 03 } ) );
        source->send( response );
    }
    else if ( opcode == WebSocketMessage::TEXT_FRAME )
    {
        const string body = "";
        auto response = make_shared< WebSocketMessage >(WebSocketMessage::TEXT_FRAME, body );
        source->send(response);
    }
}

void status_race_handler( const shared_ptr< Session > session )
{
    const auto request = session->get_request( );
    const auto connection_header = request->get_header( "connection", String::lowercase );

    if ( connection_header.find( "upgrade" ) not_eq string::npos )
    {
        if ( request->get_header( "upgrade", String::lowercase ) == "websocket" )
        {
            const auto headers = build_websocket_handshake_response_headers( request, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11" );

            session->upgrade( SWITCHING_PROTOCOLS, headers, [ ]( const shared_ptr< WebSocket > socket )
            {
                if ( socket->is_open( ) )
                {
                    if (pthread_mutex_lock(&checker_lock) != 0) {
                        log::debug("Sockethandler: Could not get a lock on the queue");
                    }
                    socket->set_close_handler( status_race_close_handler );
                    socket->set_error_handler( status_race_error_handler );
                    socket->set_message_handler( status_race_message_handler );

                    const string body = "";
                    socket->send( body, [ ]( const shared_ptr< WebSocket > socket )
                    {
                        const auto key = socket->get_key( );
                        sockets.insert( make_pair( key, socket ) );
                        log::debug(string("Race resource sockets size ").append(to_string(sockets.size())));
                    } );
                    if (pthread_mutex_unlock(&checker_lock) != 0) {
                        log::debug(string("Sockethandler: Could not unlock the queue"));
                    }
                }
                else
                {
                    fprintf( stderr, "WebSocket Negotiation Failed: Client closed connection.\n" );
                }
            } );

            return;
        }
    }

    session->close( BAD_REQUEST );
}

void all_race_handler(const shared_ptr<Session> session) {
    string body = "[";
    for(map<string,race*>::iterator it = resource->races.begin(); it != resource->races.end(); ++it) {
        body += "{\"name\": \"" + it->first + "\"},";
    }
    if (body.size() > 1) {
        body = body.substr(0, body.size() - 1);
    }
    body += "]";
    session->close( OK, body, { { "Content-Length", ::to_string( body.size( ) ) } } );
}

void get_race_handler(const shared_ptr<Session> session) {
    const auto& request = session->get_request( );
    const string name = request->get_path_parameter("name");
    race* race = resource->getRace(name);
    if (race == NULL) {
        resource->sendError(session, "Could not find " + name + " race.");
    } else {
        json_t* json = race->getJson(true);
        string body = json_dumps(json, 0);
        json_decref(json);

        session->close(OK, body, {
                { "Content-Type", "application/json" },
                { "Content-Length", ::to_string(body.size()) }
        });
    }
}

void select_race_handler(const shared_ptr<Session> session) {
    const auto& request = session->get_request( );
    const string name = request->get_path_parameter("name");
    race* race = resource->getRace(name);
    if (race == NULL) {
        resource->sendError(session, "Could not find " + name + " race.");
    } else {
//        if (resource->race_status_runner == NULL) {
//            pthread_create(&resource->race_status_runner, NULL, checkRaceStatus, resource);
//        }
        resource->selected_race = race;
        for(auto const& race : resource->races) {
            race.second->setSelected(false);
        }
        race->setSelected(true);
        json_t* json = race->getJson(true);
        string body = json_dumps(json, 0);
        json_decref(json);

        session->close(OK, body, {
                { "Content-Type", "application/json" },
                { "Content-Length", ::to_string(body.size()) }
        });
    }
}

void get_race_observer_preview(const shared_ptr<Session> session) {
    const auto& request = session->get_request( );
    const string name = request->get_path_parameter("name");
    const string group = request->get_path_parameter("group");
    const string observer = request->get_path_parameter("observer");
    const string observer_stage = request->get_path_parameter("stage");
    string previewImage = "";
    if (resource->selected_race) {
        previewImage = resource->selected_race->getPreviewImageLocation(group, observer, observer_stage);
    }
    ifstream stream(previewImage, ios::in | ios::binary);
    const string body = string( istreambuf_iterator< char >( stream ), istreambuf_iterator< char >( ) );
    const multimap< string, string > headers
            {
                    { "Content-Type", "image/jpeg" },
                    { "Content-Length", ::to_string( body.length() ) }
            };

    session->close( OK, body, headers );
}

void post_race_handler(const shared_ptr<Session> session) {
    const auto request = session->get_request();
    int length = request->get_header( "Content-Length", 0 );

    session->fetch( length, [ ]( const shared_ptr< Session > session, const Bytes& )
    {
        const auto request = session->get_request( );
        const auto body = request->get_body( );

        fprintf( stdout, "Save race: %.*s\n", static_cast< int >( body.size( ) ), body.data( ) );
        string data = string(body.begin(), body.end());
        json_t* root;
        json_error_t error;

        root = json_loadb(data.c_str(), data.size(), JSON_DECODE_INT_AS_REAL, &error);
        if (!root) {
            fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
        }
        if (!json_is_object(root)) {
            fprintf(stderr, "error: commit data is not an object\n");
            json_decref(root);
        }

        json_t *value = json_object_get(root, "name");
        if (value == NULL) {
            resource->sendError(session, "Could not find name in json.");
        } else {
            string name = "";
            name += json_string_value(value);
            race *race = resource->getRace(name);
            if (race == NULL) {
                resource->sendError(session, "Could not find " + name + " race.");
            } else {
                race->updateWithJson(root, true);
                race->saveSettings();
                json_t* json = race->getJson(true);
                string body = json_dumps(json, 0);
                json_decref(json);
                session->close(OK, body, {
                        {"Content-Type",   "application/json"},
                        {"Content-Length", ::to_string(body.size())}
                });
            }
        }
    });
}

race_resource::race_resource(Camera *camera, Car *car) {
    this->car = car;

    race_builder *rb = new race_builder();
    this->races = rb->getRaces("./resources/races/", car, camera);

    this->allRaceResource->set_path( RACE_ALL );
    this->allRaceResource->set_method_handler("GET", all_race_handler);
    log::debug(string("Restbed: ").append(RACE_ALL));

    this->singleRaceResource->set_path( RACE_SINGLE );
    this->singleRaceResource->set_method_handler("GET", get_race_handler);
    this->singleRaceResource->set_method_handler("POST", post_race_handler);
    log::debug(string("Restbed: ").append(RACE_SINGLE));

    this->selectRaceResource->set_path( RACE_SELECT );
    this->selectRaceResource->set_method_handler("POST", select_race_handler);
    log::debug(string("Restbed: ").append(RACE_SELECT));

    this->statusRaceResource->set_path( RACE_STATUS );
    this->statusRaceResource->set_method_handler("GET", status_race_handler);
    log::debug(string("Restbed: ").append(RACE_STATUS));

    this->observerPreviewResource->set_path( RACE_OBSERVER_PREVIEW );
    this->observerPreviewResource->set_method_handler("GET", get_race_observer_preview);
    log::debug(string("Restbed: ").append(RACE_OBSERVER_PREVIEW));

    pthread_create(&this->race_status_runner, NULL, checkRaceStatus, this);

    resource = this;
}

race* race_resource::getRace(string name) {
    map<string,race*>::iterator it = this->races.find(name);
    if (it == this->races.end()) {
        log::debug(string("CPU: Could not get race ").append(name.c_str()));
        return NULL;
    } else {
        return it->second;
    }

}

list<shared_ptr<Resource>> race_resource::getResources() {
    list<shared_ptr<Resource>> l = {
            this->allRaceResource,
            this->singleRaceResource,
            this->selectRaceResource,
            this->statusRaceResource,
            this->observerPreviewResource
    };
    return l;
}
