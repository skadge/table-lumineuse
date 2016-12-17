#ifndef _DETECTOR_H
#define _DETECTOR_H

#include <opencv2/core/core.hpp>
#include <map>
#include <vector>
#include <string>

const cv::Rect TABLE_ROI_640_480(30, 38, 609, 325); // x, y, width, height
const cv::Rect TABLE_ROI_320_240(0, 13, 319, 177); // x, y, width, height

class Detector {

public:

    Detector();

    cv::Size readCalibration(const std::string &filename);

    void prepare(cv::InputArray rawimage, cv::OutputArray undistorted_img, cv::OutputArray small_img);

    /**
     * Returns two floats between 0 and 1 representing the position (x,y) of a
     * bright spot, normalized over the surface of the light table.
     */
    cv::Point2f find_spot(cv::InputArray img, const std::string& calibration);

private:


    // The kernel used for dilatation and erosion
    const cv::Mat MORPHOLOGICAL_ELEMENT;

    cv::Mat distCoeffs, cameraMatrix;


};

#endif
