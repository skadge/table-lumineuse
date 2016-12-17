#include <opencv2/highgui/highgui.hpp>

#include <map>
#include <array>
#include <iostream>

#include "detector.h"

using namespace std;
using namespace cv;


int main ( int argc,char **argv ) {

    Detector detector;
    Mat rawimage;

    double total_time = 0;

    for (int i = 2; i < argc; i++) {
        rawimage = imread(argv[i], 0);

        auto start = getTickCount();

        auto spot = detector.find_spot(rawimage, argv[1]);


        total_time += (getTickCount() - start) * 1000./getTickFrequency();
    }

    cout << "In average, " << total_time / (argc - 2) << "ms per image." << endl;

    return 0;
}

