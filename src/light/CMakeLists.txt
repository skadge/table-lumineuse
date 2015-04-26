
add_library(table-ledeffects SHARED ledstrip.cpp 
                              lpd8806led.c 
                              color.cpp 
                              fade.cpp 
                              noise.cpp)

target_link_libraries(table-ledeffects ${OpenCV_LIBRARIES} )

install (TARGETS table-ledeffects
         LIBRARY DESTINATION lib)


# tests
add_executable( test_strip ledstrip.cpp test_ledstrip.cpp)
target_link_libraries( test_strip table-ledeffects )



