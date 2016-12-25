#ifndef _DETECTOR_H
#define _DETECTOR_H

#include <opencv2/core/core.hpp>
#include <map>
#include <set>
#include <string>

const cv::Rect TABLE_ROI_640_480(30, 38, 609, 325); // x, y, width, height
const cv::Rect TABLE_ROI_320_240(0, 13, 319, 177); // x, y, width, height

struct pointcmp {
    bool operator() (const cv::Point2f& p1, const cv::Point2f& p2) const
    {if (p1.x == p2.x) return p1.y < p2.y; else return p1.x < p2.y;}
};

class Detector {

public:

    Detector();

    cv::Size readCalibration(const std::string &filename);

    void prepare(cv::InputArray rawimage, cv::OutputArray undistorted_img, cv::OutputArray small_img);

    /**
     * Returns a set of two floats between 0 and 1 representing the position (x,y) of a
     * bright spot, normalized over the surface of the light table.
     */
    std::set<cv::Point2f, pointcmp> find_spots(cv::InputArray img, const std::string& calibration);

private:


    // The kernel used for dilatation and erosion
    const cv::Mat MORPHOLOGICAL_ELEMENT;

    cv::Mat distCoeffs, cameraMatrix;


};

#endif
