#ifndef SOURCE_DIRECTORY__LineDetection_H_
#define SOURCE_DIRECTORY__LineDetection_H_

#include <iostream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

/**
 * 
 */
class LineDetection {
private:
	
	Mat inputImage;
	Mat lineMask;
	Mat greenLineMask;
	Mat redLineMask;
	Mat rainbowMask;

	Mat baseImage;
	Mat demoImage;

	vector<int> maximums;



	

public:
	/**
	 * Default constructor
	 */
	LineDetection(Mat image, Mat imageBase);
	/**
	 * Destructor
	 */
	virtual ~LineDetection();
	void detectLine();
	void greenProjection();
	void redLineRegression();
	Mat getBaseImage();

	void TextColoring();



	static void save(int event, int x, int y, int flags, void* userdata);

};

#endif