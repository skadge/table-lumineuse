#include <opencv2/highgui/highgui.hpp>

#include <map>
#include <array>
#include <iostream>
#include <sstream>

#include "detector.h"
#include "http_client.h"
#include "../light/color.h"

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

        if (spot.x < 0) cout << "No spot!" << endl;
        else cout << "Center at (" << spot.x << ", " << spot.y << ")";

        auto color = Color::fromHSV(360 * spot.x, 1, spot.y);

        stringstream cmd;
        cmd << "/?content={\"mode\":\"PLAIN\",\"src\":{\"id\":1,\"type\":\"color\",\"value\":" << color << ",\"x\":0,\"y\":0}}";
        perform_request("localhost", "8080", cmd.str());


        total_time += (getTickCount() - start) * 1000./getTickFrequency();
    }

    cout << "In average, " << total_time / (argc - 2) << "ms per image." << endl;

    return 0;
}

