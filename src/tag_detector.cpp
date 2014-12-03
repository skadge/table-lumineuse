#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#include <opencv2/highgui/highgui.hpp> //for FileStorage

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


int main ( int argc,char **argv ) {

    Mat image, imageUndistorted;

    image = imread(argv[1]);

    readCalibration(argv[2]);

    undistort(image, imageUndistorted, cameraMatrix, distCoeffs);

    imshow("original", image);
    waitKey(0);
    imshow("undistorted", imageUndistorted);
    waitKey(0);
    return 0;
}

