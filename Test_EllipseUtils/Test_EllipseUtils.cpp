// Test_EllipseUtils.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "options.h"
#include "../EllipseUtils/ellipseUtils.h"

using namespace std;

static bool TestEllipseUtils_1(const COptions& opts)
{
	std::vector<double> posX{ 1191.890202, 1202.992439, 1251.177332, 1290.494167, 1316.046031, 1330.852813, 1352.380307, 1373.278949, 1365.819934, 1390.200345, 1396.257962, 1405.251032, 1408.272274, 1399.518749, 1375.738192, 1354.890642, 1344.181910, 1329.239047, 1291.598685, 1272.002922, 1236.409267, 1193.330425, 1135.603540, 1124.718146, 1058.339563, 1014.500301, 989.403974, 891.826663, 848.007368 };
	std::vector<double> posY{ 56.850882, 63.318848, 97.576252, 138.239738, 169.077405, 194.235056, 230.361399, 266.815964, 315.499247, 325.761950, 335.514097, 398.074500, 490.640574, 559.419796, 633.170436, 687.140494, 709.254771, 730.149767, 783.886493, 811.988798, 850.083598, 883.423782, 921.829060, 930.230122, 953.948848, 962.635142, 965.784117, 971.813619, 963.767163 };
	auto e = EllipseUtils::CEllipseFit<double>::LeastSquaresFitEllipse(posX, posY);

	if (opts.VerboseLog() == true)
	{
		cout << ">> TestEllipseUtils_1 <<" << endl;
		cout << " x0     = " << e.x0 << endl;
		cout << " y0     = " << e.y0 << endl;
		cout << " a      = " << e.a << endl;
		cout << " b      = " << e.b << endl;
		cout << " theta  = " << e.theta << endl;
	}

	bool isCorrect =
		fabs(e.x0 - 960.44263945130331) < 0.0001 &&
		fabs(e.y0 - 486.67764874797450) < 0.0001 &&
		fabs(e.a - 490.00254257630809) < 0.0001 &&
		fabs(e.b - 440.87372019871373) < 0.0001 &&
		fabs(e.theta - (-1.2640157348100098)) < 0.0001;

	if (opts.VerboseLog() == true)
	{
		cout << " -> Result " << (isCorrect ? "ok" : "wrong") << endl << endl;
	}

	return isCorrect;
}

static bool TestEllipseUtils_2(const COptions& opts)
{
	std::vector<float> posX{ 1191.890202f, 1202.992439f, 1251.177332f, 1290.494167f, 1316.046031f, 1330.852813f, 1352.380307f, 1373.278949f, 1365.819934f, 1390.200345f, 1396.257962f, 1405.251032f, 1408.272274f, 1399.518749f, 1375.738192f, 1354.890642f, 1344.181910f, 1329.239047f, 1291.598685f, 1272.002922f, 1236.409267f, 1193.330425f, 1135.603540f, 1124.718146f, 1058.339563f, 1014.500301f, 989.403974f, 891.826663f, 848.007368f };
	std::vector<float> posY{ 56.850882f, 63.318848f, 97.576252f, 138.239738f, 169.077405f, 194.235056f, 230.361399f, 266.815964f, 315.499247f, 325.761950f, 335.514097f, 398.074500f, 490.640574f, 559.419796f, 633.170436f, 687.140494f, 709.254771f, 730.149767f, 783.886493f, 811.988798f, 850.083598f, 883.423782f, 921.829060f, 930.230122f, 953.948848f, 962.635142f, 965.784117f, 971.813619f, 963.767163f };
	auto e = EllipseUtils::CEllipseFit<float>::LeastSquaresFitEllipse(posX, posY);

	if (opts.VerboseLog() == true)
	{
		cout << ">> TestEllipseUtils_2 <<" << endl;
		cout << " x0     = " << e.x0 << endl;
		cout << " y0     = " << e.y0 << endl;
		cout << " a      = " << e.a << endl;
		cout << " b      = " << e.b << endl;
		cout << " theta  = " << e.theta << endl;
	}

	bool isCorrect =
		fabs(e.x0 - 960.443298f) < 0.0001f &&
		fabs(e.y0 - 486.677917f) < 0.0001f &&
		fabs(e.a - 490.002533f) < 0.0001f &&
		fabs(e.b - 440.873077f) < 0.0001f &&
		fabs(e.theta - (1.87757611f)) < 0.0001f;

	if (opts.VerboseLog() == true)
	{
		cout << " -> Result " << (isCorrect ? "ok" : "wrong") << endl << endl;
	}

	return isCorrect;
}


int main()
{
	COptions options;
	bool b = TestEllipseUtils_1(options);

	b = TestEllipseUtils_2(options);

	return 0;
}

