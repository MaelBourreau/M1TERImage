#ifndef SOURCE_DIRECTORY__PreProcess_H_
#define SOURCE_DIRECTORY__PreProcess_H_

#include <iostream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

/**
 * 
 */
class PreProcess
{
  private:
	string inputFilePath;
	Mat inputImage;
	Mat outputImage;
	Mat finalImage;

  public:
	/**
	 * Default constructor
	 */
	PreProcess();
	/**
	 * Destructor
	 */
	virtual ~PreProcess();

	/**
	 * Open and load an image located in the system at filePath
	 */
	bool open(string filePath);

	Mat getFinalImage();
	Mat getInputImage();

	/**
	 * Shows the interface with the input image
	 */
	void process();

	static void gaussian(int size, void *data);
	//Dilatation vertical (kernel de 1)
	static void dilatation(int size, void *data);
	static void save(int event, int x, int y, int flags, void *userdata);
};

#endif
