// EllipseFitToSvg.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include "ReadPoints.h"

int main(int argc, char** argv)
{
	std::ifstream myfile(R"(D:\DEV\GitHub\EllipseUtils\EllipseFitToSvg\points1.txt)");

	CReadPoints readPoints;
	readPoints.Read(myfile, [](double x, double y)->void {std::cout << x << " " << y << std::endl; });
	return 0;
}

