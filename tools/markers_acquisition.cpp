#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <random>
//#include <unistd>

#include "../src/ledstrip.h"

using namespace std;
using namespace cv;

int main ( int argc,char **argv ) {


    raspicam::RaspiCam_Cv camera;

    camera.set ( CV_CAP_PROP_FRAME_WIDTH,  320 );
    camera.set ( CV_CAP_PROP_FRAME_HEIGHT, 240 );
    camera.set ( CV_CAP_PROP_BRIGHTNESS, 40);
    camera.set ( CV_CAP_PROP_CONTRAST , 70 );
    camera.set ( CV_CAP_PROP_SATURATION, 30 );
    camera.set ( CV_CAP_PROP_GAIN, 50 );
    camera.set ( CV_CAP_PROP_FORMAT, CV_8UC1 ); // greyscale
    //camera.set ( CV_CAP_PROP_FORMAT, CV_8UC3 ); // RGB
    camera.set ( CV_CAP_PROP_EXPOSURE, -1 ); // shutter speed, 0 to 100. -1 -> auto

//     Camera.setSharpness ( getParamVal ( "-sh",argc,argv,0 ) );
//     if ( findParam ( "-vs",argc,argv ) !=-1 )
//         Camera.setVideoStabilization ( true );
//     Camera.setExposureCompensation ( getParamVal ( "-ev",argc,argv ,0 ) );

    cout<<"Connecting to camera"<<endl;
    if ( !camera.open() ) {
        cerr<<"Error opening camera"<<endl;
        return -1;
    }
    cout << "Connected to camera =" << camera.getId() <<endl;

    Mat image;

    Ledstrip leds = Ledstrip();
    default_random_engine generator;
    uniform_int_distribution<int> distribution(100,255);

    for (auto i = 0; i < 20; i++) {

        cout << "Image " << i << endl;
        leds.fill(distribution(generator),
                  distribution(generator),
                  distribution(generator));
        camera.grab();
        camera.retrieve ( image );
        imwrite (string("raspicam_rgb_") + to_string(i) + ".png",image );
    }

    camera.release();

}
