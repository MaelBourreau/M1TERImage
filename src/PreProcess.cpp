#include "PreProcess.h"


//global variables

int gaussian_ksize;
int dilatation_ksize = 2;




PreProcess::PreProcess() {
}

PreProcess::~PreProcess() {
}

bool PreProcess::open(string filePath) {
	inputImage = imread(filePath, IMREAD_COLOR);
	if (inputImage.empty())
		return false;

	stringstream ss;
	ss << filePath << "Blurred.png";
	this->outputFilePath = ss.str();
	return true;
}

void PreProcess::gaussian(int size, void * data)
{
	
	Mat outputImage = ((PreProcess *)data)->outputImage.clone();
	Mat tmp = outputImage.clone();
	if (size%2 == 0)
		size++;
	GaussianBlur(outputImage, tmp,Size(size,size), 0);
	imshow("Input Image", tmp);

}

void PreProcess::dilatation(int size, void* data){

	Mat inputImage = ((PreProcess *)data)->inputImage.clone();
	Mat outputImage = ((PreProcess *)data)->outputImage;
	if (size < 2)
		size = 2;

	Mat element = getStructuringElement( MORPH_RECT,
                                       Size( size,1));
	
	erode(inputImage, outputImage,element);
	((PreProcess *)data)->outputImage = outputImage;

	imshow("Input Image", outputImage);

}

void PreProcess::show() {
	namedWindow("Input Image", WINDOW_AUTOSIZE);

	createTrackbar("Dilatation","Input Image", &gaussian_ksize,  50, dilatation, this);
	createTrackbar("Gaussien", "Input Image", &gaussian_ksize, 100, gaussian, this);

	imshow("Input Image", inputImage);
	waitKey(0);
}

