#ifndef INTENSITYMAP_HPP_INCLUDED
#define INTENSITYMAP_HPP_INCLUDED

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdlib.h>

#include <stdio.h>
using namespace cv;

Mat findIntensityImage(cv::Mat rgbImage);
vector<Mat> intFeatureMaps(vector<Mat> intPyr);
Mat findAggIntMap(Mat intensityImage);

#endif // INTENSITYMAP_HPP_INCLUDED
