cmake_minimum_required(VERSION 2.8)

project(table)

SET(CMAKE_CXX_FLAGS "-std=c++11")

find_package( Boost 1.54 COMPONENTS system thread REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(PkgConfig)
pkg_check_modules(JSONCPP jsoncpp)
include_directories(${JSONCPP_INCLUDE_DIRS})


add_library(LPD8806 SHARED src/ledstrip.cpp src/lpd8806led.c src/color.cpp)

add_library(gpio SHARED src/gpio.c)

add_library(tagdetector SHARED src/detector.cpp)
target_link_libraries( tagdetector ${OpenCV_LIBRARIES} )

file(GLOB http_server src/http_server/*.cpp)

add_executable( table src/main.cpp src/handler.cpp src/table.cpp ${http_server})
target_link_libraries( table gpio LPD8806 ${Boost_LIBRARIES} ${JSONCPP_LIBRARIES} )
install (TARGETS table gpio tagdetector LPD8806 RUNTIME DESTINATION bin
                                    LIBRARY DESTINATION lib)

# tests
add_executable( test_strip src/ledstrip.cpp src/test_ledstrip.cpp)
target_link_libraries( test_strip LPD8806 )


add_executable( static_tags src/tag_detector.cpp)
target_link_libraries( static_tags tagdetector )



find_package(raspicam)
if (${raspicam_FOUND})
    include_directories(${raspicam_INCLUDE_DIRS})

    add_executable( markers_acquisition tools/markers_acquisition.cpp)
    target_link_libraries( markers_acquisition LPD8806 ${OpenCV_LIBRARIES} ${raspicam_CV_LIBS})

    add_executable( live_tags src/live_tag_detector.cpp)
    target_link_libraries( live_tags tagdetector ${raspicam_CV_LIBS})
endif()


