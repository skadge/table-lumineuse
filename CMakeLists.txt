cmake_minimum_required(VERSION 2.8)

project(table)

SET(CMAKE_CXX_FLAGS "-std=c++11")

find_package( Boost 1.54 COMPONENTS system thread filesystem REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )


find_package(PkgConfig)
pkg_check_modules(JSONCPP jsoncpp)
include_directories(${JSONCPP_INCLUDE_DIRS})

# LIGHT EFFECTS
add_subdirectory(src/light)

# SOUND EFFECTS
add_subdirectory(src/sound)

# SPOT TRACKER
option(WITH_SPOT_TRACKING "Compile the 'spot tracker' executable -- requires OpenCV" OFF)

if(WITH_SPOT_TRACKING)
    find_package(OpenCV)
    if (${OpenCV_FOUND})
        include_directories(${OpenCV_INCLUDE_DIRS})
        add_subdirectory(src/spot_tracker)
    else()
        message(FATAL "OpenCV required to compile the spot tracker")
    endif()
endif()

#
add_library(table-gpio SHARED src/gpio.c)

#
file(GLOB http_server src/http_server/*.cpp)

###############
# MAIN BINARY #
###############

if(WITH_SOUNDS)
   add_definitions(-DWITH_SOUNDS)
endif()

add_executable( table src/main.cpp 
                      src/handler.cpp 
                      src/table.cpp
                      src/sources.cpp
                      ${http_server})
if(WITH_SOUNDS)
    target_link_libraries( table table-gpio table-ledeffects table-soundeffects ${Boost_LIBRARIES} ${JSONCPP_LIBRARIES} )
else()
    target_link_libraries( table table-gpio table-ledeffects ${Boost_LIBRARIES} ${JSONCPP_LIBRARIES} )
endif()
#################

install (TARGETS table table-gpio
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib)

 install(DIRECTORY share
	 DESTINATION ${CMAKE_INSTALL_PREFIX})
