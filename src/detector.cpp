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

void Detector::unfold_circle(InputArray circle) {

    const int NB_SLICES = 32;

    int radius = circle.size().width / 2; // we expect the circle ROI to be squarish a this point

    vector<int> pattern(radius);
    vector<int> orientation(NB_SLICES);

    for (int i = 0; i < NB_SLICES; i++) {
        for (int r = 0; r < radius; r++) {
            int x = cvRound(r * cos(i * 2.f * PI / NB_SLICES) + radius);
            int y = cvRound(r * sin(i * 2.f * PI / NB_SLICES) + radius);
            auto val = circle.getMat().at<unsigned char>(x, y);
            pattern[r] += val;
            orientation[i] += val;
        }
    }

    cout << endl << "Pattern:" << endl;
    for (int i = 0; i < radius; i++) {
        pattern[i] = pattern[i] / NB_SLICES; 
        cout << ( (pattern[i] > 170) ? " " : ((pattern[i] < 140) ? "-" : "~"));
    }

    cout << endl << endl << "Orientation: " << endl;

    for (int i = 0; i < NB_SLICES; i++) {
        orientation[i] = orientation[i] / radius; 
        cout << cvRound(i * 360./NB_SLICES) << " deg: " << orientation[i] << ((orientation[i] > 200) ? "  [x]":"")<< endl;
    }


}


void Detector::decode_marker(InputArray img, Vec3f circle) {

    circle[0] *=2;
    circle[1] *=2;
    circle[2] *=2;

    auto roi = circle2rect(circle, 2);
    // make sure our ROI remains inside the image
    roi = roi & Rect(Point(0,0), img.size());

    Mat tag = img.getMat()(roi);

    //equalizeHist(tag, tag);
    //show_hist(tag);
    adaptiveThreshold(tag, tag, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY,15,0);

#ifdef DEBUG
    imshow("Raw tag", tag);
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

    if (!circles.empty()) {

        auto circle = circles[0];

        roi = circle2rect(circle) & Rect(Point(0,0), tag.size());
        Mat ctag = tag(roi);

        unfold_circle(ctag);

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


    
        //Moments m = moments(tag, true);
        //Point p(m.m10/m.m00, m.m01/m.m00);
        //cv::circle(tag, p, 1, Scalar(128,0,0));
        //Mat mask = Mat::zeros(tag.size(), CV_8UC1);
        //cv::circle(mask, p, 9, Scalar(255,255,255), -1);

        //Mat destImg;
        //tag.copyTo(destImg, mask);
        //
        resize(cimg, cimg, Size(), 5, 5);


        imshow("Tag", cimg);
#endif
    }
#ifdef DEBUG
        waitKey(0);
#endif
}

void Detector::find_markers(cv::InputArray rawimage, const std::string& calibration) {

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
        decode_marker(img, c);
    }

}

