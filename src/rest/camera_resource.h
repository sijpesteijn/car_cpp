//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_CAMERA_RESOURCE_H
#define CARMAGEDDON_CAMERA_RESOURCE_H


#include "../domain/camera.h"
#include "carmageddon_resource.h"
#include <restbed>
#include <list>

class camera_resource: public carmageddon_resource {
public:
    camera_resource(Camera *camera);
    std::list<std::shared_ptr<restbed::Resource>> getResources();
private:
    std::shared_ptr<restbed::Resource> cameraSettingsResource = std::make_shared< restbed::Resource >( );
    std::shared_ptr<restbed::Resource> cameraGetDimensionsResource = std::make_shared< restbed::Resource >( );
    std::shared_ptr<restbed::Resource> cameraSnapshotResource = std::make_shared< restbed::Resource >( );

};

#endif //CARMAGEDDON_CAMERA_RESOURCE_H
