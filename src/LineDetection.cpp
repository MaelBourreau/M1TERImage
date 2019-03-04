#include "LineDetection.h"

LineDetection::LineDetection(Mat image, Mat imageBase)
{
    inputImage = image;
    baseImage = imageBase;
    demoImage = baseImage.clone();
}

LineDetection::~LineDetection()
{
}
void LineDetection::detectLine()
{
    baseImage.at<Vec3b>(839, 0) = {0, 255, 0};
    Mat image = inputImage.clone();
    Mat imagetmp = baseImage.clone();

    //cvtColor(baseImage, imagetmp, cv::COLOR_GRAY2BGR);
    cvtColor(inputImage, image, CV_RGB2GRAY);
    Mat lineMask = Mat::zeros(cv::Size(imagetmp.cols, imagetmp.rows), imagetmp.type());
    Mat greenLineMask = Mat::zeros(cv::Size(imagetmp.cols, imagetmp.rows), CV_32F);
    Mat redLineMask = Mat::zeros(cv::Size(imagetmp.cols, imagetmp.rows), imagetmp.type());
    bool isMinimum = true;
    uchar currVal = 0;

    for (int i = 0; i < image.cols; i++)
    {
        for (int j = 0; j < image.rows; j++)
        {

            if (isMinimum)
            {
                if (currVal < image.at<uchar>(j, i) - 2)
                {
                    imagetmp.at<Vec3b>(j, i) = {0, 0, 255};
                    lineMask.at<Vec3b>(j, i) = {0, 0, 255};
                    greenLineMask.at<uchar>(j, i) = 1;
                    isMinimum = false;
                }
            }
            else
            {
                if (currVal > image.at<uchar>(j, i) + 2)
                {
                    imagetmp.at<Vec3b>(j, i) = {0, 255, 0};
                    lineMask.at<Vec3b>(j, i) = {0, 255, 0};
                    redLineMask.at<Vec3b>(j, i) = {0, 255, 0};
                    isMinimum = true;
                }
            }
            currVal = image.at<uchar>(j, i);
        }
    }

    inputImage = imagetmp;

    namedWindow("LES LIGNES", WINDOW_AUTOSIZE);
    imshow("LES LIGNES", lineMask);
    imwrite("../../imres/res.jpg", imagetmp);

    this->lineMask = lineMask;
    this->greenLineMask = greenLineMask;
    this->redLineMask = redLineMask;
    this->baseImage = imagetmp;

    greenProjection();
}

void LineDetection::greenProjection()
{

    Mat element = getStructuringElement(MORPH_RECT,
                                        Size(1, 3));
    Mat masktmp = lineMask.clone();

    dilate(masktmp, lineMask, element);

    vector<int> projection = vector<int>(lineMask.rows, 0);
    vector<int> result;

    for (int i = 0; i < lineMask.rows; i++)
    {
        for (int j = 0; j < lineMask.cols; j++)
        {
            if (lineMask.at<Vec3b>(i, j)[2] == 0 && lineMask.at<Vec3b>(i, j)[1] == 255 && lineMask.at<Vec3b>(i, j)[0] == 0)
            {
                projection[i]++;
                baseImage.at<Vec3b>(i, projection[i]) = {0, 255, 0};
            }
        }
        if (projection[i] < 15)
            projection[i] = 0;

    }

    int curmax = 0;
    int curmaxi = 0;
    int nombredezeros = 0;
    int nombredeverts = 0;
    for (int i = 0; i < projection.size(); i++)
    {
        if (projection[i] != 0)
        {
            nombredeverts++;
        }
        if (projection[i] > curmax)
        {
            curmax = projection[i];
            curmaxi = i;
        }
        if (projection[i] == 0 && curmaxi > 0)
        {
            nombredezeros++;
            if (nombredezeros == 5 && nombredeverts > 10)
            {
                line(baseImage, Point(projection[curmaxi], curmaxi), Point(baseImage.rows - 1, curmaxi), Scalar(255, 0, 0));
                result.push_back(curmaxi);

                nombredezeros = 0;
                curmax = 0;
                curmaxi = 0;
                nombredeverts = 0;
            }
            else if (nombredezeros == 5 && nombredeverts < 10)
            {
                nombredeverts = 0;
                nombredezeros = 0;
                curmax = 0;
                curmaxi = 0;
            }
        }
        else if (projection[i] != 0 && nombredezeros != 0)
        {
            nombredezeros = 0;
        }
    }
    this->baseImage = baseImage;

    maximums = result;
    redLineRegression();
}

void LineDetection::redLineRegression()
{
    int prev_max = maximums[0];
    RNG rng(12345);
    Mat rainbowMask = Mat::zeros(cv::Size(baseImage.cols, baseImage.rows), baseImage.type());
    for (int i = 1; i < maximums.size(); ++i)
    {
        vector<Point> points;
        for (int rows = prev_max; rows < maximums[i]; rows++)
        {
            for (int cols = 0; cols < lineMask.cols; cols++)
            {

                if (lineMask.at<Vec3b>(rows, cols)[2] == 255 && lineMask.at<Vec3b>(rows, cols)[1] == 0 && lineMask.at<Vec3b>(rows, cols)[0] == 0)
                {
                    points.push_back(Point(cols, rows));
                }
            }
        }
        Vec4f ligne;

        fitLine(points, ligne, CV_DIST_L1, 0, 0.01, 0.01);

        //line sous forme de vecteur, donc on le convertis
        float vx = ligne[0];
        float vy = ligne[1];
        int x = (int)ligne[2];
        int y = (int)ligne[3];

        Point p1 = Point(x - (baseImage.rows + baseImage.cols) * vx, y - (baseImage.rows + baseImage.cols) * vy);
        Point p2 = Point(x + (baseImage.rows + baseImage.cols) * vx, y + (baseImage.rows + baseImage.cols) * vy);

        // line(rainbowMask,p1,p2,Scalar(0,255,255));
        //line(baseImage,p1,p2,Scalar(0,255,255));

        line(rainbowMask, p1, p2, Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)));

        line(baseImage, p1, p2, Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)));

        prev_max = maximums[i];

        namedWindow("Approximation Lignes", WINDOW_AUTOSIZE);
        imshow("Approximation Lignes", baseImage);
        imwrite("../../imres/final.jpg", baseImage);
    }
    this->rainbowMask = rainbowMask;

    TextColoring();
}

Mat LineDetection::getBaseImage()
{
    return this->baseImage.clone();
}

void LineDetection::TextColoring()
{
    int seuil = 100;

    for (int rows = 0; rows < rainbowMask.rows; rows++)
    {
        for (int cols = 0; cols < rainbowMask.cols; cols++)
        {
            if (rainbowMask.at<Vec3b>(rows, cols)[0] != 0 || rainbowMask.at<Vec3b>(rows, cols)[1] != 0 || rainbowMask.at<Vec3b>(rows, cols)[2] != 0)
            {
                if (demoImage.at<Vec3b>(rows, cols)[0] < seuil && demoImage.at<Vec3b>(rows, cols)[1] < seuil && demoImage.at<Vec3b>(rows, cols)[2] < seuil)
                {

                    if (demoImage.at<Vec3b>(rows, cols) != rainbowMask.at<Vec3b>(rows, cols))
                    {
                        floodFill(demoImage, Point(cols, rows), Scalar(rainbowMask.at<Vec3b>(rows, cols)[0], rainbowMask.at<Vec3b>(rows, cols)[1], rainbowMask.at<Vec3b>(rows, cols)[2]), 0, Scalar(70, 70, 70), Scalar(70, 70, 70), FLOODFILL_FIXED_RANGE);
                    }
                }
            }
        }
    }
    namedWindow("CEVRAIMENTBO", WINDOW_AUTOSIZE);
    imshow("CEVRAIMENTBO", demoImage);
    imwrite("../../imres/cebo.jpg", demoImage);
}
void LineDetection::save(int event, int x, int y, int flags, void *userdata)
{
}
