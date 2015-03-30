/*
 * Author: Gopalakrishna Hegde and team (A2Zed)
 * Date: 8 Oct 2014
 *
 *
 */

#include "common_utils.hpp"
#include <sys/time.h>

// Utility to measure run time
// runtime = end timestamp - start timestamp (in microsec)
long timestamp()
{
	struct timeval current_time;
	gettimeofday(&current_time, NULL);
	return (current_time.tv_sec * 1000000 + current_time.tv_usec);
}

// API that returns gaussian pyramid of given level
vector<Mat> getPyramid(cv::Mat image, int level)
{
	vector<Mat> intPyramid;
	buildPyramid(image, intPyramid, level, BORDER_DEFAULT);
	return intPyramid;
}


// Find combined feature map at level 4.
Mat aggregateFeatureMap(vector<Mat> featureMap)
{
    Mat aggMap, tmp,dst;
    int c, delta, down;

    // add maps at level 4.. featureMap[4] and [5] stores these maps
    cv::add(featureMap[4] ,featureMap[5], aggMap); // saturate add

    for (c = 2;c <= 3; c++)
    {
        for (delta = 3; delta <= 4; delta++)
        {
            tmp = featureMap[2*c + delta - 7]; // I(2,5) is stored on Map[0]
            for (down = 1; down <= 4 - c; down++)
            {
                pyrDown(tmp, dst);
                tmp = dst;
            }
            if ( aggMap.size() == dst.size())
            {
                 cv::add(aggMap, dst, aggMap);
            }
            else
            {
                cv::add(aggMap, dst(Range(0, aggMap.rows), Range(0, aggMap.cols)), aggMap);
            }
        }
    }
    return aggMap;
}
