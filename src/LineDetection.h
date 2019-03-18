#ifndef SOURCE_DIRECTORY__LineDetection_H_
#define SOURCE_DIRECTORY__LineDetection_H_

#include <iostream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

/**
 * 
 */
class LineDetection
{
  private:
	Mat inputImage;
	Mat lineMask;
	Mat greenLineMask;
	Mat redLineMask;
	Mat rainbowMask;
	Mat blueMask;

	Mat baseImage;
	Mat demoImage;

	string imageName;

	vector<int> maximums;

  public:
	/**
	 * Default constructor
	 */
	LineDetection(Mat image, Mat imageBase, string imageName);
	/**
	 * Destructor
	 */
	virtual ~LineDetection();
	void detectLine();
	void greenProjection();
	void redLineRegression();
	Mat getBaseImage();
	Mat getFinalImage();
	int getMaximumSize();
	void writeEvalutation(string path);

	void TextColoring();
	void affichage();

	static void save(int event, int x, int y, int flags, void *userdata);
	static bool isNotBanned(vector<Vec3b> bannedList, Vec3b Color);
};

#endif