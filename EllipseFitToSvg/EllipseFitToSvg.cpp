// EllipseFitToSvg.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include "ReadPoints.h"
#include "WriteSvg.h"
#include "../EllipseUtils/ellipseUtils.h"

int main(int argc, char** argv)
{
	std::ifstream myfile(R"(D:\DEV\GitHub\EllipseUtils\EllipseFitToSvg\points1.txt)");

	CReadPoints readPoints;
	//readPoints.Read(myfile, [](double x, double y)->void {std::cout << x << " " << y << std::endl; });
	auto points = readPoints.ReadD(myfile);

	CWriteSvg writeSvg(std::cout);

	EllipseUtils::EllipseParameters<double> ellipse_parameters;
	ellipse_parameters.x0 = 960.44263945130331;
	ellipse_parameters.y0 = 486.67764874797450;
	ellipse_parameters.a = 490.00254257630809;
	ellipse_parameters.b = 440.87372019871373;
	ellipse_parameters.theta = -1.2640157348100098;

	writeSvg.Write([&](int idx, double& x, double& y)->bool
	{
		if (idx >= std::get<0>(points).size())
		{
			return false;
		}

		x = std::get<0>(points).at(idx);
		y = std::get<1>(points).at(idx);
		return true;
	},
		&ellipse_parameters);

	return 0;
}

