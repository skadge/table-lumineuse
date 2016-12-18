#include <raspicam/raspicam_cv.h>

#include <map>
#include <array>
#include <iostream>

#include "detector.h"

using namespace std;
using namespace cv;


int main ( int argc,char **argv ) {

    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <calibration>" << endl;
        return 1;
    }

    raspicam::RaspiCam_Cv camera;

    camera.set ( CV_CAP_PROP_FRAME_WIDTH,  320 );
    camera.set ( CV_CAP_PROP_FRAME_HEIGHT, 240 );
    camera.set ( CV_CAP_PROP_BRIGHTNESS, 50);
    camera.set ( CV_CAP_PROP_CONTRAST , 50 );
    camera.set ( CV_CAP_PROP_SATURATION, 50 );
    camera.set ( CV_CAP_PROP_GAIN, 50 );
    camera.set ( CV_CAP_PROP_FORMAT, CV_8UC1 ); // greyscale
    //camera.set ( CV_CAP_PROP_FORMAT, CV_8UC3 ); // RGB
    camera.set ( CV_CAP_PROP_EXPOSURE, -1 ); // shutter speed, 0 to 100. -1 -> auto

    //    camera.setSharpness ( getParamVal ( "-sh",argc,argv,0 ) );
    //    if ( findParam ( "-vs",argc,argv ) !=-1 )
    //        camera.setVideoStabilization ( true );
    //    camera.setExposureCompensation ( getParamVal ( "-ev",argc,argv ,0 ) );

    cout<<"Connecting to camera"<<endl;
    if ( !camera.open() ) {
        cerr<<"Error opening camera"<<endl;
        return -1;
    }
    cout<<"Connected to camera ="<<camera.getId() <<endl;


    Detector detector;
    Mat rawimage;

    while (true) {
        auto start = getTickCount();

        camera.grab();
        camera.retrieve ( rawimage );

        auto camera_ticks = getTickCount();


        auto spot = detector.find_spot(rawimage, argv[1]);

        cout << "Center at (" << spot.x << ", " << spot.y << ")";

        auto end = getTickCount();

        auto camera_time = (camera_ticks - start) * 1000./getTickFrequency();
        auto detection_time = (end - camera_ticks) * 1000./getTickFrequency();
        cout << camera_time + detection_time << "ms for this frame";
        cout << " (camera: " << camera_time << "ms, detection: ";
        cout << detection_time << "ms)." << endl;
    }
    return 0;
}

