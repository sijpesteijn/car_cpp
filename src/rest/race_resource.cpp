//
// Created by Gijs Sijpesteijn on 18/11/2017.
//

#include <syslog.h>
#include "race_resource.h"
#include "../races/drag_race.h"
#include "../races/track_race.h"
#include "../util/util.h"

using namespace std;
using namespace restbed;

#define RACE_ALL "/race"
#define RACE_SINGLE "/race/{name: .*}"
#define RACE_STATUS "/status"

static map< string, shared_ptr< WebSocket > > sockets = { };
static pthread_mutex_t checker_lock = PTHREAD_MUTEX_INITIALIZER;
static race_resource *resource;


void* checkRaceStatus(void* params) {
    race_resource *r = (race_resource*) params;
    while(1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        if (sockets.size() > 0 && r->selected_race) {
            for (map<string, shared_ptr<WebSocket>>::iterator iter = sockets.begin();
                 iter != sockets.end(); ++iter) {
                shared_ptr<WebSocket> socket = iter->second;
                json_t* json = r->selected_race->getJson();
                string body = json_dumps(json, 0);
                json_decref(json);

                auto response = make_shared<WebSocketMessage>(WebSocketMessage::TEXT_FRAME, body);
                socket->send(response);
            }
        }
    }
    return NULL;
}

void status_race_close_handler( const shared_ptr< WebSocket > socket )
{
    syslog(LOG_DEBUG, "Observer status close handler");
    if ( socket->is_open( ) )
    {
        auto response = make_shared< WebSocketMessage >( WebSocketMessage::CONNECTION_CLOSE_FRAME, Bytes( { 10, 00 } ) );
        socket->send( response );
    }

    const auto key = socket->get_key( );
    sockets.erase( key );
    syslog(LOG_DEBUG, "Connection closed with %s.\n", key.data( ));
}

void status_race_error_handler( const shared_ptr< WebSocket > socket, const error_code error )
{
    const auto key = socket->get_key( );
    syslog(LOG_ERR, "WebSocket Errored '%s' for %s.\n", error.message( ).data( ), key.data( ) );
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
                        syslog(LOG_ERR, "Sockethandler: Could not get a lock on the queue");
                    }
                    socket->set_close_handler( status_race_close_handler );
                    socket->set_error_handler( status_race_error_handler );
                    socket->set_message_handler( status_race_message_handler );

                    const string body = "";
                    socket->send( body, [ ]( const shared_ptr< WebSocket > socket )
                    {
                        const auto key = socket->get_key( );
                        sockets.insert( make_pair( key, socket ) );
                        syslog(LOG_DEBUG, "%lu", sockets.size());
                        fprintf( stderr, "Race resource: Sent welcome message to %s.\n", key.data( ) );
                    } );
                    if (pthread_mutex_unlock(&checker_lock) != 0) {
                        syslog(LOG_ERR, "%s", "Sockethandler: Could not unlock the queue");
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
        json_t* json = race->getJson();
        string body = json_dumps(json, 0);
        json_decref(json);

        session->close(OK, body, {
                { "Content-Type", "application/json" },
                { "Content-Length", ::to_string(body.size()) }
        });
    }
}

void post_race_handler(const shared_ptr<Session> session) {
    const auto request = session->get_request();
    int length = request->get_header( "Content-Length", 0 );

    session->fetch( length, [ ]( const shared_ptr< Session > session, const Bytes& )
    {
        const auto request = session->get_request( );
        const auto body = request->get_body( );

//        fprintf( stdout, "Complete body content: %.*s\n", static_cast< int >( body.size( ) ), body.data( ) );
        string data = string(body.begin(), body.end());
        json_t* root;
        json_error_t error;

        root = json_loadb(data.c_str(), data.size(), 0, &error);
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
            resource->selected_race = resource->getRace(name);
            if (resource->selected_race == NULL) {
                resource->sendError(session, "Could not find " + name + " race.");
            } else {
                resource->selected_race->updateWithJson(root, 1);
                resource->selected_race->saveSettings();
                json_t* json = resource->selected_race->getJson();
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

race_resource::race_resource(Camera *camera) {
    drag_race *dr = new drag_race(camera);
    this->races.insert(pair<string, race*>(dr->getName(), dr));
    track_race *tr = new track_race();
    this->races.insert(pair<string, race*>(tr->getName(), tr));

    this->allRaceResource->set_path( RACE_ALL );
    this->allRaceResource->set_method_handler("GET", all_race_handler);
    syslog(LOG_DEBUG, "Restbed: %s",  RACE_ALL);

    this->singleRaceResource->set_path( RACE_SINGLE );
    this->singleRaceResource->set_method_handler("GET", get_race_handler);
    this->singleRaceResource->set_method_handler("POST", post_race_handler);
    syslog(LOG_DEBUG, "Restbed: %s",  RACE_SINGLE);

    this->statusRaceResource->set_path( RACE_STATUS );
    this->statusRaceResource->set_method_handler("GET", status_race_handler);
    syslog(LOG_DEBUG, "Restbed: %s", RACE_STATUS);

    pthread_t race_status_runner;
    pthread_create(&race_status_runner, NULL, checkRaceStatus, this);

    resource = this;
}

race* race_resource::getRace(string name) {
    map<string,race*>::iterator it = this->races.find(name);
    if (it == this->races.end()) {
        syslog(LOG_ERR, "CPU: Could not get race %s.", name.c_str());
        return NULL;
    } else {
        return it->second;
    }

}

list<shared_ptr<Resource>> race_resource::getResources() {
    list<shared_ptr<Resource>> l = {
            this->allRaceResource,
            this->singleRaceResource,
            this->statusRaceResource
    };
    return l;
}