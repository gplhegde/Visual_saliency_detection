/*
 * Author: Gopalakrishna Hegde and team (A2Zed)
 * Date: 8 Oct 2014
 *
 *
 */


#include "colorMap.hpp"
#include "common_utils.hpp"
//#define COLOR_DEBUG

// Normalize the color image based on intensity image
Mat normalizeRgbImage(Mat rgbImg, Mat intImg)
{

    double maxIntensity;
    int row, col;
    Mat tmpImg = rgbImg;
    // Find the max intensity in the intensity image.
    cv::minMaxLoc(intImg, NULL, &maxIntensity, NULL, NULL);

    Vec3b zero(0,0,0);

    if (tmpImg.size() != intImg.size())
    {
        std::cout << "Image size mismatch";
        exit(0);
    }
    // Normalization of all channels
    // Pixels are normalized only if the corresponding pixel in the intensity image is more than 10% of max intensity
    for (row = 0; row < rgbImg.rows; row++)
    {
        for (col= 0; col < rgbImg.cols; col++)
        {

            if (intImg.at<uchar>(row, col) >= (0.1* maxIntensity))
            {
                tmpImg.at<Vec3b>(row, col)[0] = saturate_cast<uchar>( (UINT8_MAX *tmpImg.at<Vec3b>(row, col)[0])/maxIntensity );
                tmpImg.at<Vec3b>(row, col)[1] = saturate_cast<uchar>( (UINT8_MAX *tmpImg.at<Vec3b>(row, col)[1])/maxIntensity );
                tmpImg.at<Vec3b>(row, col)[2] = saturate_cast<uchar>( (UINT8_MAX *tmpImg.at<Vec3b>(row, col)[2])/maxIntensity );
            }
            else
            {
                tmpImg.at<Vec3b>(row,col) = zero;
            }
        }
    }
#ifdef COLOR_DEBUG
    namedWindow( "norm image", CV_WINDOW_AUTOSIZE );
    imshow( "norm image", tmpImg);
#endif // COLOR_DEBUG
    return tmpImg;

}

// API to create R, G, B , Y color channels
vector<Mat> createColorChanels(Mat img) // Working
{
    vector<Mat> colorCh(4);
    vector<Mat> chanels;

    split(img, chanels);
    colorCh[0] = chanels[2] - ((chanels[1] + chanels[0])/2); // R channel
    colorCh[1] = chanels[1] - ((chanels[0] + chanels[2])/2); // G channel
    colorCh[2] = chanels[0] - ((chanels[0] + chanels[1])/2); // B channel
    // TODO: set -ve values in Y channel to 0
    colorCh[3] = (((chanels[2] + chanels[1])/2) - abs(chanels[2] - chanels[1])/2 - chanels[0]); // Y channel
    threshold(colorCh[3], colorCh[3],0,UINT8_MAX,THRESH_TOZERO);
#ifdef COLOR_DEBUG
    namedWindow( "R ch", CV_WINDOW_AUTOSIZE );
    imshow( "R ch", colorCh[0]);

    namedWindow( "G ch", CV_WINDOW_AUTOSIZE );
    imshow( "G ch", colorCh[1]);

    namedWindow( "B ch", CV_WINDOW_AUTOSIZE );
    imshow( "B ch", colorCh[2]);

    namedWindow( "Y ch", CV_WINDOW_AUTOSIZE );
    imshow( "Y ch", colorCh[3]);
#endif // COLOR_DEBUG

    return colorCh;
}

// API to find 12 color maps
vector<Mat> colorFeatureMap(vector<Mat> rgbyCh)
{
    int c, delta, up;
    vector<Mat> colorMap(12); // first 6 -> RG maps next 6 BY maps
    vector<Mat> rPyr(9);
    vector<Mat> gPyr(9);
    vector<Mat> bPyr(9);
    vector<Mat> yPyr(9);

    // Find pyramids of color channels
    rPyr = getPyramid(rgbyCh[0], 9);
    gPyr = getPyramid(rgbyCh[1], 9);
    bPyr = getPyramid(rgbyCh[2], 9);
    yPyr = getPyramid(rgbyCh[3], 9);

    Mat tmpRG, tmpBY, dstRG, dstBY;

    for (c = 2; c <= 4; c++)
	{
		for(delta = 3;delta <= 4;delta++)
		{
			// Scale up
			//tmp = intPyr[c + delta];
			tmpRG = gPyr[c + delta] - rPyr[c + delta];
			tmpBY = yPyr[c + delta] - bPyr[c + delta];
			for (up = 1; up <= delta; up++)
			{
                pyrUp( tmpRG, dstRG, Size( tmpRG.cols*2, tmpRG.rows*2 ));
				tmpRG = dstRG;
				pyrUp( tmpBY, dstBY, Size( tmpBY.cols*2, tmpBY.rows*2 ));
				tmpBY = dstBY;
			}
			// Size of images should be same for subtraction
			// scuuessive scale down and scale up ulters the size if the size if not power of 2
			if (rPyr[c].size() != dstRG.size())
			{
                // Opencv rounds up the size to upper value if the size is odd while scaling down
                // Hence we discard the extra row/col while subtracting
				// C(2,5) is stored in colorMap[0] and so on.... hence the magic expression "2*c+delta-7" !!!
                colorMap[2*c + delta-7] = abs((rPyr[c] - gPyr[c]) - dstRG(Range(0,rPyr[c].rows), Range(0, rPyr[c].cols)));
                colorMap[2*c + delta-7+6] = abs((bPyr[c] - yPyr[c]) - dstBY(Range(0,rPyr[c].rows), Range(0, rPyr[c].cols)));
                std::cout << "image size mismatch; need to throw few rows and cols" ;
            }
            else
            {
                colorMap[2*c+ delta-7] = abs((rPyr[c] - gPyr[c]) - dstRG);
                colorMap[2*c+ delta-7+6] = abs((bPyr[c] - yPyr[c]) - dstBY);
            }
		}
	}
#ifdef COLOR_DEBUG
	namedWindow( "cmap0", CV_WINDOW_AUTOSIZE );
    imshow( "cmap0", colorMap[0]);

    namedWindow( "cmap6", CV_WINDOW_AUTOSIZE );
    imshow( "cmap6", colorMap[6]);
#endif // COLOR_DEBUG

    return colorMap;

}

// Find final color map
// TODO: Normalization of all color maps before 
Mat findAggColorMap(Mat testImg, Mat intensityImage)
{
    Mat normImg = normalizeRgbImage(testImg, intensityImage);
    vector<Mat> colorCh = createColorChanels(normImg);
    vector<Mat> colorMap = colorFeatureMap(colorCh); // 12 maps, 0->5 RG maps, 6->11 BY maps
    // Add RG and BY maps at respective scales
    vector<Mat> RGBYMap(6);

    for (int i = 0; i < 6;i++)
    {
        cv:add(colorMap[i], colorMap[i+6], RGBYMap[i]);
    }

    Mat aggMap = aggregateFeatureMap(RGBYMap);
#ifdef COLOR_DEBUG
    namedWindow( "Agg color map", CV_WINDOW_AUTOSIZE );
    imshow( "Agg color map", aggMap);
#endif // COLOR_DEBUG
    return aggMap;
}
