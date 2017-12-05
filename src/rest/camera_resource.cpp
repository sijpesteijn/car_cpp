//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#include "camera_resource.h"
#include <syslog.h>
#include <jansson.h>
using namespace cv;
using namespace std;
using namespace restbed;

#define CAMERA_SETTINGS "/camera/settings"
#define CAMERA_DIMENSIONS "/camera/dimensions"
#define CAMERA_SNAPSHOT "/camera/snapshot/{timestamp: .*}"
Camera *camera;

void get_camera_settings_handler(const shared_ptr<Session> session)
{
    const string body = json_dumps(camera->getJson(), 0);
//    fprintf( stdout, "%.*s\n", ( int ) body.size( ), body.data( ) );
    session->close( OK, body, { { "Content-Length", ::to_string( body.size( ) ) } } );
}

void post_camera_settings_handler( const shared_ptr< Session > session )
{
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
        camera->fromJson(root);
        session->close( OK, body, { { "Content-Length", ::to_string( body.size( ) ) } } );
    } );
}

void get_camera_dimensions_handler(const shared_ptr<Session> session)
{
    const string body = "[{\"name\": \"352x288\", \"width\": 352, \"height\": 288},"
            "{\"name\": \"640x480\", \"width\": 640, \"height\": 480},"
            "{\"name\": \"1280x720\", \"width\": 1280, \"height\": 720},"
            "{\"name\": \"1280x960\", \"width\": 1280, \"height\": 960}]";
    session->close( OK, body, { { "Content-Length", ::to_string( body.size( ) ) } } );
}

void get_camera_snapshot_handler( const shared_ptr< Session > session)
{
    Mat snapshot = camera->getFrame();
    Mat new_image = Mat::zeros(snapshot.size(), snapshot.type());
    for( int y = 0; y < snapshot.rows; y++ )
    { for( int x = 0; x < snapshot.cols; x++ )
        { for( int c = 0; c < 3; c++ )
            {
                new_image.at<Vec3b>(y,x)[c] =
                        saturate_cast<uchar>( camera->whitebalance_alpha*( snapshot.at<Vec3b>(y,x)[c] ) + camera->whitebalance_beta );
            }
        }
    }
    // TODO iets beters dan dit. Mat moet een ifstream worden. Maar hoe?
    imwrite("snapshot.jpg", new_image);
//	Mat snapshot = imread("bla.png");
//    Canny(snapshot, snapshot, 50, 200, 3);
//    cvtColor(snapshot, snapshot, CV_GRAY2BGR);
//    vector<Vec4i> lines;
//      HoughLinesP(snapshot, lines, 1, CV_PI/180, 50, 50, 10 );
//      for( size_t i = 0; i < lines.size(); i++ )
//      {
//        Vec4i l = lines[i];
//        line( snapshot, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
//      }
//      imwrite("output.png", snapshot);
    ifstream stream("snapshot.jpg", ios::in | ios::binary);
    const string body = string( istreambuf_iterator< char >( stream ), istreambuf_iterator< char >( ) );
    const multimap< string, string > headers
            {
                    { "Content-Type", "image/jpeg" },
                    { "Content-Length", ::to_string( body.length() ) }
            };

    session->close( OK, body, headers );
}

camera_resource::camera_resource(Camera *cam) {
    camera = cam;
    this->cameraGetDimensionsResource->set_path( CAMERA_DIMENSIONS );
    this->cameraGetDimensionsResource->set_method_handler("GET", get_camera_dimensions_handler);
    syslog(LOG_DEBUG, "Restbed: %s",  CAMERA_DIMENSIONS);

    this->cameraSettingsResource->set_path( CAMERA_SETTINGS );
    this->cameraSettingsResource->set_method_handler("GET", get_camera_settings_handler);
    this->cameraSettingsResource->set_method_handler("POST", post_camera_settings_handler);
    syslog(LOG_DEBUG, "Restbed: %s",  CAMERA_SETTINGS);

    this->cameraSnapshotResource->set_path( CAMERA_SNAPSHOT );
    this->cameraSnapshotResource->set_method_handler( "GET", get_camera_snapshot_handler );
    syslog(LOG_DEBUG, "Restbed: %s",  CAMERA_SNAPSHOT);
}

list<shared_ptr<Resource>> camera_resource::getResources() {
    list<shared_ptr<Resource>> l = {
            this->cameraSnapshotResource,
            this->cameraSettingsResource,
            this->cameraGetDimensionsResource
    };
    return l;
}
