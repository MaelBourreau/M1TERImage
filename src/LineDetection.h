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
	Mat outputImage;
	

public:
	/**
	 * Default constructor
	 */
	LineDetection();
	/**
	 * Destructor
	 */
	virtual ~LineDetection();
	
	static void save(int event, int x, int y, int flags, void* userdata);

};

#endif