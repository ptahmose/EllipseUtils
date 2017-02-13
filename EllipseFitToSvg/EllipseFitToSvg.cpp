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
	bool b = options.ParseCommandLine(argc - 1, argv + 1);
	if (b == false)
	{
		fprintf(stderr, "There was an error parsing the command-line argument -> exiting\n");
		return EXIT_FAILURE;
	}

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

	return EXIT_SUCCESS;
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

	if (genParams.stdDevX == 0 && genParams.stdDevY == 0)
	{
		CEllipseUtilities::SampleEllipse<double>(
			genParams.ellipseParameters,
			genParams.numberOfPointsToSample,
			[&](double x, double y)->bool
		{
			cout << x << " , " << y << endl;
			return true;
		}
		);
	}
	else
	{
		std::mt19937 generator;
		if (genParams.rng_seed_valid)
		{
			generator.seed(genParams.rng_seed);
		}
		else
		{
			generator.seed(std::random_device()());
		}

		const double mean = 0;
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
}
