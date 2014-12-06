#include <opencv2/highgui/highgui.hpp>

#include <iostream>

#include "detector.h"

using namespace std;
using namespace cv;

int main ( int argc,char **argv ) {

    Detector detector;
    Mat rawimage;
    rawimage = imread(argv[2], 0);

    auto start = getTickCount();

    detector.find_markers(rawimage, argv[1]);

    cout << "It took " << (getTickCount() - start) * 1000./getTickFrequency() << "ms" << endl;

    return 0;
}

