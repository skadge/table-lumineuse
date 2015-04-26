#include <opencv2/highgui/highgui.hpp>

#include <map>
#include <array>
#include <iostream>

#include "detector.h"

using namespace std;
using namespace cv;

// id, x, y, theta (deg)
const map<char, array<int, 3>> test_markers {
    {1, {{81, 223, 285}}},
    {2, {{441, 249, 253}}},
    {3, {{197, 67, 225}}},
    {4, {{583, 45, 278}}}
    };

int EPSILON = 20;

int main ( int argc,char **argv ) {

    Detector detector;
    Mat rawimage;

    int nb_markers = (argc - 2) * test_markers.size();

    int found_markers = 0;
    int false_positive = 0;

    double total_time = 0;

    for (int i = 2; i < argc; i++) {
        rawimage = imread(argv[i], 0);

        auto start = getTickCount();

        auto markers = detector.find_markers(rawimage, argv[1]);
        for (auto marker : markers) {

            // does the ID exist?
            if (test_markers.find(marker.id) == test_markers.end()) continue;

            auto target = test_markers.at(marker.id);

            if (   abs(target[0] - (int) marker.x) < EPSILON
                && abs(target[1] - (int) marker.y) < EPSILON
                && abs(target[2] - marker.theta) < EPSILON) {
                found_markers++;
            }
            else {
                false_positive++;
            }

        }


        total_time += (getTickCount() - start) * 1000./getTickFrequency();
    }

    cout << "Found " << found_markers << " out of " << nb_markers << " (" << 100. * found_markers / nb_markers << "%), and " << false_positive << " false positives" << endl;
    cout << "In average, " << total_time / (argc - 2) << "ms per image." << endl;

    return 0;
}

