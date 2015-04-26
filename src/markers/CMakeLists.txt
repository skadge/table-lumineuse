add_library(table-tagdetector SHARED detector.cpp)
target_link_libraries(table-tagdetector ${OpenCV_LIBRARIES} )

install (TARGETS table-tagdetector
         LIBRARY DESTINATION lib)


add_executable( static_tags tag_detector.cpp)
target_link_libraries( static_tags table-tagdetector )

find_package(raspicam)
if (${raspicam_FOUND})
    include_directories(${raspicam_INCLUDE_DIRS})

    add_executable( markers_acquisition ../../tools/markers_acquisition.cpp)
    target_link_libraries( markers_acquisition table-ledeffects ${OpenCV_LIBRARIES} ${raspicam_CV_LIBS})

    add_executable( live_tags live_tag_detector.cpp)
    target_link_libraries( live_tags table-tagdetector ${raspicam_CV_LIBS})
endif()



