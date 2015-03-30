#ifndef COLORMAP_HPP_INCLUDED
#define COLORMAP_HPP_INCLUDED

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
using namespace cv;
Mat normalizeRgbImage(Mat rgbImg, Mat intImg);
vector<Mat> createColorChanels(Mat img);
vector<Mat> colorFeatureMap(vector<Mat> rgbyCh);
Mat findAggColorMap(Mat testImg, Mat intensityImage);

#endif // COLORMAP_HPP_INCLUDED
