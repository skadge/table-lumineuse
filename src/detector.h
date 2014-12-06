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

    // defines the ID and patterns of the various markers.
    // ' ' for white, '-' for black
    const std::map<char, std::string> markers {
        {1, "     -----     -----"},
        {2, "            --------"},
        {3, "-------      -------"}
        };

    // how many samples to take on a circle (ie how many radii) to recognize
    // the tag. Higher values increase the accuracy of orientation
    const int NB_RADII = 32;

    cv::Rect circle2rect(cv::Vec3f circle, double scaling = 1.0);
    cv::Mat distCoeffs, cameraMatrix;


};

#endif
