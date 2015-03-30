/*
 * Author: Gopalakrishna Hegde and team (A2Zed)
 * Date: 8 Oct 2014
 *
 *
 */


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include "colorMap.hpp"
#include "common_utils.hpp"
#include "intensity_map.hpp"
#include "orientation_map.hpp"

using namespace cv;
//#define DEBUG


/**
 * @function main
 */
int main( int argc, char** argv )
{
	cv::Mat intensityImage;
	vector<Mat> intPyramid;
	Mat intMap, colorMap, orMap;
	long total_time , int_time, color_time, ori_time;

	total_time = timestamp();
    Mat testImg  = imread( "coptor.jpg" );
    //Mat testImg  = imread( "GlobalRarity.png" );
    if( !testImg.data )
    {
		printf(" No data! -- Exiting the program \n");
        return -1;
	}

	//namedWindow("Original Image", CV_WINDOW_AUTOSIZE);
	//imshow("Original Imgae", testImg);
	int_time = timestamp();
	intensityImage = findIntensityImage(testImg);

	intMap = findAggIntMap(intensityImage);
	
	std::cout << "int time =" << (timestamp() - int_time) << "\n";
	color_time = timestamp();
	colorMap = findAggColorMap(testImg, intensityImage);
	std::cout << "color time =" << (timestamp() - color_time) << "\n";
	
	ori_time = timestamp();
	orMap = getAggOrientationMap(intensityImage);

	std::cout << "ori time =" << (timestamp() - ori_time) << "\n";
	Mat finalMap = (intMap + colorMap + orMap )/3;
	std::cout << "total time =" << (timestamp() - total_time) << "\n";
	namedWindow("final map", CV_WINDOW_AUTOSIZE);
	imshow("final map", finalMap);

	Mat tmpFinalMap;
	for(int up = 1; up<=4; up++)
	{
        pyrUp(finalMap, tmpFinalMap);
        finalMap = tmpFinalMap;
	}
    //namedWindow("final map", CV_WINDOW_AUTOSIZE);
	//imshow("final map", finalMap);

	//threshold(tmpFinalMap, finalMap,120,UINT8_MAX,THRESH_BINARY);
	Mat thImg;
	inRange(finalMap, 100, 255, thImg);
    namedWindow("threshold map", CV_WINDOW_AUTOSIZE);
	imshow("threshold map", finalMap);

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours( thImg, contours, hierarchy,
        CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

    // iterate through all the top-level contours,
    // draw each connected component with its own random color
    int idx = 0;
    for( ; idx >= 0; idx = hierarchy[idx][0] )
    {
        Scalar color( rand()&255, rand()&255, rand()&255 );
        drawContours( testImg, contours, idx, color, 3, 8, hierarchy );
    }
    namedWindow( "Salient objects", CV_WINDOW_AUTOSIZE);
    imshow( "Salient objects", testImg );
	waitKey(0);
  return 0;
}




