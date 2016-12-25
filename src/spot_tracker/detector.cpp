#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>


#include <opencv2/highgui/highgui.hpp> //for FileStorage

#include <vector>
#include <bitset>
#include <iostream>

#include "detector.h"

#define PI 3.14159265

//#define DEBUG

using namespace std;
using namespace cv;

Detector::Detector() :
        MORPHOLOGICAL_ELEMENT(getStructuringElement( 
                MORPH_ELLIPSE, Size( 3, 3 ), Point(1, 1)))
{
}

Size Detector::readCalibration(const string &filename) {
    Size size;
    FileStorage fs(filename, cv::FileStorage::READ);
    fs["image_width"]             >> size.width;
    fs["image_height"]            >> size.height;
    fs["distortion_coefficients"] >> distCoeffs;
    fs["camera_matrix"]           >> cameraMatrix;

    return size;
}

void Detector::prepare(InputArray rawimage, OutputArray undistorted_img, OutputArray small_img) {

    Mat tmp_img;

    auto newcamera = getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, rawimage.size(), 0); 
    undistort(rawimage, tmp_img, cameraMatrix, distCoeffs, newcamera);

    Rect roi;
    if(rawimage.size().width==320) {roi=TABLE_ROI_320_240;}
    else {roi=TABLE_ROI_640_480;}
    tmp_img(roi).copyTo(undistorted_img);


    resize(undistorted_img, small_img, Size(), 0.5, 0.5);


}

set<Point2f, pointcmp> Detector::find_spots(cv::InputArray rawimage, const std::string& calibration) {

    Mat img, small;
    set<Point2f, pointcmp> result;

    readCalibration(calibration);

    //imshow("Raw", rawimage);
    prepare(rawimage, img, small);

    //imshow("Undistorded", img);

    threshold(img,img, 210, 255, cv::THRESH_BINARY);

    //imshow("After threshodling", img);

    erode(img, img, MORPHOLOGICAL_ELEMENT);

    //imshow("After erosion", img);
    //waitKey();
    
    vector<vector<Point>> contours;
    cv::findContours(img, contours, cv::noArray(), CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    
    for (auto contour : contours) {
        float x=0, y=0;
        for (auto p : contour) {
            x += p.x; y += p.y;
        }
        result.insert(Point2f((x/contour.size())/img.size().width,
                              (y/contour.size())/img.size().height));
    }

    return result;

}

