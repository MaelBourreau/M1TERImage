#include <iostream>
using namespace std;

#include "PreProcess.h"
#include "LineDetection.h"
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
using namespace boost::filesystem;

int main(int argc, char **argv)
{

	if (argc == 2)
	{
		PreProcess filter;
		bool opened = filter.open(argv[1]);
		path p(argv[1]);

		if (!opened)
		{
			cerr << "Error opening image" << endl;
			cout << "Usage: imageBlur <Image_Path>" << endl;
			return -1;
		}

		filter.process();
		//imwrite("../../imres/gaussian.jpg",filter.getFinalImage());
		LineDetection detector(filter.getFinalImage(), filter.getInputImage(), "salut");

		detector.detectLine();
		detector.greenProjection();
		detector.redLineRegression();
		detector.TextColoring();
		detector.postProcess();
		
		detector.affichage();
		waitKey(0);
		detector.writeEvalutation("../../result/" + p.filename().string() + ".dat");

		

		return 0;
	}
	
}
