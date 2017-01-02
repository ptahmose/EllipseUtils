// EllipseFitToSvg.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <random>

#include "ReadPoints.h"
#include "WriteSvg.h"
#include "options.h"
#include "../EllipseUtils/ellipseUtils.h"

using namespace std;
using namespace EllipseUtils;

static void DoFitPoints(const COptions& options);
static void DoGeneratePoints(const COptions& options);

int main(int argc, char** argv)
{
	COptions options;
	options.ParseCommandLine(argc - 1, argv + 1);

	switch (options.GetCommandMode())
	{
	case CommandMode::FitPoints:
		DoFitPoints(options);
		break;
	case CommandMode::GeneratePoints:
		DoGeneratePoints(options);
		break;
	default:
		break;
	}

#if false
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
#endif
	return 0;
}

/*static*/void DoFitPoints(const COptions& options)
{
	auto filename = options.GetFilenameForPoints();
	std::istream* stream; bool needToDeleteStream = false;
	if (filename.empty() || filename.compare("-") == 0)
	{
		stream = &std::cin;
	}
	else
	{
		stream = new std::ifstream(filename);
		needToDeleteStream = true;
	}

	CReadPoints readPoints;
	auto points = readPoints.ReadD(*stream);
	if (needToDeleteStream)
	{
		delete stream;
	}

	auto ellipse_parameters = EllipseUtils::CEllipseFit<double>::LeastSquaresFitEllipse(std::get<0>(points), std::get<1>(points));

	if ((options.GetFitPointsOutputMode() & FitPointsOutputMode::WriteResultToStdout) == FitPointsOutputMode::WriteResultToStdout)
	{
		cout << ">> fitted ellipse <<" << endl;
		cout << " x0     = " << ellipse_parameters.x0 << endl;
		cout << " y0     = " << ellipse_parameters.y0 << endl;
		cout << " a      = " << ellipse_parameters.a << endl;
		cout << " b      = " << ellipse_parameters.b << endl;
		cout << " theta  = " << ellipse_parameters.theta << endl;
	}

	if ((options.GetFitPointsOutputMode() & FitPointsOutputMode::WriteSvg) == FitPointsOutputMode::WriteSvg)
	{
		//std::unique_ptr<CWriteSvg> writeSvg;
		std::ostream* ostream;
		auto outpfilename = options.GetFilenameForSvgOutput();
		if (outpfilename.empty())
		{
			ostream = &std::cout;
			needToDeleteStream = false;
		}
		else
		{
			ostream = new std::ofstream(outpfilename);
			needToDeleteStream = true;
		}

		CWriteSvg writeSvg(*ostream);
		writeSvg.Write(
			[&](size_t idx, double& x, double& y)->bool
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

		if (needToDeleteStream)
		{
			delete ostream;
		}
	}
}

/*static*/void DoGeneratePoints(const COptions& options)
{
	GenerateEllipseParameters genParams = options.GetGenerateEllipseParameters();

	std::mt19937 generator;
	double mean = 0;
	std::normal_distribution<double> normalDistX(mean, genParams.stdDevX);
	std::normal_distribution<double> normalDistY(mean, genParams.stdDevY);

	CEllipseUtilities::SampleEllipse<double>(
		genParams.ellipseParameters,
		genParams.numberOfPointsToSample,
		[&](double x, double y)->bool
			{
				x += normalDistX(generator);
				y += normalDistY(generator);
				cout << x << " , " << y << endl;
				return true;
			}
		);
}
