cmake_minimum_required(VERSION 2.8)

project(table)

SET(CMAKE_CXX_FLAGS "-std=c++11")

find_package( Boost 1.54 COMPONENTS system thread REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(raspicam REQUIRED)
include_directories(${raspicam_INCLUDE_DIRS})

find_package(PkgConfig)
pkg_check_modules(JSONCPP jsoncpp)
include_directories(${JSONCPP_INCLUDE_DIRS})

pkg_check_modules(CHILITAGS chilitags)
include_directories(${CHILITAGS_INCLUDE_DIRS})


add_library(LPD8806 SHARED src/ledstrip.cpp src/lpd8806led.c)

add_executable( test_strip src/ledstrip.cpp src/test_ledstrip.cpp)
target_link_libraries( test_strip LPD8806 )


file(GLOB http_server src/http_server/*.cpp)

add_executable( table src/server.cpp src/table.cpp ${http_server})
target_link_libraries( table LPD8806 ${Boost_LIBRARIES} ${JSONCPP_LIBRARIES} )

#add_executable( server ${http_server})
#target_link_libraries( server ${Boost_LIBRARIES} )

add_executable( tags src/chilitags.cpp)
target_link_libraries( tags ${CHILITAGS_LDFLAGS} ${OpenCV_LIBRARIES} ${raspicam_CV_LIBS})



#add_executable( table src/table.cpp)
#target_link_libraries( table LPD8806 )
#install (TARGETS table RUNTIME DESTINATION bin)

