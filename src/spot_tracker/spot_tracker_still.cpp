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

    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <calib> <imgs...>" << endl;
        exit(1);
    }

    for (int i = 2; i < argc; i++) {

        cout << "Reading image " << argv[i] << "..." << endl;
        rawimage = imread(argv[i], 0);



        auto start = getTickCount();

        cout << "Looking for spots..." << endl;

        auto spots = detector.find_spots(rawimage, argv[1]);

        if (spots.empty()) cout << "No spot!" << endl;
        else {
            cout << "Found " << spots.size() << " spots" << endl;
            for (auto spot : spots) {
                cout << "Center at (" << spot.x << ", " << spot.y << ")" << endl;
            }
        }

        auto spot = *spots.begin(); // just take the first one
        auto color = Color::fromHSV(360 * spot.x, 1, spot.y);

        stringstream cmd;
        cmd << "/?content={\"mode\":\"PLAIN\",\"src\":{\"id\":1,\"type\":\"color\",\"value\":" << color << ",\"x\":0,\"y\":0}}";
        perform_request("localhost", "8080", cmd.str());


        total_time += (getTickCount() - start) * 1000./getTickFrequency();
    }

    cout << "In average, " << total_time / (argc - 2) << "ms per image." << endl;

    return 0;
}

