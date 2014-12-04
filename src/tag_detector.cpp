#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>


#include <opencv2/highgui/highgui.hpp> //for FileStorage

#include <iostream>

using namespace std;
using namespace cv;

Mat distCoeffs, cameraMatrix;

Size readCalibration(const string &filename) {
    Size size;
    FileStorage fs(filename, cv::FileStorage::READ);
    fs["image_width"]             >> size.width;
    fs["image_height"]            >> size.height;
    fs["distortion_coefficients"] >> distCoeffs;
    fs["camera_matrix"]           >> cameraMatrix;

    return size;
}

void prepare(InputArray rawimage, OutputArray img) {


    auto newcamera = getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, rawimage.size(), 0); 
    undistort(rawimage, img, cameraMatrix, distCoeffs, newcamera);

    resize(img, img, Size(320,240));


}

void detect_circles(InputArray img, OutputArray circles) {

    HoughCircles(img, circles,
                 HOUGH_GRADIENT,
                 1, // dp
                 20, // min dist
                 30, // param 1
                 8, // param 2
                 7, // min radius
                 11); // max radius

    cout << "Found " << circles.size() << " circles" << endl;

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


Rect circle2rect(Vec3f circle, double scaling = 1.0) {
    int radius = cvRound(circle[2] * scaling);

    Rect roi = Rect(Point(circle[0] - radius ,
                          circle[1] - radius ),
                    Point(circle[0] + radius ,
                          circle[1] + radius ));

    return roi;
}

void decode_marker(InputArray img, Vec3f circle) {

    auto roi = circle2rect(circle, 2);
    // make sure our ROI remains inside the image
    roi = roi & Rect(Point(0,0), img.size());

    Mat tag = img.getMat()(roi);

    //equalizeHist(tag, tag);
    //show_hist(tag);
    adaptiveThreshold(tag, tag, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY,7,0);
 
    vector<Vec3f> circles;
    HoughCircles(tag, circles,
                 HOUGH_GRADIENT,
                 1, // dp
                 20, // min dist
                 30, // param 1
                 8, // param 2
                 8, // min radius
                 10); // max radius

    if (!circles.empty()) {

        auto circle = circles[0];

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

        roi = circle2rect(circle) & Rect(Point(0,0), tag.size());
        Mat ctag = cimg(roi);


    
        //Moments m = moments(tag, true);
        //Point p(m.m10/m.m00, m.m01/m.m00);
        //cv::circle(tag, p, 1, Scalar(128,0,0));
        //Mat mask = Mat::zeros(tag.size(), CV_8UC1);
        //cv::circle(mask, p, 9, Scalar(255,255,255), -1);

        //Mat destImg;
        //tag.copyTo(destImg, mask);
        //
        resize(ctag, ctag, Size(), 5, 5);


        imshow("Tag", ctag);
        waitKey(0);
    }
}

int main ( int argc,char **argv ) {

    Mat rawimage, img;
    Mat cimg;

    readCalibration(argv[1]);

    rawimage = imread(argv[2], 0);

    prepare(rawimage, img);

    cvtColor(img, cimg, COLOR_GRAY2BGR);

    vector<Vec3f> circles;
    detect_circles(img, circles);

    for (auto c : circles) {
        Point center(cvRound(c[0]), cvRound(c[1]));
        int radius = cvRound(c[2]);
        // circle center
        circle( cimg, center, 3, Scalar(0,255,0), -1, 8, 0 );
        // circle outline
        circle( cimg, center, radius, Scalar(0,0,255), 3, 8, 0 );

        decode_marker(img, c);
    }

    //imshow("detected circles",cimg);
    //waitKey(0);



    //imshow("undistorted", imageUndistorted);
    //waitKey(0);
    return 0;
}

