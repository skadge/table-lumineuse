
add_executable( spot_tracker_still spot_tracker_still.cpp detector.cpp http_client.cpp ../light/color.cpp)
target_link_libraries( spot_tracker_still ${OpenCV_LIBRARIES} ${Boost_LIBRARIES} )

find_package(raspicam)
if (${raspicam_FOUND})
    include_directories(${raspicam_INCLUDE_DIRS})

    add_executable( spot-tracker spot_tracker.cpp detector.cpp http_client.cpp ../light/color.cpp)
    target_link_libraries( spot-tracker ${OpenCV_LIBRARIES} ${raspicam_CV_LIBS} ${Boost_LIBRARIES})

    install (TARGETS spot-tracker
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)

    install (FILES
            ../../share/table_calib_320x240_opencv.yml
            ../../share/table_calib_640x480_opencv.yml
            DESTINATION share/spot-tracker)


endif()



