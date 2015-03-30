#ifndef ORIENTATION_MAP_HPP_INCLUDED
#define ORIENTATION_MAP_HPP_INCLUDED

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
vector<Mat> getGaborImages(Mat img, int kernelSize);

Mat getAggOrientationMap(Mat intensityImage);

#endif // ORIENTATION_MAP_HPP_INCLUDED
