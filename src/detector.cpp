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
        TABLE_ROI(30, 38, 609, 325), // x, y, width, height
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

    tmp_img(TABLE_ROI).copyTo(undistorted_img);

    resize(undistorted_img, small_img, Size(), 0.5, 0.5);


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

    vector<int> pattern(RADIUS_RESOLUTION);
    int32_t encoded_pattern = 0;
    vector<int> orientation(NB_RADII);

    double radius_scaling = (double) radius / RADIUS_RESOLUTION;

    for (int i = 0; i < NB_RADII; i++) {
        for (int r = 0; r < RADIUS_RESOLUTION; r++) {
            int x = cvRound(r * radius_scaling * cos(i * 2.f * PI / NB_RADII) + radius);
            int y = cvRound(r * radius_scaling * sin(i * 2.f * PI / NB_RADII) + radius);
            auto val = circle.getMat().at<unsigned char>(x, y);
            pattern[r] += val;
            orientation[i] += val;
        }
    }

#ifdef DEBUG
    cout << endl << "Pattern:" << endl;
#endif
    unsigned char nb_fuzzy = 0;
    for (int i = 0; i < RADIUS_RESOLUTION; i++) {
        pattern[i] = pattern[i] / NB_RADII; 
        if (pattern[i] < 150) {
            encoded_pattern = encoded_pattern | 1 << (RADIUS_RESOLUTION - i);
        }
        if (pattern[i] > 120 && pattern[i] < 170) {
            nb_fuzzy++;
        }
    }


#ifdef DEBUG
    cout << "Nb fuzzy:" << (int) nb_fuzzy << endl;
#endif
    // if many radii are 'fuzzy', we are probably seeing noise.
    if (nb_fuzzy > 0.2 * RADIUS_RESOLUTION) return;

    unsigned char current_best = 0;
    unsigned char best_score = RADIUS_RESOLUTION;
    for (const auto& kv : MARKERS) {
#ifdef DEBUG
        cout << "Marker " << (int) kv.first << ": " << endl;
        cout << "  " << bitset<25>(encoded_pattern) << endl;
        cout << "^ " << bitset<25>(kv.second) << endl;
        cout << "= " << bitset<25>(encoded_pattern ^ kv.second);
        cout << " (" << (int) count_bits(encoded_pattern ^ kv.second) << " '1's)" << endl;
#endif
        // the XOR returns '1's for each difference with the pattern.
        // We then count the nb of differences, and keep only when < 30%
        unsigned char score = count_bits(encoded_pattern ^ kv.second);

        if (score < 0.3 * RADIUS_RESOLUTION) {
            marker.valid = true;

            if (score < best_score) {
                best_score = score;
                current_best = kv.first;
            }
        }
    }
    marker.id = current_best;

    // no pattern recognized? no need to check the orientation!
    if (!marker.valid) return;

#ifdef DEBUG
    cout << endl << endl << "Orientation: " << endl;
#endif

    float theta_acc = 0.f;
    int nb_thetas = 0;
    int theta_max = 0;

    // get the max angle
    for (int i = 0; i < NB_RADII; i++) {
        int theta = orientation[i] / RADIUS_RESOLUTION; 
        if (theta > theta_max) theta_max = theta;
    }

    for (int i = 0; i < NB_RADII; i++) {
        int theta = orientation[i] / RADIUS_RESOLUTION; 

        if (theta > 0.9 * theta_max) {
            theta_acc += i * 360./NB_RADII; // deg
            nb_thetas++;
        }

#ifdef DEBUG
        cout << cvRound(i * 360./NB_RADII) << " deg: " << theta << ((theta > theta_max * 0.9) ? "  [x]":"")<< endl;
#endif
    }


    marker.theta = theta_acc / nb_thetas;


}


Marker Detector::decode_marker(InputArray img, Vec3f approx_circle) {

    Marker marker;

    // TODO: improve accuracy by using x,y from refined circle below
    marker.x = approx_circle[0];
    marker.y = approx_circle[1];

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

    dilate(tag, tag, MORPHOLOGICAL_ELEMENT);
    erode(tag, tag, MORPHOLOGICAL_ELEMENT);
#ifdef DEBUG
    imshow("Tag after dilation", tag);
    
    Mat toto;
    Canny(tag, toto, 20, 10);
    imshow("After canny", toto);
#endif


    vector<Vec3f> circles;
    HoughCircles(tag, circles,
                 HOUGH_GRADIENT,
                 2, // dp
                 20, // min dist
                 20, // param 1
                 20, // param 2
                 17, // min radius
                 20); // max radius

    if (circles.empty()) return marker; // 'marker.valid' is false by default


    auto circle = circles[0];

    roi = circle2rect(circle) & Rect(Point(0,0), tag.size());
    Mat ctag = tag(roi);

    unfold_circle(ctag, marker);

#ifdef DEBUG
    Mat cimg;
    cvtColor(tag, cimg, COLOR_GRAY2BGR);
    Point center(cvRound(circle[0]), cvRound(circle[1]));
    int radius = cvRound(circle[2]);
    // circle center
    cv::circle( cimg, center, 1, Scalar(0,255,0));
    // circle outline
    cv::circle( cimg, center, radius, Scalar(0,0,255), 1);

    resize(cimg, cimg, Size(), 5, 5);
    imshow("Tag", cimg);
#endif

#ifdef DEBUG
    if (marker.valid) {
        cout << "Found marker " << (int) marker.id << " at (" << marker.x << ", " << marker.y << ", theta=" << marker.theta << " deg)" << endl;
    }
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
        // back to original resolution
        c[0] *=2;
        c[1] *=2;
        c[2] *=2;

#ifdef DEBUG
        Point center(cvRound(c[0]), cvRound(c[1]));
        int radius = cvRound(c[2]);
        // circle center
        circle( cimg, center, 3, Scalar(0,255,0), -1, 8, 0 );
        // circle outline
        circle( cimg, center, radius, Scalar(0,0,255), 3, 8, 0 );
        imshow("Small detection surface", small);
        imshow("Detected circles", cimg);
        waitKey(0);
#endif

        Marker marker = decode_marker(img, c);

        if(marker.valid) markers.push_back(marker);

    }

    return markers;
}

