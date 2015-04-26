#include <raspicam/raspicam_cv.h>

#include <map>
#include <array>
#include <iostream>

#include "detector.h"

using namespace std;
using namespace cv;

int main ( int argc,char **argv ) {

    if (argc != 2) {
        cout << "Usage: live_tag_detector <calibration>" << endl;
        return 1;
    }

    raspicam::RaspiCam_Cv Camera;

    Camera.set ( CV_CAP_PROP_FRAME_WIDTH,  640 );
    Camera.set ( CV_CAP_PROP_FRAME_HEIGHT, 480 );
    Camera.set ( CV_CAP_PROP_BRIGHTNESS, 50);
    Camera.set ( CV_CAP_PROP_CONTRAST , 50 );
    Camera.set ( CV_CAP_PROP_SATURATION, 50 );
    Camera.set ( CV_CAP_PROP_GAIN, 50 );
    Camera.set ( CV_CAP_PROP_FORMAT, CV_8UC1 ); // greyscale
    //camera.set ( CV_CAP_PROP_FORMAT, CV_8UC3 ); // RGB
    Camera.set ( CV_CAP_PROP_EXPOSURE, -1 ); // shutter speed, 0 to 100. -1 -> auto

//     Camera.setSharpness ( getParamVal ( "-sh",argc,argv,0 ) );
//     if ( findParam ( "-vs",argc,argv ) !=-1 )
//         Camera.setVideoStabilization ( true );
//     Camera.setExposureCompensation ( getParamVal ( "-ev",argc,argv ,0 ) );

    cout<<"Connecting to camera"<<endl;
    if ( !Camera.open() ) {
        cerr<<"Error opening camera"<<endl;
        return -1;
    }
    cout<<"Connected to camera ="<<Camera.getId() <<endl;


    Detector detector;
    Mat rawimage;

    while (true) {
        auto start = getTickCount();

        Camera.grab();
        Camera.retrieve ( rawimage );

        auto camera_ticks = getTickCount();

        auto markers = detector.find_markers(rawimage, argv[1]);

        for (auto marker : markers) {
            cout << "Marker " << (int) marker.id;
            cout << " at (" << marker.x << ", " << marker.y << ")";
            cout << ", theta: " << marker.theta << "deg" << endl;
        }

        auto end = getTickCount();

        auto camera_time = (camera_ticks - start) * 1000./getTickFrequency();
        auto detection_time = (end - camera_ticks) * 1000./getTickFrequency();
        cout << camera_time + detection_time << "ms for this frame";
        cout << " (camera: " << camera_time << "ms, detection: ";
        cout << detection_time << "ms)." << endl;
    }


    return 0;
}

