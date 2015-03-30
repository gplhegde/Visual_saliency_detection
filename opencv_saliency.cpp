#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>

using namespace cv;

vector<Mat> pyramidFilter(Mat input);
Mat centerSurround(Mat surround, Mat center);
Mat getIntensityMap(Mat input);
Mat getRGMap(Mat red, Mat green);
Mat getBYMap(Mat blue, Mat yellow);

int main(int argc, char** argv)
{

		Mat red;
		Mat green;
		Mat blue;
		Mat yellow;
		Mat intensity;
		Mat saliencyMap;
		Mat intensityMap;
		Mat intensityMapNorm;
		Mat rgMap;
		Mat rgMapNorm;
		Mat byMap ;
        	Mat byMapNorm;

		Mat gabor0 ;
		Mat gabor45 ;
		Mat gabor90 ;
		Mat gabor135 ;
		Mat orMap;

        Mat input = imread("../images/donald_duck.bmp");
    if( !input.data )
    {
		printf(" No data! -- Exiting the program \n");
        return -1;
	}
	namedWindow("org", CV_WINDOW_AUTOSIZE);
	imshow("org", input);
		vector<Mat> frame;
		// Extract channels...

		split(input, frame);
		red = frame[2];
		green = frame[1];
		blue = frame[0];

		//yellow = (red + green)/2;
		yellow = (red + green)/2 - (red-green)/2 -blue;
		cvtColor(input, intensity, CV_BGR2GRAY);
 //       intensity = (frame[0] + frame[1]+ frame[2]) /3;
    


		// Compute on each channel
		intensityMap = getIntensityMap(intensity);

		rgMap = getRGMap(red, green);
		byMap = getBYMap(blue,yellow);

		addWeighted(rgMap, 0.5, byMap, 0.5, 0.0, rgMap);
		// Orientation MAP calculation
		Mat kernel = getGaborKernel(Size(5, 5), 3, 0, CV_PI, 1, 0, CV_32F);
		filter2D(intensity, gabor0, -1, kernel);
		 kernel = getGaborKernel(Size(5, 5), 3, CV_PI/4, CV_PI, 1, 0, CV_32F);
		filter2D(intensity, gabor45, -1, kernel);
		 kernel = getGaborKernel(Size(5, 5), 3, CV_PI/2, CV_PI, 1, 0, CV_32F);
		filter2D(intensity, gabor90, -1, kernel);
		 kernel = getGaborKernel(Size(5, 5), 3, (3*CV_PI)/4, CV_PI, 1, 0, CV_32F);
		filter2D(intensity, gabor135, -1, kernel);

		gabor0 = getIntensityMap(gabor0);
		gabor45 = getIntensityMap(gabor45);
		gabor90 = getIntensityMap(gabor90);
		gabor135= getIntensityMap(gabor135);

		Mat orMap1;
		Mat orMap2;
		addWeighted(gabor0,0.5,gabor45,0.5,0.0,orMap1);
		addWeighted(gabor90,0.5,gabor135,0.5,0.0,orMap2);
		addWeighted(orMap1,0.5,orMap2,0.5,0.0,orMap);
		// Final assembly

		normalize(intensityMap, intensityMapNorm, 0., 256., NORM_MINMAX);
		normalize(rgMap, rgMapNorm, 0., 256., NORM_MINMAX);
		addWeighted(intensityMap, 0.5, rgMap, 0.5, 0.0, saliencyMap);
		addWeighted(orMap, 0.5, saliencyMap, 0.5, 0.0, saliencyMap);

		// Final display image must be rescale to fit Android screen resolution
		// What a dumbass hack. Nachiket:1, Android:0
		Mat result ;
		Mat resultNorm ;
		Size size;
		resize(saliencyMap, result, size, 2.0, 2.0, 1);
		normalize(result, resultNorm, 0., 256., NORM_MINMAX);
		namedWindow("final map", CV_WINDOW_AUTOSIZE);
	imshow("final map", resultNorm);

        waitKey(0);
		imwrite("saliency_map.jpg", resultNorm);
	}

vector<Mat> pyramidFilter(Mat input) {
		int depth=4;
		vector<Mat> pyramid;
    namedWindow("final map", CV_WINDOW_AUTOSIZE);
	imshow("final map", input);
	buildPyramid(input, pyramid, depth+1, BORDER_DEFAULT);

		return pyramid;


}

Mat centerSurround(Mat surround, Mat center) {
		Mat csOutput ;
		Mat centerRescaled ;
		double factor = surround.cols/(double)center.cols;
		resize(center, centerRescaled, Size(surround.cols,surround.rows), 0, 0, 1);
		subtract(centerRescaled, surround, csOutput);
		return csOutput;
	}

Mat getIntensityMap(Mat input) {
		vector<Mat> pyr = pyramidFilter(input);
		Mat i02 = centerSurround(pyr[0], pyr[2]);
		Mat i03 = centerSurround(pyr[0], pyr[3]);

		Mat x;
		Mat x_1 ;
		addWeighted(i02, 0.5, i03, 0.5, 0.0, x);
		resize(x, x_1, Size(), 0.5, 0.5, 0);
		Mat i13 = centerSurround(pyr[1], pyr[3]);
		Mat i14 = centerSurround(pyr[1], pyr[4]);

		Mat y ;
		addWeighted(i13, 0.5, i14, 0.5, 0.0, y);
		Mat result ;

		addWeighted(x_1, 0.5, y, 0.5, 0.0, result);
		return result;
	}

Mat getRGMap(Mat red, Mat green) {
		vector<Mat> pyrRed = pyramidFilter(red);
		vector<Mat> pyrGreen = pyramidFilter(green);
		Mat rgDiff0;
		Mat rgDiff1 ;
		Mat grDiff2 ;
		Mat grDiff3 ;
		Mat grDiff4 ;

		subtract(pyrRed[0], pyrGreen[0], rgDiff0);
		subtract(pyrRed[1], pyrGreen[1], rgDiff1);
		subtract(pyrGreen[2], pyrRed[2], grDiff2);
		subtract(pyrGreen[3], pyrRed[3], grDiff3);
		subtract(pyrGreen[4], pyrRed[4], grDiff4);
		Mat rg02 = centerSurround(rgDiff0, grDiff2);
		Mat rg03 = centerSurround(rgDiff0, grDiff3);
		Mat x ;
		Mat x_1 ;
		addWeighted(rg02, 0.5, rg03, 0.5, 0.0, x);
		resize(x, x_1, Size(), 0.5, 0.5, 1);
		Mat rg13 = centerSurround(rgDiff1, grDiff3);
		Mat rg14 = centerSurround(rgDiff1, grDiff4);
		Mat y ;
		addWeighted(rg13, 0.5, rg14, 0.5, 0.0, y);
		Mat result ;
		addWeighted(x_1, 0.5, y, 0.5, 0.0, result);
		return result;
	}
Mat getBYMap(Mat blue, Mat yellow)
{
	vector<Mat> pyrBlue = pyramidFilter(blue);
                vector<Mat> pyrYellow = pyramidFilter(yellow);
                Mat byDiff0;
                Mat byDiff1;
                Mat ybDiff2 ;
                Mat ybDiff3 ;
                Mat ybDiff4 ;

                subtract(pyrBlue[0], pyrYellow[0], byDiff0);
                subtract(pyrBlue[1], pyrYellow[1], byDiff1);
                subtract(pyrYellow[2], pyrBlue[2], ybDiff2);
                subtract(pyrYellow[3], pyrBlue[3], ybDiff3);
                subtract(pyrYellow[4], pyrBlue[4], ybDiff4);
                Mat by02 = centerSurround(byDiff0, ybDiff2);
                Mat by03 = centerSurround(byDiff0, ybDiff3);
                Mat x ;
                Mat x_1 ;
                addWeighted(by02, 0.5, by03, 0.5, 0.0, x);
                resize(x, x_1, Size(), 0.5, 0.5, 1);
                Mat by13 = centerSurround(byDiff1, ybDiff3);
                Mat by14 = centerSurround(byDiff1, ybDiff4);
                Mat y ;
                addWeighted(by13, 0.5, by14, 0.5, 0.0, y);
                Mat result ;
                addWeighted(x_1, 0.5, y, 0.5, 0.0, result);
                return result;
}
