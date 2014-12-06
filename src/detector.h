#ifndef _DETECTOR_H
#define _DETECTOR_H

#include <opencv2/core/core.hpp>
#include <map>
#include <vector>
#include <string>

struct Marker {
    bool valid = false;
    unsigned char id;
    float theta;
    unsigned int x, y;
};

class Detector {

public:
    cv::Size readCalibration(const std::string &filename);

    void prepare(cv::InputArray rawimage, cv::OutputArray undistorted_img, cv::OutputArray small_img);

    void detect_circles(cv::InputArray img, cv::OutputArray circles);


    void unfold_circle(cv::InputArray circle, Marker& marker);

    Marker decode_marker(cv::InputArray img, cv::Vec3f circle);

    std::vector<Marker> find_markers(cv::InputArray img, const std::string& calibration);

private:

    // nb of sample points taken on a radius. MUST BE < 32 (since the patterns
    // are encoded as a 32bit int)
    const int RADIUS_RESOLUTION = 25;

    // defines the ID and patterns of the various markers.
    // ' ' for white, '-' for black, 1 char = 1 mm, higher bits towards center
    const std::map<unsigned char, int32_t> MARKERS {
        {1, 0b0000001111111100111111111},
        {2, 0b0000000000111111111111111},
        {3, 0b1111111100000000111111111},
        {4, 0b1111111111111100111111111}
        };

    // how many samples to take on a circle (ie how many radii) to recognize
    // the tag. Higher values increase the accuracy of orientation
    const int NB_RADII = 32;

    cv::Rect circle2rect(cv::Vec3f circle, double scaling = 1.0);
    unsigned char count_bits(int32_t val) {
        unsigned char count=0;
        while(val!=0){
              val = val & (val-1);
              count++;
        }
        return count;
    }
    cv::Mat distCoeffs, cameraMatrix;


};

#endif
