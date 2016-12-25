#include <raspicam/raspicam_cv.h>

#include <chrono>
#include <thread>
#include <map>
#include <array>
#include <iostream>
#include <sstream>

#include "detector.h"
#include "http_client.h"
#include "../light/color.h"

using namespace std;
using namespace cv;

static const double MAIN_LOOP_FPS=10;
static const chrono::milliseconds main_loop_duration{(int) (1000/MAIN_LOOP_FPS)};

const double EPSILON=.005; // minimum 0.5% of difference with previous spot to send color update

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

    Color prev_color;
    Point2f prev_spot;

    auto start = chrono::high_resolution_clock::now();
    auto end = start;
    chrono::milliseconds dt{0};


    while (true) {

        camera.grab();
        camera.retrieve ( rawimage );

        auto spots = detector.find_spots(rawimage, argv[1]);


        if (spots.empty()) {
            //cout << "No spot!" << endl;
        }
        else {
            auto spot = *spots.begin(); // just take the first spot
            if (abs(spot.x - prev_spot.x) < EPSILON && abs(spot.y - prev_spot.y) < EPSILON) {
                //cout << "Spot too close from previous. Not doing anything." << endl;
            }
            else {
                //cout << "Center at (" << spot.x << ", " << spot.y << ")";

                auto color = Color::fromHSV(360 * spot.x, 1, spot.y);

                if (color != prev_color) {
                    prev_color = color;

                    stringstream cmd;
                    cmd << "/?content={\"mode\":\"PLAIN\",\"src\":{\"id\":1,\"type\":\"color\",\"value\":" << color << ",\"x\":0,\"y\":0}}";
                    perform_request("localhost", "8080", cmd.str());
                }
            }
        }

        prev_spot = spot;

        end = chrono::high_resolution_clock::now();

        this_thread::sleep_for(main_loop_duration - (end - start));

        start = chrono::high_resolution_clock::now();
    }
    return 0;
}

