cmake_minimum_required(VERSION 3.5)
project(carmageddon)

set(CMAKE_CXX_STANDARD 11)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( /usr/local/include/restbed )
include_directories( /usr/local/include/openssl )
include_directories( /usr/local/include/jansson )

if( APPLE )
    link_directories(/usr/local/lib)
    link_directories(/usr/local/opt/openssl/lib)
endif()

set(SOURCE_FILES src/main.cpp src/domain/camera.h src/domain/camera.cpp src/domain/pwm.h src/domain/pwmmock.cpp src/domain/steer.h src/domain/steer.cpp src/domain/engine.h src/domain/engine.cpp src/domain/car.h src/domain/car.cpp src/rest/carmageddon_resource.h src/rest/camera_resource.h src/rest/camera_resource.cpp src/rest/car_resource.cpp src/rest/car_resource.h src/rest/rest.cpp src/rest/rest.h src/rest/carstatus_handler.cpp src/rest/carstatus_handler.h src/util/util.cpp src/util/util.h src/rest/lifeline_handler.cpp src/rest/lifeline_handler.h src/rest/observer_handler.cpp src/rest/observer_handler.h src/rest/observer_status_handler.cpp src/rest/observer_status_handler.h src/cpu.cpp src/cpu.h src/domain/observer.h src/observers/traffic_light.cpp src/observers/traffic_light.h src/observers/lane_detection.cpp src/observers/lane_detection.h src/observers/finish_detection.cpp src/observers/finish_detection.h src/races/drag_race.cpp src/races/drag_race.h src/races/track_race.cpp src/races/track_race.h src/races/race.h src/rest/race_resource.cpp src/rest/race_resource.h src/util/settings.cpp src/util/settings.h)

set(LIBS ${OpenCV_LIBS} restbed jansson ssl pthread crypto )
add_executable(carmageddon ${SOURCE_FILES})
target_link_libraries( carmageddon ${LIBS})
