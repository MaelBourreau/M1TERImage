#include "LineDetection.h"


LineDetection::LineDetection(Mat image, Mat imageBase)
{
    inputImage = image;
    baseImage = imageBase;

}

LineDetection::~LineDetection()
{
}
void LineDetection::detectLine()
{
    baseImage.at<Vec3b>(839,0) = {0, 255, 0};
    Mat image = inputImage.clone();
    Mat imagetmp = baseImage.clone();
    //cvtColor(baseImage, imagetmp, cv::COLOR_GRAY2BGR);
    cvtColor(inputImage, image, CV_RGB2GRAY);
    
    bool isMinimum = true;
    uchar currVal = 0;


    for (int i = 0; i < image.cols; i++)
    {
        for (int j = 0; j < image.rows; j++)
        {  

            if (isMinimum)
            {
                if (currVal < image.at<uchar>(j,i)-2) {
                    imagetmp.at<Vec3b>(j,i) = {0,0,255};
                    isMinimum = false;
                }
            }
            else
            {
                if (currVal > image.at<uchar>(j,i)+2 )
                {
                    imagetmp.at<Vec3b>(j,i) = {0,255,0};
                    isMinimum = true;

                }
            }
            currVal = image.at<uchar>(j,i);
        }
    }

    namedWindow("LES LIGNES", WINDOW_AUTOSIZE);
    imshow("LES LIGNES", imagetmp);
    imwrite("../../imres/res.jpg", imagetmp);
    

}

void LineDetection::save(int event, int x, int y, int flags, void* userdata)
{

}
