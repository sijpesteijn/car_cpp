cmake_minimum_required(VERSION 3.5)
project(cpu)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./)

set(CMAKE_CXX_STANDARD 11)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( /usr/local/include/restbed )
include_directories( /usr/local/include/openssl )
include_directories( /usr/local/include/jansson )

if( APPLE )
    link_directories(/usr/local/lib)
    link_directories(/usr/local/opt/openssl/lib)
    set(SOURCE_FILES src/main.cpp src/util/log.h src/util/log.cpp src/util/util.h src/util/util.cpp src/util/observer_group.h src/util/observer_group.cpp src/util/race_builder.h src/util/race_builder.cpp src/util/opencv_line.h src/util/opencv_line.cpp src/util/pid_controller.h src/util/pid_controller.cpp src/util/settings.h src/util/settings.cpp
            src/domain/pwm.h src/domain/pwmmock.cpp src/domain/steer.h src/domain/steer.cpp src/domain/engine.h src/domain/engine.cpp src/domain/car.h src/domain/car.cpp src/domain/camera.h src/domain/camera.cpp
            src/observers/observer.h src/observers/traffic_light.h src/observers/traffic_light.cpp src/observers/lane_detection.h src/observers/lane_detection.cpp src/observers/finish_detection.h src/observers/finish_detection.cpp
            src/races/race.h src/races/race.cpp
            src/rest/rest.h src/rest/rest.cpp src/rest/race_resource.h src/rest/race_resource.cpp src/rest/camera_resource.h src/rest/camera_resource.cpp)
else()
    set(SOURCE_FILES src/main.cpp src/util/log.h src/util/log.cpp src/util/util.h src/util/util.cpp src/util/observer_group.h src/util/observer_group.cpp src/util/race_builder.h src/util/race_builder.cpp src/util/opencv_line.h src/util/opencv_line.cpp src/util/pid_controller.h src/util/pid_controller.cpp src/util/settings.h src/util/settings.cpp
            src/domain/pwm.h src/domain/pwmmock.cpp src/domain/steer.h src/domain/steer.cpp src/domain/engine.h src/domain/engine.cpp src/domain/car.h src/domain/car.cpp src/domain/camera.h src/domain/camera.cpp
            src/observers/observer.h src/observers/traffic_light.h src/observers/traffic_light.cpp src/observers/lane_detection.h src/observers/lane_detection.cpp src/observers/finish_detection.h src/observers/finish_detection.cpp
            src/races/race.h src/races/race.cpp
            src/rest/rest.h src/rest/rest.cpp src/rest/race_resource.h src/rest/race_resource.cpp src/rest/camera_resource.h src/rest/camera_resource.cpp)
endif()

set(LIBS ${OpenCV_LIBS} curl restbed jansson ssl pthread crypto )
add_executable(cpu ${SOURCE_FILES})
target_link_libraries( cpu ${LIBS})
