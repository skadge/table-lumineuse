cmake_minimum_required(VERSION 2.8)

project(table)

SET(CMAKE_CXX_FLAGS "-std=c++11")

FIND_PACKAGE( Boost 1.54 COMPONENTS system thread REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

find_package(PkgConfig)
pkg_check_modules(JSONCPP jsoncpp)
include_directories(${JSONCPP_INCLUDE_DIRS})

add_library(LPD8806 SHARED src/ledstrip.cpp src/lpd8806led.c)

add_executable( test_strip src/ledstrip.cpp src/test_ledstrip.cpp)
target_link_libraries( test_strip LPD8806 )

add_executable( table src/server.cpp src/table.cpp)
target_link_libraries( table LPD8806 ${Boost_LIBRARIES} ${JSONCPP_LIBRARIES} )


#add_executable( table src/table.cpp)
#target_link_libraries( table LPD8806 )
#install (TARGETS table RUNTIME DESTINATION bin)

