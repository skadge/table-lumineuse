#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <chilitags.hpp>

using namespace std;
using namespace cv;
using namespace chilitags;

int main ( int argc,char **argv ) {
    Chilitags chilitags;

    // The detection is not perfect, so if a tag is not detected during one frame,
    // the tag will shortly disappears, which results in flickering.
    // To address this, Chilitags "cheats" by keeping tags for n frames
    // at the same position. When tags disappear for more than 5 frames,
    // Chilitags actually removes it.
    // Here, we cancel this to show the raw detection results.
    chilitags.setFilter(0, 0.);
    chilitags.setPerformance(Chilitags::FASTER);

    raspicam::RaspiCam_Cv Camera;

    Camera.set ( CV_CAP_PROP_FRAME_WIDTH,  640 );
    Camera.set ( CV_CAP_PROP_FRAME_HEIGHT, 480 );
    Camera.set ( CV_CAP_PROP_BRIGHTNESS, 50);
    Camera.set ( CV_CAP_PROP_CONTRAST , 50 );
    Camera.set ( CV_CAP_PROP_SATURATION, 50 );
    Camera.set ( CV_CAP_PROP_GAIN, 50 );
    Camera.set ( CV_CAP_PROP_FORMAT, CV_8UC1 ); // greyscale
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

    cv::Mat image;

    double time_=cv::getTickCount();

    for (auto i = 0; i < 50; i++) {

        Camera.grab();
        Camera.retrieve ( image );

        auto tags = chilitags.find(image);
        cout << "Found " << tags.size() << " tags\n";
    }

    double secondsElapsed= double ( getTickCount()-time_ ) /double ( getTickFrequency() ); //time in second
    cout<< (float) secondsElapsed / 50.f<<" seconds per frame: FPS = "<< 50.f /secondsElapsed << endl;
    imwrite ( "raspicam.png",image );
    Camera.release();

}
