cmake_minimum_required(VERSION 2.8)

project(table)

SET(CMAKE_CXX_FLAGS "-std=c++11")

find_package( Boost 1.54 COMPONENTS system thread filesystem REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

find_package(OpenCV)
if (${OpenCV_FOUND})
    add_definitions(-DHAS_OPENCV)
    include_directories(${OpenCV_INCLUDE_DIRS})
endif()

find_package(PkgConfig)
pkg_check_modules(JSONCPP jsoncpp)
include_directories(${JSONCPP_INCLUDE_DIRS})

# LIGHT EFFECTS
add_subdirectory(src/light)

# SOUND EFFECTS
add_subdirectory(src/sound)

# MARKER TRACKING
add_subdirectory(src/markers)

#
add_library(table-gpio SHARED src/gpio.c)

#
file(GLOB http_server src/http_server/*.cpp)

###############
# MAIN BINARY #
###############

add_executable( table src/main.cpp 
                      src/handler.cpp 
                      src/table.cpp
                      src/sources.cpp
                      ${http_server})
target_link_libraries( table table-gpio table-ledeffects table-soundeffects ${Boost_LIBRARIES} ${JSONCPP_LIBRARIES} )

#################

install (TARGETS table table-gpio
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib)

