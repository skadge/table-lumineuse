pkg_check_modules(SFML sfml-audio)
include_directories(${SFML_INCLUDE_DIRS})

add_library(table-soundeffects SHARED soundeffect.cpp 
                                      soundlibrary.cpp 
                                      sfml_sound.cpp)

target_link_libraries(table-soundeffects ${SFML_LIBRARIES} )

install (TARGETS table-soundeffects 
         LIBRARY DESTINATION lib)



