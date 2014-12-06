#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>


#include <opencv2/highgui/highgui.hpp> //for FileStorage

#include <iostream>

#include "detector.h"

#define PI 3.14159265

//#define DEBUG

using namespace std;
using namespace cv;

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


    auto newcamera = getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, rawimage.size(), 0); 
    undistort(rawimage, undistorted_img, cameraMatrix, distCoeffs, newcamera);

    resize(undistorted_img, small_img, Size(320,240));


}

void Detector::detect_circles(InputArray img, OutputArray circles) {

    HoughCircles(img, circles,
                 HOUGH_GRADIENT,
                 1, // dp
                 20, // min dist
                 30, // param 1
                 8, // param 2
                 7, // min radius
                 11); // max radius

#ifdef DEBUG
    cout << "Found " << circles.size() << " circles" << endl;
#endif

}

void show_hist(InputArray img) {

    int height = 200;

    Mat hist(height, 256, CV_8UC3, Scalar(255,255,255));

    vector<int> bins (255);

    int max = 0;
    for (int i = 0; i < img.size().height * img.size().width ; i++) {
        auto val = img.getMat().data[i];
        bins[val] ++;
        if (bins[val] > max) {
            max = bins[val];
        }
    }

    for (int i; i <= 255; i++) {
        line(hist, Point(i,height), Point(i, height - (int)(bins[i] * (double)(height)/max) ), Scalar(0,0,0), 1);
    }

    imshow("Histo", hist);
}


Rect Detector::circle2rect(Vec3f circle, double scaling) {
    int radius = cvRound(circle[2] * scaling);

    Rect roi = Rect(Point(circle[0] - radius ,
                          circle[1] - radius ),
                    Point(circle[0] + radius ,
                          circle[1] + radius ));

    return roi;
}

void Detector::unfold_circle(InputArray circle, Marker& marker) {

    int radius = circle.size().width / 2; // we expect the circle ROI to be squarish a this point

    vector<int> pattern(radius);
    vector<int> orientation(NB_RADII);

    for (int i = 0; i < NB_RADII; i++) {
        for (int r = 0; r < radius; r++) {
            int x = cvRound(r * cos(i * 2.f * PI / NB_RADII) + radius);
            int y = cvRound(r * sin(i * 2.f * PI / NB_RADII) + radius);
            auto val = circle.getMat().at<unsigned char>(x, y);
            pattern[r] += val;
            orientation[i] += val;
        }
    }

#ifdef DEBUG
    cout << endl << "Pattern:" << endl;
#endif
    for (int i = 0; i < radius; i++) {
        pattern[i] = pattern[i] / NB_RADII; 
#ifdef DEBUG
        cout << ( (pattern[i] > 170) ? " " : ((pattern[i] < 140) ? "-" : "~"));
#endif
    }

#ifdef DEBUG
    cout << endl << endl << "Orientation: " << endl;
#endif

    for (int i = 0; i < NB_RADII; i++) {
        orientation[i] = orientation[i] / radius; 
#ifdef DEBUG
        cout << cvRound(i * 360./NB_RADII) << " deg: " << orientation[i] << ((orientation[i] > 200) ? "  [x]":"")<< endl;
#endif
    }

    marker.valid = true;


}


Marker Detector::decode_marker(InputArray img, Vec3f approx_circle) {

    Marker marker;

    auto roi = circle2rect(approx_circle, 2);
    // make sure our ROI remains inside the image
    roi = roi & Rect(Point(0,0), img.size());

    Mat tag = img.getMat()(roi);

    //equalizeHist(tag, tag);
    //show_hist(tag);
    adaptiveThreshold(tag, tag, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY,15,0);

#ifdef DEBUG
    imshow("Tag after thresholding", tag);
#endif

    vector<Vec3f> circles;
    HoughCircles(tag, circles,
                 HOUGH_GRADIENT,
                 1, // dp
                 20, // min dist
                 30, // param 1
                 9, // param 2
                 16, // min radius
                 20); // max radius

    if (circles.empty()) return marker; // 'marker.valid' is false by default


    auto circle = circles[0];

    roi = circle2rect(circle) & Rect(Point(0,0), tag.size());
    Mat ctag = tag(roi);

    unfold_circle(ctag, marker);

#ifdef DEBUG
    Mat cimg;
    cvtColor(tag, cimg, COLOR_GRAY2BGR);
    for (auto c : circles) {
        Point center(cvRound(c[0]), cvRound(c[1]));
        int radius = cvRound(c[2]);
        // circle center
        cv::circle( cimg, center, 1, Scalar(0,255,0));
        // circle outline
        cv::circle( cimg, center, radius, Scalar(0,0,255), 1);
    }

    resize(cimg, cimg, Size(), 5, 5);
    imshow("Tag", cimg);
    waitKey(0);
#endif

    return marker;
}

vector<Marker> Detector::find_markers(cv::InputArray rawimage, const std::string& calibration) {

    vector<Marker> markers;

    Mat img, small;
    vector<Vec3f> circles;

    readCalibration(calibration);

    prepare(rawimage, img, small);
    detect_circles(small, circles);

#ifdef DEBUG
    Mat cimg;
    cvtColor(img, cimg, COLOR_GRAY2BGR);
#endif

    for (auto c : circles) {
#ifdef DEBUG
        Point center(cvRound(c[0]), cvRound(c[1]));
        int radius = cvRound(c[2]);
        // circle center
        circle( cimg, center, 3, Scalar(0,255,0), -1, 8, 0 );
        // circle outline
        circle( cimg, center, radius, Scalar(0,0,255), 3, 8, 0 );
#endif

        // back from 320x240 to 640x480
        c[0] *=2;
        c[1] *=2;
        c[2] *=2;

        Marker marker = decode_marker(img, c);

        if(marker.valid) markers.push_back(marker);

    }

    return markers;
}

