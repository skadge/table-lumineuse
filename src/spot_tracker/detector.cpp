#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>


#include <opencv2/highgui/highgui.hpp> //for FileStorage

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

Point2f Detector::find_spot(cv::InputArray rawimage, const std::string& calibration) {

    Mat img, small;
    vector<Vec3f> circles;

    readCalibration(calibration);

    //imshow("Raw", rawimage);
    prepare(rawimage, img, small);

    //imshow("Undistorded", img);

    threshold(img,img, 210, 255, cv::THRESH_BINARY);

    //imshow("After threshodling", img);

    erode(img, img, MORPHOLOGICAL_ELEMENT);

    //imshow("After erosion", img);
    
    auto mm = moments(img, true);

    Point2i center(mm.m10/mm.m00, mm.m01/mm.m00);
    //cout << center << endl;
    //waitKey();

    return {(float)center.x/img.size().width, (float)center.y/img.size().height};

}

