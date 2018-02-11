//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#include "car_resource.h"
#include "../util/log.h"
#include <iostream>

using namespace std;
using namespace restbed;

#define CAR "/car"
#define CAR_MODE "/car/mode"
#define CAR_ANGLE_POST "/car/steer/{angle: .*}"
#define CAR_THROTTLE_POST "/car/engine/{throttle: .*}"
#define CAR_MODE_POST "/car/mode/{mode: .*}"

static Car *car;
static car_resource *resource;

//void post_angle_handler(const shared_ptr<Session> session) {
//    if (car->getMode() == car_mode::stopped) {
//        const string body = "Car stopped";
//        session->close(405, body, {{ "Content-Length", ::to_string(body.size()) }});
//    } else {
//        const auto request = session->get_request();
//        const int angle = atoi(request->get_path_parameter("angle").c_str());
//
//        const string body = car->setAngle(angle) == 0 ? "true" : "false";
//        session->close(OK, body, {
//                { "Content-Type", "application/json" },
//                { "Content-Length", ::to_string(body.size()) }
//        });
//    }
//}
//
//void post_throttle_handler(const shared_ptr<Session> session) {
//    if (car->getMode() == car_mode::stopped) {
//        const string body = "Car stopped";
//        session->close(405, body, {{ "Content-Length", ::to_string(body.size()) }});
//    } else {
//        const auto request = session->get_request();
//        const int throttle = atoi(
//                request->get_path_parameter("throttle").c_str());
//
//        const string body = car->setThrottle(throttle) == 0 ? "true" : "false";
//        session->close(OK, body, {
//                { "Content-Type", "application/json" },
//                { "Content-Length", ::to_string(body.size()) }
//        });
//    }
//}

void get_car_handler(const shared_ptr<Session> session) {
    json_t* json = car->getJson();
    string body = json_dumps(json, 0);
    session->close(OK, body, {
            { "Content-Type", "application/json" },
            { "Content-Length", ::to_string(body.size()) }
    });
    json_decref(json);
}

void post_car_handler(const shared_ptr<Session> session) {
    const auto request = session->get_request( );

    int content_length = request->get_header( "Content-Length", 0 );

    session->fetch( content_length, [ ]( const shared_ptr< Session > session, const Bytes & body )
    {
        fprintf( stdout, "%.*s\n", ( int ) body.size( ), body.data( ) );
        json_t* root;
        json_error_t error;

        root = json_loadb(reinterpret_cast<const char *>(body.data()), body.size(), 0, &error);
        if (!root) {
            fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
        }
        if (!json_is_object(root)) {
            fprintf(stderr, "error: commit data is not an object\n");
            json_decref(root);
        }
        car->fromJson(root);
        session->close( OK, body, { { "Content-Length", ::to_string( body.size( ) ) } } );
    } );
}

void get_car_mode_handler(const shared_ptr<Session> session) {
    const string body = to_string(static_cast<std::underlying_type<car_mode>::type>(car->getMode()));
    session->close(OK, body, {
            { "Content-Type", "application/json" },
            { "Content-Length", ::to_string(body.size()) }
    });
}

//void post_car_mode_handler(const shared_ptr<Session> session) {
//    const auto& request = session->get_request();
//    if (car->getEnabled()) {
//        const string mode_str = request->get_path_parameter("mode");
//        if (isdigit(mode_str[0])) {
//            car_mode mode = static_cast<car_mode>(atoi(mode_str.c_str()));
//            int modeInt = (int)mode;
//            if (modeInt >= 0 && modeInt <= (int)car_mode::num_values) {
//                log::debug(string("Car mode set to: ").append(to_string(static_cast<std::underlying_type<car_mode>::type>(mode))));
//                car->setMode(mode);
//                const string body = "{\"mode\": " + to_string(static_cast<std::underlying_type<car_mode>::type>(car->getMode())) + "}";
//                session->close(OK, body, {
//                        { "Content-Type", "application/json" },
//                        { "Content-Length", ::to_string(body.size()) }
//                });
//            } else {
//                resource->sendError(session, "unknown mode " + to_string(modeInt));
//            }
//        } else {
//            resource->sendError(session, "unknown mode " + mode_str);
//        }
//    } else {
//        resource->sendError(session, "car not connected");
//    }
//}

car_resource::car_resource(Car *c) {
    car = c;

    this->carResource->set_path(CAR);
    this->carResource->set_method_handler("GET", get_car_handler);
    this->carResource->set_method_handler("POST", post_car_handler);
    log::debug(string("Restbed: ").append(CAR));

    this->carGetModeResource->set_path(CAR_MODE);
    this->carGetModeResource->set_method_handler("GET", get_car_mode_handler);
    log::debug(string("Restbed: ").append(CAR_MODE));

//    this->carPostModeResource->set_path(CAR_MODE_POST);
//    this->carPostModeResource->set_method_handler("POST", post_car_mode_handler);
//    log::debug(string("Restbed: ").append(CAR_MODE_POST));
//
//    this->steerResource->set_path(CAR_ANGLE_POST);
//    this->steerResource->set_method_handler("POST", post_angle_handler);
//    log::debug(string("Restbed: ").append(CAR_ANGLE_POST));
//
//    this->engineResource->set_path(CAR_THROTTLE_POST);
//    this->engineResource->set_method_handler("POST", post_throttle_handler);
//    log::debug(string("Restbed: ").append(CAR_THROTTLE_POST));
    resource = this;
}

list<shared_ptr<Resource>> car_resource::getResources() {
    list<shared_ptr<Resource>> l = { this->carResource, this->carGetModeResource };
    return l;
}
