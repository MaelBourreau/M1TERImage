#include <iostream>
using namespace std;

#include "PreProcess.h"
#include "LineDetection.h"

int main(int argc, char** argv) {
	if (argc != 2) {
		cout << "Usage: detectionLigne <Image_Path>" << endl;
		return -1;
	}
	

	PreProcess filter;
	bool opened = filter.open(argv[1]);
	if (!opened) {
		cerr << "Error opening image" << endl;
		cout << "Usage: imageBlur <Image_Path>" << endl;
		return -1;
	}
	Mat img;
	
	filter.process();
	imwrite("../../imres/gaussian.jpg",filter.getFinalImage());
	LineDetection detector(filter.getFinalImage(), filter.getInputImage());

	detector.detectLine();


	Mat inputImage = imread(argv[1], IMREAD_COLOR);

	
	waitKey(0);


}
