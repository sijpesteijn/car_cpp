//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#include "lifeline_resource.h"
#include <iostream>
#include "../util/util.h"
#include "../util/log.h"

using namespace std;
using namespace restbed;
using namespace std::chrono;

#define LIFELINE "/lifeline"

static Car *car;
static pthread_mutex_t checker_lock = PTHREAD_MUTEX_INITIALIZER;
static shared_ptr< Service > service = nullptr;
static map< string, shared_ptr< WebSocket > > sockets = { };

void lifeline_close_handler( const shared_ptr< WebSocket > socket )
{
    log::debug(string("Lifeline close handler"));
    if ( socket->is_open( ) )
    {
        auto response = make_shared< WebSocketMessage >( WebSocketMessage::CONNECTION_CLOSE_FRAME, Bytes( { 10, 00 } ) );
        socket->send( response );
    }

    const auto key = socket->get_key( );
    sockets.erase( key );
    log::debug(string("Lifeline handler sockets size: ").append(to_string(sockets.size())));
    if (car->getEnabled() != 0 && sockets.size() == 0) {
        log::debug(string("No connections car stopped"));
        car->setEnabled(false);
    }

    log::debug(string("Closed connection to ").append(key.data()));
}

void lifeline_error_handler( const shared_ptr< WebSocket > socket, const error_code error )
{
    const auto key = socket->get_key( );
    log::debug(string("WebSocket Errored").append(error.message().data()).append(" for").append(key.data()));
}

void lifeline_message_handler( const shared_ptr< WebSocket > source, const shared_ptr< WebSocketMessage > message )
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
        //leaving the connection selected; see also Settings::set_connection_timeout.
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
        json_t* json = car->getJson();
        string body = json_dumps(json, 0);
        json_decref(json);
        auto response = make_shared< WebSocketMessage >(WebSocketMessage::TEXT_FRAME, body );
        source->send(response);
    }
}

void get_lifeline_method_handler( const shared_ptr< Session > session )
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
                        log::debug(string("Sockethandler: Could not get a lock on the queue"));
                    }
                    socket->set_close_handler( lifeline_close_handler );
                    socket->set_error_handler( lifeline_error_handler );
                    socket->set_message_handler( lifeline_message_handler );

                    json_t* json = car->getJson();
                    string body = json_dumps(json, 0);
                    json_decref(json);
                    socket->send( body, [ ]( const shared_ptr< WebSocket > socket )
                    {
                        const auto key = socket->get_key( );
                        sockets.insert( make_pair( key, socket ) );
                        log::debug(string("Lifeline handler sockets size: ").append(to_string(sockets.size())));
                        car->setEnabled(true);

                        log::debug(string("Lifeline resource sockets size ").append(to_string(sockets.size())));
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

//void* connectionChecker(void* params) {
//	Car *car = (Car*) params;
//	while(1) {
//		if (pthread_mutex_lock(&checker_lock) != 0) {
//			syslog(LOG_ERR, "Sockethandler: Could not get a lock on the queue");
//		}
//		if (car->getEnabled() != 0 && sockets.size() == 0) {
//			syslog(LOG_ERR, "No connections car stopped");
//			car->setEnabled(0);
//		}
//		if (car->getEnabled() == 0 && sockets.size() > 0) {
//			syslog(LOG_ERR, "Enabling car since we have websocket connections.");
//			car->setEnabled(1);
//		}
//		if (pthread_mutex_unlock(&checker_lock) != 0) {
//			syslog(LOG_ERR, "Sockethandler: Could not unlock the queue");
//		}
//	}
//	return NULL;
//}

lifeline_resource::lifeline_resource(Car *carP) {
    car = carP;
    this->resource = make_shared< Resource >( );
    this->resource->set_path( LIFELINE );
    this->resource->set_method_handler( "GET", get_lifeline_method_handler );
    log::debug(string("Restbed websocket: ").append(LIFELINE));
//	pthread_t checker;
//	pthread_create(&checker, NULL, connectionChecker, carP);
}

list<shared_ptr<Resource>> lifeline_resource::getResources() {
    return { this->resource };
}