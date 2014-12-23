#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <random>
//#include <unistd>

#include "../src/ledstrip.h"

using namespace std;
using namespace cv;

//parse command line
//returns the value of a command line param. If not found, defvalue is returned
float getParamVal ( string param,int argc,char **argv,float defvalue=-1 ) {
    int idx=-1;
    for ( int i=0; i<argc && idx==-1; i++ )
        if ( string ( argv[i] ) ==param ) idx=i;
    if ( idx==-1 ) return defvalue;
    else return atof ( argv[  idx+1] );
}

int main ( int argc,char **argv ) {


    raspicam::RaspiCam_Cv camera;

    camera.set ( CV_CAP_PROP_FRAME_WIDTH,  640 );
    camera.set ( CV_CAP_PROP_FRAME_HEIGHT, 480 );
    camera.set ( CV_CAP_PROP_BRIGHTNESS,getParamVal ( "-br",argc,argv,50 ) );
    camera.set ( CV_CAP_PROP_CONTRAST ,getParamVal ( "-co",argc,argv,50 ) );
    camera.set ( CV_CAP_PROP_SATURATION, getParamVal ( "-sa",argc,argv,50 ) );
    camera.set ( CV_CAP_PROP_GAIN, getParamVal ( "-g",argc,argv ,50 ) );
    camera.set ( CV_CAP_PROP_FORMAT, CV_8UC1 ); // greyscale
    //camera.set ( CV_CAP_PROP_FORMAT, CV_8UC3 ); // RGB
    camera.set ( CV_CAP_PROP_EXPOSURE, -1 ); // shutter speed, 0 to 100. -1 -> auto

    //camera.setSharpness ( getParamVal ( "-sh",argc,argv,0 ) );
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
        auto r = distribution(generator);
        auto g = distribution(generator);
        auto b = distribution(generator);

        leds.fill(r,g,b);
        camera.grab();
        camera.retrieve ( image );
        imwrite (string("raspicam_") + to_string(r) + "_" + to_string(g) + "_" +to_string(b) +".png",image );
    }

    camera.release();

}
