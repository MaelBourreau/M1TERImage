#include "PreProcess.h"
#include "LineDetection.h"

//global variables

int gaussian_ksize;
int dilatation_ksize = 2;

PreProcess::PreProcess()
{
}

PreProcess::~PreProcess()
{
}

bool PreProcess::open(string filePath)
{
	inputImage = imread(filePath, IMREAD_COLOR);
	//cvtColor(tmp, inputImage, CV_RGB2GRAY);
	if (inputImage.empty())
		return false;
	finalImage = inputImage.clone();

	inputFilePath = filePath;

	return true;
}

void PreProcess::gaussian(int size, void *data)
{

	Mat outputImage = ((PreProcess *)data)->outputImage.clone();
	Mat tmp2 = outputImage.clone();
	if (size % 2 == 0)
		size++;
	GaussianBlur(outputImage, tmp2, Size(size, size), 0);
	GaussianBlur(tmp2, ((PreProcess *)data)->finalImage, Size(size, size), 0);
	imshow("Input Image", ((PreProcess *)data)->finalImage);
}

void PreProcess::dilatation(int size, void *data)
{

	Mat inputImage = ((PreProcess *)data)->inputImage.clone();
	Mat outputImage = ((PreProcess *)data)->outputImage;
	if (size < 2)
		size = 2;

	Mat element = getStructuringElement(MORPH_RECT,
										Size(size, 1));

	erode(inputImage, outputImage, element);
	((PreProcess *)data)->outputImage = outputImage;

	imshow("Input Image", outputImage);
}

void PreProcess::process()
{
	namedWindow("Input Image", WINDOW_AUTOSIZE);

	dilatation(24, this);
	gaussian(24, this);

	//createTrackbar("Dilatation","Input Image", &gaussian_ksize,  50, dilatation, this);
	//createTrackbar("Gaussien", "Input Image", &gaussian_ksize, 100, gaussian, this);

	//setMouseCallback("Input Image",save, this);
	imshow("Input Image", inputImage);
}

void PreProcess::save(int event, int x, int y, int flags, void *userdata)
{
}

Mat PreProcess::getFinalImage()
{
	return this->finalImage.clone();
}

Mat PreProcess::getInputImage()
{
	return this->inputImage.clone();
}
