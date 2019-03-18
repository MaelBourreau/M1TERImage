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
		detector.affichage();
		waitKey(0);
		//detector.writeEvalutation("../../result/" + p.filename().string() + ".dat");

		

		return 0;
	}
	/*
	path src_directory("../../images");
	
	if (is_directory(src_directory))
	{
		directory_iterator it(src_directory), eod;

		int cnt = std::count_if(directory_iterator(src_directory), directory_iterator(), static_cast<bool (*)(const path &)>(is_regular_file));
		int compteur = 1;

		BOOST_FOREACH (path const &p, std::make_pair(it, eod))
		{
			if (is_regular_file(p))
			{
				cout << "processing file " << p.filename().string() << "| " << compteur << "/" << cnt << endl;

				PreProcess filter;
				bool opened = filter.open(p.string());
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
				if (detector.getMaximumSize() == 0)
				{
					imwrite("../../result/" + p.filename().string(), filter.getInputImage());
				}
				else
				{
					detector.redLineRegression();
					detector.TextColoring();
					detector.affichage();
					detector.writeEvalutation("../../result/" + p.filename().string() + ".dat");
					imwrite("../../result/" + p.filename().string(), detector.getFinalImage());
				}
			}
			compteur++;
		}
	}
	*/
}
