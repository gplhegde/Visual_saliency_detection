/*
 * Author: Gopalakrishna Hegde and team (A2Zed)
 * Date: 8 Oct 2014
 *
 *
 */


#include "orientation_map.hpp"
#include "intensity_map.hpp"
#include "common_utils.hpp"
//#define ORIENTATION_DEBUG

// Returns 0,45,90,135 deg rotated and filtered images.
vector<Mat> getGaborImages(Mat img, int kernelSize)
{
    double sigma = 3;
    double lambda = CV_PI;
    double gamma = 1;
    double psi = 0;
    vector<Mat> gaborImg(4);
    Mat dst;

    Mat kernel = getGaborKernel(Size(kernelSize, kernelSize), sigma, 0, lambda, gamma, psi, CV_32F);
    filter2D(img, dst, -1, kernel);
    gaborImg[0] = dst;

    kernel = getGaborKernel(Size(kernelSize, kernelSize), sigma, CV_PI/4, lambda, gamma, psi, CV_32F);
    filter2D(img, dst, -1, kernel);
    gaborImg[1] = dst;

    kernel = getGaborKernel(Size(kernelSize, kernelSize), sigma, CV_PI/2, lambda, gamma, psi, CV_32F);
    filter2D(img, dst, -1, kernel);
    gaborImg[2] = dst;

    kernel = getGaborKernel(Size(kernelSize, kernelSize), sigma, (3*CV_PI)/4, lambda, gamma, psi, CV_32F);
    filter2D(img, dst, -1, kernel);
    gaborImg[3] = dst;
#ifdef ORIENTATION_DEBUG
    namedWindow( "img45", CV_WINDOW_AUTOSIZE );
    imshow( "img45", gaborImg[1]);
#endif // ORIENTATION_DEBUG


    return gaborImg;


}

// find the final orientation map.
// TODO: Normalization of all maps before finding final map.
Mat getAggOrientationMap(Mat intensityImage)
{
    vector<Mat> gaborImg = getGaborImages(intensityImage, 5);

    vector<Mat> gaborPyr0(9);
    vector<Mat> gaborPyr45(9);
    vector<Mat> gaborPyr90(9);
    vector<Mat> gaborPyr135(9);

    vector<Mat> orMap0(6);
    vector<Mat> orMap45(6);
    vector<Mat> orMap90(6);
    vector<Mat> orMap135(6);

    vector<Mat> orMap(24);

    // First find the gabor pyramid
    gaborPyr0 = getPyramid(gaborImg[0], 9);
    gaborPyr45 = getPyramid(gaborImg[1], 9);
    gaborPyr90 = getPyramid(gaborImg[2], 9);
    gaborPyr135 = getPyramid(gaborImg[3], 9);

    orMap0 = intFeatureMaps(gaborPyr0);
    orMap45 = intFeatureMaps(gaborPyr45);
    orMap90 = intFeatureMaps(gaborPyr90);
    orMap135 = intFeatureMaps(gaborPyr135);

    Mat aggMap0 = aggregateFeatureMap(orMap0);
    Mat aggMap45 = aggregateFeatureMap(orMap45);
    Mat aggMap90 = aggregateFeatureMap(orMap90);
    Mat aggMap135 = aggregateFeatureMap(orMap135);

    Mat tmpAgg1, tmpAgg2, orAggMap;
    add(aggMap0, aggMap45, tmpAgg1);
    add(aggMap90, aggMap135, tmpAgg2);

    add(tmpAgg1, tmpAgg2, orAggMap);
#ifdef ORIENTATION_DEBUG
    namedWindow("aggOrMap", CV_WINDOW_AUTOSIZE);
    imshow("aggOrMap", orAggMap);
#endif //ORIENTATION_DEBUG
    return orAggMap;





}
