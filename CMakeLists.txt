cmake_minimum_required(VERSION 2.8)

project(table)

#add_definitions(-std=c++11)

add_library(LPD8806 SHARED src/lpd8806led.c src/ledstrip.cpp )

add_executable( test_strip src/test_ledstrip.cpp)
target_link_libraries( test_strip LPD8806 )

#add_executable( table src/table.cpp)
#target_link_libraries( table LPD8806 )
#install (TARGETS table RUNTIME DESTINATION bin)

