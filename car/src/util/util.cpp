//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#include "util.h"
//#include "../races/race.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

static const string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "0123456789+/";

string base64_encoder(unsigned char const* bytes_to_encode, unsigned int in_len) {
    string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; (i <4) ; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i)
    {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while((i++ < 3))
            ret += '=';

    }

    return ret;

}

multimap< string, string > build_websocket_handshake_response_headers( const shared_ptr< const Request >& request, const string secret_key )
{
    auto key = request->get_header( "Sec-WebSocket-Key" );
    key.append( secret_key );

    Byte hash[ SHA_DIGEST_LENGTH ];
    SHA1( reinterpret_cast< const unsigned char* >( key.data( ) ), key.length( ), hash );

    string encoded = base64_encoder( hash, SHA_DIGEST_LENGTH );
    cout << encoded << endl;

    multimap< string, string > headers;
    headers.insert( make_pair( "Upgrade", "websocket" ) );
    headers.insert( make_pair( "Connection", "Upgrade" ) );
    headers.insert( make_pair( "Sec-WebSocket-Accept", encoded) );

    return headers;
}

//void* checkObservers(void* params) {
//    race *r = (race*) params;
//    bool run = true;
//    while(run && r->car->isAutonomous()) {
//        Mat snapshot = r->camera->getFrame();
//        if (!r->isRunning() && r->car->isAutonomous()) {
//            for (observer_group *group: r->groups) {
//                group->processSnapshot(snapshot);
//            }
//        }
//        if (r->isRunning() && r->car->isAutonomous()) {
//            for (std::list<observer_group*>::iterator it= r->groups.begin(); it != r->groups.end(); ++it) {
//                observer_group *group = *it;
//                if (group->isRunning() && group->isFinished()) {
//                    group->setRunning(false);
//                    it = ++it;
//                    if (it != r->groups.end()) {
//                        group = *it;
//                        group->setRunning(true);
//                        group->processSnapshot(snapshot);
//                    } else {
//                        r->setRunning(false);
//                    }
//                } else {
//                    group->processSnapshot(snapshot);
//                }
//            }
//        }
//        this_thread::sleep_for(std::chrono::milliseconds(r->camera->observers_delay));
//    }
//    return NULL;
//}

void contrast_stretching(Mat src, Mat dst, int min, int max, int index) {
    int norm[256];
    if (max <= min) {
        Mat_<Vec3b>::const_iterator it = src.begin<Vec3b>();
        Mat_<Vec3b>::const_iterator itend = src.end<Vec3b>();
        Mat_<Vec3b>::iterator itout = src.end<Vec3b>();

        for ( ; it != itend; ++it, ++itout) {
            Vec3b color1 = *it;
            color1[index]= 255/2;
            *itout = color1;
        }
    } else {
        int i=0;
        for (i=0; i< min; i++) {
            norm[i] = 0;
        }
        for (i=min; i < max; i++) {
            norm[i] = ((i -min) * 255/((max-min) + 0.5));
        }

        for (i = max; i< 255 + 1; i++) {
            norm[i] = 255;
        }

        Mat_<Vec3b>::const_iterator it = src.begin<Vec3b>();
        Mat_<Vec3b>::const_iterator itend = src.end<Vec3b>();
        Mat_<Vec3b>::iterator itout = dst.begin<Vec3b>();

        for(; it != itend; ++it, ++itout) {
            Vec3b color = *it;
            Vec3b color1 = *itout;
            color1[index]=norm[color[index]];
            *itout = color1;
        }
    }
}