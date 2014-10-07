cmake_minimum_required(VERSION 2.8)

project(table)

#add_definitions(-std=c++11)
FIND_PACKAGE( Boost 1.54 COMPONENTS system thread REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

find_package(PkgConfig)
pkg_check_modules(JSONCPP jsoncpp)
include_directories(${JSONCPP_INCLUDE_DIRS})

add_library(LPD8806 SHARED src/lpd8806led.c src/ledstrip.cpp )

add_executable( test_strip src/test_ledstrip.cpp)
target_link_libraries( test_strip LPD8806 )

add_executable( test_server src/server.cpp)
target_link_libraries( test_server ${Boost_LIBRARIES} ${JSONCPP_LIBRARIES} )


#add_executable( table src/table.cpp)
#target_link_libraries( table LPD8806 )
#install (TARGETS table RUNTIME DESTINATION bin)

