#include "LineDetection.h"
#include <cstdio>
#include <iostream>
#include <fstream>

LineDetection::LineDetection(Mat image, Mat imageBase, string imageName)
{
    inputImage = image;
    baseImage = imageBase;
    demoImage = baseImage.clone();
    this->imageName = imageName;
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

    imwrite("../../imres/res.jpg", imagetmp);

    this->lineMask = lineMask;
    this->greenLineMask = greenLineMask;
    this->redLineMask = redLineMask;
    this->baseImage = imagetmp;
}

void LineDetection::greenProjection()
{

    Mat element = getStructuringElement(MORPH_RECT,
                                        Size(1, 15));
    Mat masktmp = lineMask.clone();
    Mat blueMask = Mat::zeros(cv::Size(lineMask.cols, lineMask.rows), lineMask.type());

    dilate(lineMask, masktmp, element);

    vector<int> projection = vector<int>(masktmp.rows, 0);
    vector<int> result;
    long moyenne = 0;
    int lignesvert = 0;
    for (int i = 0; i < masktmp.rows; i++)
    {
        for (int j = 0; j < masktmp.cols; j++)
        {
            if (masktmp.at<Vec3b>(i, j)[2] == 0 && masktmp.at<Vec3b>(i, j)[1] == 255 && masktmp.at<Vec3b>(i, j)[0] == 0)
            {
                projection[i]++;
                moyenne++;

                baseImage.at<Vec3b>(i, projection[i]) = {0, 255, 0};
            }
        }
        if (projection[i] > 0)
            lignesvert++;
    }
    moyenne /= lignesvert;
    moyenne = 1.2 * moyenne;

    for (int i = 0; i < projection.size(); i++)
    {
        if (projection[i] < moyenne)
        {
            int prevProjection = projection[i];

            projection[i] = 0;
            cv::line(baseImage, Point(0, i), Point(prevProjection, i), Scalar(255, 255, 255));
        }
    }
    vector<int> blocs;

    int curmax = 0;
    int curmaxi = 0;
    int nombredezeros = 0;
    int zeros_seuil = 20;

    for (int i = 0; i < projection.size(); i++)
    {
        if (projection[i] == 0)
        {
            nombredezeros++;
            if (nombredezeros > zeros_seuil)
            {
                
                nombredezeros=0;
                if (blocs.size()>0)
                {
                    int min_proj_i = blocs[0];
                    for (int bloc_pos : blocs)
                    {
                        if (projection[bloc_pos] < projection[min_proj_i])
                        {
                             min_proj_i = bloc_pos;

                        }
                    }
                    cv::line(blueMask, Point(0, min_proj_i), Point(baseImage.cols - 1, min_proj_i), Scalar(255, 0, 0));
                    cv::line(baseImage, Point(projection[min_proj_i], min_proj_i), Point(baseImage.cols - 1, min_proj_i), Scalar(255, 0, 0));
                    result.push_back(min_proj_i);
                    blocs.clear();

                }

                
            }

        }
        if (projection[i] > curmax)
        {
            nombredezeros=0;

            curmax = projection[i];
            curmaxi = i;
        }
        if (projection[i] == 0 && curmaxi > 0)
        {
            blocs.push_back(curmaxi);
            

            nombredezeros = 0;
            curmax = 0;
            curmaxi = 0;
        }


    }
    this->baseImage = baseImage;
    this->blueMask = blueMask;

    maximums = result;

    imwrite("../../imres/final.jpg", baseImage);
}

void LineDetection::redLineRegression()
{

    int prev_max = maximums[0];
    RNG rng(12345);
    Mat rainbowMask = Mat::zeros(cv::Size(baseImage.cols, baseImage.rows), baseImage.type());
    for (int i = 1; i < maximums.size(); ++i)
    {
        vector<Point2f> points;
        for (int rows = prev_max; rows < maximums[i]; rows++)
        {
            for (int cols = 0; cols < lineMask.cols; cols++)
            {

                if (lineMask.at<Vec3b>(rows, cols)[2] == 255)
                {
                    points.push_back(Point2f(cols, rows));
                }
            }
        }

        Vec4f ligne;

        if (points.size() > 0)
        {
            fitLine(points, ligne, CV_DIST_L2, 0, 0.01, 0.01);

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
        }

        prev_max = maximums[i];
    }
/*
    Mat element = getStructuringElement(MORPH_RECT,
                                        Size(1, 3));

    Mat dilatedMask;

    dilate(rainbowMask, dilatedMask, element);

    this->rainbowMask = dilatedMask;

   */

   this->rainbowMask = rainbowMask;
}

Mat LineDetection::getBaseImage()
{
    return this->baseImage.clone();
}

void LineDetection::TextColoring()
{


    int seuil = 254;
    int interval = 50;

    vector<Vec3b> bannedLines;

    for (int rows = 0; rows < rainbowMask.rows; rows++)
    {
        for (int cols = 0; cols < rainbowMask.cols; cols++)
        {
            if (rainbowMask.at<Vec3b>(rows, cols)[0] != 0 || rainbowMask.at<Vec3b>(rows, cols)[1] != 0 || rainbowMask.at<Vec3b>(rows, cols)[2] != 0)
            {
                if (blueMask.at<Vec3b>(rows, cols)[0] != 0)
                {
                    bannedLines.push_back(rainbowMask.at<Vec3b>(rows, cols));
                }

                if (demoImage.at<Vec3b>(rows, cols)[0] < seuil && demoImage.at<Vec3b>(rows, cols)[1] < seuil && demoImage.at<Vec3b>(rows, cols)[2] < seuil)
                {
                    if (isNotBanned(bannedLines, rainbowMask.at<Vec3b>(rows, cols)))
                    {

                        if (demoImage.at<Vec3b>(rows, cols) != rainbowMask.at<Vec3b>(rows, cols))
                        {
                            if (demoImage.at<Vec3b>(rows, cols)[0] != 0 && demoImage.at<Vec3b>(rows, cols)[1] != 0 && demoImage.at<Vec3b>(rows, cols)[1] != 0)
                            {
                                floodFill(demoImage, Point(cols, rows), Scalar(0,0,255), 0, Scalar(interval, interval, interval), Scalar(interval, interval, interval), FLOODFILL_FIXED_RANGE);
                           
                            }
                            else
                            {
                                floodFill(demoImage, Point(cols, rows), Scalar(rainbowMask.at<Vec3b>(rows, cols)[0], rainbowMask.at<Vec3b>(rows, cols)[1], rainbowMask.at<Vec3b>(rows, cols)[2]), 0, Scalar(interval, interval, interval), Scalar(interval, interval, interval), FLOODFILL_FIXED_RANGE);
                            }
                           
                        }
                    }
                }
            }


        
        }
    }
    this->demoImage = demoImage;

    imwrite("../../imres/final_image_before.jpg", demoImage);
}

void LineDetection::affichage()
{
    
    for (int rows = 0; rows < demoImage.rows; rows++)
    {
        for (int cols = 0; cols < demoImage.cols; cols++)
        {
            
            if (blueMask.at<Vec3b>(rows,cols)[0] != 0)
            {
                demoImage.at<Vec3b>(rows,cols)[0]=255;
                demoImage.at<Vec3b>(rows,cols)[1]=0;
                demoImage.at<Vec3b>(rows,cols)[2]=0;
            }
           
            if (rainbowMask.at<Vec3b>(rows,cols)[0] != 0 || rainbowMask.at<Vec3b>(rows,cols)[1] != 0 || rainbowMask.at<Vec3b>(rows,cols)[1] != 0)
            {
                demoImage.at<Vec3b>(rows,cols) = rainbowMask.at<Vec3b>(rows,cols);

            }
           
            
        }
    }
    
    namedWindow("LES LIGNES", WINDOW_AUTOSIZE);
    imshow("LES LIGNES", lineMask);

    namedWindow("Approximation Lignes", WINDOW_AUTOSIZE);
    imshow("Approximation Lignes", baseImage);
    imwrite("../../imres/final.jpg", baseImage);

    namedWindow("Blue Image", WINDOW_AUTOSIZE);
    imshow("Blue Image", blueMask);

    namedWindow("Final Image", WINDOW_AUTOSIZE);
    imshow("Final Image", demoImage);
    imwrite("../../imres/final_image.jpg", demoImage);
}
Mat LineDetection::getFinalImage()
{
    return this->demoImage;
}
int LineDetection::getMaximumSize()
{
    return maximums.size();
}

void LineDetection::save(int event, int x, int y, int flags, void *userdata)
{
}

void LineDetection::postProcess()
{

       for (int rows = 0; rows < demoImage.rows; rows++)
    {
        for (int cols = 0; cols < demoImage.cols; cols++)
        {
            if (demoImage.at<Vec3b>(rows,cols)[0] == 0 && demoImage.at<Vec3b>(rows,cols)[1] == 0 && demoImage.at<Vec3b>(rows,cols)[1] == 0)
            {
                neirestColoredNeighbors(Point(cols,rows));

            }
        }
    }
                
}

void LineDetection::writeEvalutation(string path)
{
    vector<Vec3b> couleursVisite;
    couleursVisite.push_back({255, 255, 255});
    long count = 0;
    ofstream file;
    file.open(path);

    unsigned int *IM_SegmResult = (unsigned int *)calloc(demoImage.cols * demoImage.rows, sizeof(int));
    FILE *pFile = fopen(path.c_str(), "w");

    for (int rows = 0; rows < demoImage.rows; rows++)
    {
        for (int cols = 0; cols < demoImage.cols; cols++)
        {
            count++;
            bool find = false;
            for (unsigned int tmpindice = 0; tmpindice < couleursVisite.size(); tmpindice++)
            {
                if (couleursVisite[tmpindice][0] == demoImage.at<Vec3b>(rows, cols)[0] &&
                    couleursVisite[tmpindice][1] == demoImage.at<Vec3b>(rows, cols)[1] &&
                    couleursVisite[tmpindice][2] == demoImage.at<Vec3b>(rows, cols)[2])
                {
                    //file <<  tmpindice;
                    char *buff = new char[sizeof(int)];
                    *buff = tmpindice;
                    file.write(buff, sizeof(int));
                    delete[] buff;

                    IM_SegmResult[count] = tmpindice;
                    find = true;
                    break;
                }
            }
            if (!find)
            {
                couleursVisite.push_back(demoImage.at<Vec3b>(rows, cols));
                // test avec .write()

                char *buff = new char[sizeof(int)];
                *buff = couleursVisite.size() - 1;
                //file.write(buff, sizeof(int));

                //delete[] buff;

                IM_SegmResult[count] = couleursVisite.size() - 1;
            }
        }
    }
    // file.close();

    fwrite(IM_SegmResult, sizeof(int), demoImage.cols * demoImage.rows, pFile);
    fclose(pFile);
}

bool LineDetection::isNotBanned(vector<Vec3b> bannedList, Vec3b Color)
{
    for (int i = 0; i < bannedList.size(); i++)
    {
        if (bannedList[i][0] == Color[0] && bannedList[i][1] == Color[1] && bannedList[i][2] == Color[2])
        {
            return false;
        }
    }
    return true;
}

void LineDetection::neirestColoredNeighbors (Point p)
{



    int size = 1;
    bool colored = false;
    int pos_x,pos_y;

    
    while (!colored)
    {
        pos_x = p.x-size;
        pos_y= p.y-size ;

        int pos_x_depart = pos_x;
        int pos_y_depart = pos_y;


        for (pos_x; pos_x<pos_x_depart+2*size; pos_x++)
        {
            blueMask.at<Vec3b>(pos_y,pos_x)[2] = 255;

            if ((demoImage.at<Vec3b>(pos_y,pos_x)[0] != 0 && demoImage.at<Vec3b>(pos_y,pos_x)[1] != 0 && demoImage.at<Vec3b>(pos_y,pos_x)[2] != 0 ) && (demoImage.at<Vec3b>(pos_y,pos_x)[0] != 255 && demoImage.at<Vec3b>(pos_y,pos_x)[1] != 255  && demoImage.at<Vec3b>(pos_y,pos_x)[2] != 255 ))
            {
                 floodFill(demoImage, p, Scalar(demoImage.at<Vec3b>(pos_y, pos_x)[0],demoImage.at<Vec3b>(pos_y, pos_x)[1],demoImage.at<Vec3b>(pos_y, pos_x)[2]), 0, Scalar(10, 10, 10), Scalar(10, 10, 10), FLOODFILL_FIXED_RANGE);
               
                 colored = true;
                break;
            }
        }
        pos_x_depart = pos_x;

        if (colored)
            break;
        for (pos_y; pos_y<pos_y_depart+2*size; pos_y++)
        {
            blueMask.at<Vec3b>(pos_y,pos_x)[1] = 255;
            if ((demoImage.at<Vec3b>(pos_y,pos_x)[0] != 0 && demoImage.at<Vec3b>(pos_y,pos_x)[1] != 0 && demoImage.at<Vec3b>(pos_y,pos_x)[2] != 0 ) && (demoImage.at<Vec3b>(pos_y,pos_x)[0] != 255 && demoImage.at<Vec3b>(pos_y,pos_x)[1] != 255  && demoImage.at<Vec3b>(pos_y,pos_x)[2] != 255 ))
            {
                 floodFill(demoImage, p, Scalar(demoImage.at<Vec3b>(pos_y, pos_x)[0],demoImage.at<Vec3b>(pos_y, pos_x)[1],demoImage.at<Vec3b>(pos_y, pos_x)[2]), 0, Scalar(10, 10, 10), Scalar(10, 10, 10), FLOODFILL_FIXED_RANGE);
               
                   colored = true;
                break;
            }
        }
        pos_y_depart = pos_y;

        if (colored)
            break;

        for (pos_x; pos_x>pos_x_depart-2*size; pos_x--)
        {
            blueMask.at<Vec3b>(pos_y,pos_x)[2] = 255;
           if ((demoImage.at<Vec3b>(pos_y,pos_x)[0] != 0 && demoImage.at<Vec3b>(pos_y,pos_x)[1] != 0 && demoImage.at<Vec3b>(pos_y,pos_x)[2] != 0 ) && (demoImage.at<Vec3b>(pos_y,pos_x)[0] != 255 && demoImage.at<Vec3b>(pos_y,pos_x)[1] != 255  && demoImage.at<Vec3b>(pos_y,pos_x)[2] != 255 ))
             {
                  floodFill(demoImage, p, Scalar(demoImage.at<Vec3b>(pos_y, pos_x)[0],demoImage.at<Vec3b>(pos_y, pos_x)[1],demoImage.at<Vec3b>(pos_y, pos_x)[2]), 0, Scalar(10, 10, 10), Scalar(10, 10, 10), FLOODFILL_FIXED_RANGE);
               
                colored = true;
                break;
            }
        }
        pos_x_depart = pos_x;

        if (colored)
            break;
        for (pos_y; pos_y>pos_y_depart-2*size; pos_y--)
        {
            blueMask.at<Vec3b>(pos_y,pos_x)[1] = 255;
            if ((demoImage.at<Vec3b>(pos_y,pos_x)[0] != 0 && demoImage.at<Vec3b>(pos_y,pos_x)[1] != 0 && demoImage.at<Vec3b>(pos_y,pos_x)[2] != 0 ) && (demoImage.at<Vec3b>(pos_y,pos_x)[0] != 255 && demoImage.at<Vec3b>(pos_y,pos_x)[1] != 255  && demoImage.at<Vec3b>(pos_y,pos_x)[2] != 255 ))
            {
                floodFill(demoImage, p, Scalar(demoImage.at<Vec3b>(pos_y, pos_x)[0],demoImage.at<Vec3b>(pos_y, pos_x)[1],demoImage.at<Vec3b>(pos_y, pos_x)[2]), 0, Scalar(10, 10, 10), Scalar(10, 10, 10), FLOODFILL_FIXED_RANGE);
                  colored = true;
                break;
            }
        }
        pos_y_depart = pos_y;

        size++;


    }

    /*
    int size_line=1;
    colored=false;
    bool down = false;
    while(!colored){
        blueMask.at<Vec3b>(p.y+size_line, p.x)[2] = 255;
        blueMask.at<Vec3b>(p.y-size_line,p.x)[2]=255;

        int nbZero = 0;

        if (rainbowMask.at<Vec3b>(p.y+size_line,p.x)[0] == 0)
            nbZero++;
        if (rainbowMask.at<Vec3b>(p.y+size_line,p.x)[1] == 0)
            nbZero++;
        if (rainbowMask.at<Vec3b>(p.y+size_line,p.x)[2] == 0)
            nbZero++;

        if(nbZero<3 ){
            colored =true;
            down = true;
        }
        else{
                nbZero=0;
            if (p.y-size_line > 0 && rainbowMask.at<Vec3b>(p.y-size_line,p.x)[0] == 0)
                nbZero++;
            if (p.y-size_line > 0 && rainbowMask.at<Vec3b>(p.y-size_line,p.x)[1] == 0)
                nbZero++;
            if (p.y-size_line > 0 && rainbowMask.at<Vec3b>(p.y-size_line,p.x)[2] == 0)
                nbZero++;
            if( p.y-size_line > 0 && nbZero<3  )
            {
                colored =true;
            }
            else
            {
                size_line++;   
                if (size> size_line)
                break;
            }
        }
    }
    
        if(down){
            floodFill(demoImage, p, Scalar(rainbowMask.at<Vec3b>(p.y+size_line, p.x)[0],rainbowMask.at<Vec3b>(p.y+size_line, p.x)[1],rainbowMask.at<Vec3b>(p.y+size_line, p.x)[2]), 0, Scalar(10, 10, 10), Scalar(10, 10, 10), FLOODFILL_FIXED_RANGE);
        }
        else{
            floodFill(demoImage, p, Scalar(rainbowMask.at<Vec3b>(p.y-size_line, p.x)[0],rainbowMask.at<Vec3b>(p.y-size_line, p.x)[1],rainbowMask.at<Vec3b>(p.y-size_line, p.x)[2]), 0, Scalar(10, 10, 10), Scalar(10, 10, 10), FLOODFILL_FIXED_RANGE);
        }
        */

}
