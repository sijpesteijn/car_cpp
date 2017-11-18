//
// Created by Gijs Sijpesteijn on 09/11/2017.
//

#include "observer_status_handler.h"
#include <syslog.h>
#include "../util/util.h"

using namespace restbed;
using namespace std;

#define OBSERVER_STATUS "/status"
static map< string, shared_ptr< WebSocket > > sockets = { };
static pthread_mutex_t checker_lock = PTHREAD_MUTEX_INITIALIZER;

void observer_status_close_handler( const shared_ptr< WebSocket > socket )
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

void observer_status_error_handler( const shared_ptr< WebSocket > socket, const error_code error )
{
    const auto key = socket->get_key( );
    syslog(LOG_ERR, "WebSocket Errored '%s' for %s.\n", error.message( ).data( ), key.data( ) );
}

void observer_status_message_handler( const shared_ptr< WebSocket > source, const shared_ptr< WebSocketMessage > message )
{
    const auto opcode = message->get_opcode( );

    if ( opcode == WebSocketMessage::PING_FRAME )
    {
        auto response = make_shared< WebSocketMessage >( WebSocketMessage::PONG_FRAME, message->get_data( ) );
        source->send( response );
    }
    else if ( opcode == WebSocketMessage::PONG_FRAME )
    {
        cout << "PONG" << endl;
        //Ignore PONG_FRAME.
        //
        //Every time the ping_handler is scheduled to run, it fires off a PING_FRAME to each
        //WebSocket. The client, if behaving correctly, will respond with a PONG_FRAME.
        //
        //On each occasion the underlying TCP socket sees any packet data transfer, whether
        //a PING, PONG, TEXT, or BINARY... frame. It will automatically reset the timeout counter
        //leaving the connection active; see also Settings::set_connection_timeout.
        return;
    }
    else if ( opcode == WebSocketMessage::CONNECTION_CLOSE_FRAME )
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
        const string body = ""; // "{\"mode\": " + to_string(static_cast<std::underlying_type<car_mode>::type>(car->getMode())) + ","
//                            + "\"angle\": " + to_string(car->getAngle()) + ","
//                            + "\"enabled\": " + to_string(car->getEnabled()) + ","
//                            + "\"throttle\": " + to_string(car->getThrottle()) + "}";
        auto response = make_shared< WebSocketMessage >(WebSocketMessage::TEXT_FRAME, body );
        source->send(response);
//        response->set_mask( 0 );
//        for ( auto socket : sockets )
//        {
//            auto destination = socket.second;
//            destination->send( response );
//        }
//
//        const auto key = source->get_key( );
//        const auto data = String::format( "Received message '%.*s' from %s\n", message->get_data( ).size( ), message->get_data( ).data( ), key.data( ) );
//        fprintf( stderr, "%s", data.data( ) );
    }
}

void get_observer_status_handler( const shared_ptr< Session > session )
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
                    socket->set_close_handler( observer_status_close_handler );
                    socket->set_error_handler( observer_status_error_handler );
                    socket->set_message_handler( observer_status_message_handler );

                    const string body = ""; // "{\"mode\": " + to_string(static_cast<std::underlying_type<car_mode>::type>(car->getMode())) + ","
//                                        + "\"angle\": " + to_string(car->getAngle()) + ","
//                                        + "\"throttle\": " + to_string(car->getThrottle()) + "}";
                    socket->send( body, [ ]( const shared_ptr< WebSocket > socket )
                    {
                        const auto key = socket->get_key( );
                        sockets.insert( make_pair( key, socket ) );
                        syslog(LOG_DEBUG, "%lu", sockets.size());
//                        car->setEnabled(1);

                        fprintf( stderr, "Sent welcome message to %s.\n", key.data( ) );
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

observer_status_handler::observer_status_handler() {
    this->observerStatusResource->set_path(OBSERVER_STATUS);
    this->observerStatusResource->set_method_handler("GET", get_observer_status_handler);
    syslog(LOG_DEBUG, "Restbed: %s",  OBSERVER_STATUS);
};

list<shared_ptr<Resource>> observer_status_handler::getResources() {
    list<shared_ptr<Resource>> l = {
            this->observerStatusResource
    };
    return l;
}

string getStates(list<observer *> observers) {
    string body = "[";
    for (auto const& i : observers) {
        if (i->isActive()) {
            body += i->getJson() + ",";
        }
    }
    if (body.length() > 1) {
        body = body.substr(0, body.length() - 1);
    }
    return body + "]";
}

void observer_status_handler::notifyClients(std::list<observer *> observers) {
    for(map<string, shared_ptr< WebSocket>>::iterator it = sockets.begin(); it != sockets.end(); ++it) {
//        cout << it->first << endl;
        shared_ptr<WebSocket> socket = it->second;
        const string body = getStates(observers);
        auto response = make_shared< WebSocketMessage >(WebSocketMessage::TEXT_FRAME, body );
        socket->send(response);
    }
}
