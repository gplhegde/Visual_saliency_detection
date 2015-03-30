/*
 * Author: Gopalakrishna Hegde and team (A2Zed)
 * Date: 8 Oct 2014
 *
 *
 */


#include "intensity_map.hpp"
#include "common_utils.hpp"
//#define INT_DEBUG

// API to find the intensity image from rgb image.
Mat findIntensityImage(cv::Mat rgbImage)
{
	vector<Mat> rgb;
	split(rgbImage, rgb);

	Mat intensityImage = (rgb[0] + rgb[1] + rgb[2])/3;
	return intensityImage;

#ifdef INT_DEBUG
    namedWindow( "Intensity Image", CV_WINDOW_AUTOSIZE );
    imshow( "Intensity Image", intensityImage );
#endif // INT_DEBUG
}

// Find the intensity/orientation 6 feature maps
// TODO: move this API to common_utils
vector<Mat> intFeatureMaps(vector<Mat> intPyr)
{
	int c, s, delta, up;
	vector<Mat> intMap(6);
	Mat tmp,dst;

	std::cout << "feature map calculation \n";
	for (c = 2; c <= 4; c++)
	{
		for(delta = 3;delta <= 4;delta++)
		{
			// Scale up
			tmp = intPyr[c + delta];
			for (up = 1; up <= delta; up++)
			{
                pyrUp( tmp, dst, Size( tmp.cols*2, tmp.rows*2 ));
				tmp = dst;
			}
			// Size of images should be same for subtraction
			// scuuessive scale down and scale up ulters the size if the size if not power of 2
			if (intPyr[c].size() != dst.size())
			{
                // Opencv rounds up the size to upper value if the size is odd while scaling down
                // Hence we discard the extra row/col while subtracting
                intMap[2*c + delta-7] = intPyr[c] - dst(Range(0,intPyr[c].rows), Range(0, intPyr[c].cols));
                std::cout << "image size mismatch; need to throw few rows and cols\n" ;
            }
            else
            {
                intMap[2*c+ delta-7] = intPyr[c] - dst;
            }
		}
	}
#ifdef INT_DEBUG
	namedWindow( "map_2_5", CV_WINDOW_AUTOSIZE );
    imshow( "map_2_5", intMap[0]);

    namedWindow( "map_3_6", CV_WINDOW_AUTOSIZE );
    imshow( "map_3_6", intMap[2]);

    namedWindow( "map_4_7", CV_WINDOW_AUTOSIZE );
    imshow( "map_4_7", intMap[4]);
#endif // INT_DEBUG

	return intMap;
}
// Find the final aggregate intensity map at level 4
Mat findAggIntMap(Mat intensityImage)
{
    vector<Mat> intPyramid;
    vector<Mat> intMaps;
    Mat aggMap;

    // Find the intensity pyramid
    intPyramid = getPyramid(intensityImage, 9);
    // calculate intensity feature maps
    intMaps = intFeatureMaps(intPyramid);
    // TODO: Normalization has to be done !!!
    // find the aggregate feature map
    aggMap = aggregateFeatureMap(intMaps);
#ifdef INT_DEBUG
    namedWindow( "Agg int map", CV_WINDOW_AUTOSIZE );
    imshow( "Agg int map", aggMap);
#endif // INT_DEBUG
    return aggMap;
}
