#ifndef COMMON_UTILS_HPP_INCLUDED
#define COMMON_UTILS_HPP_INCLUDED

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
long timestamp();
vector<Mat> getPyramid(cv::Mat image, int level);

Mat aggregateFeatureMap(vector<Mat> featureMap);

#endif // UTILS_HPP_INCLUDED
