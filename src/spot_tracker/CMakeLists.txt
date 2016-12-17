
add_executable( spot_tracker_still spot_tracker_still.cpp detector.cpp)
target_link_libraries( spot_tracker_still ${OpenCV_LIBRARIES} )

find_package(raspicam)
if (${raspicam_FOUND})
    include_directories(${raspicam_INCLUDE_DIRS})

    add_executable( spot_tracker spot_tracker.cpp detector.cpp)
    target_link_libraries( spot_tracker ${OpenCV_LIBRARIES} ${raspicam_CV_LIBS})
endif()



