
option(WITH_SOUNDS "Enable sound effects -- requires the SFML library" OFF)

if(WITH_SOUNDS)
   add_definitions(-DWITH_SOUNDS)
   pkg_check_modules(SFML sfml-audio REQUIRED)
   include_directories(${SFML_INCLUDE_DIRS})
   
   add_library(table-soundeffects SHARED soundeffect.cpp 
                                         soundlibrary.cpp 
                                         sfml_sound.cpp)
   
   target_link_libraries(table-soundeffects ${SFML_LIBRARIES} )
   
   install (TARGETS table-soundeffects 
            LIBRARY DESTINATION lib)
endif()


