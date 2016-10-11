#include "stdafx.h"
#include "WriteSvg.h"

using namespace std;

CWriteSvg::CWriteSvg(std::ostream& stream) :stream(stream)
{
}

void CWriteSvg::Write(std::function<bool(int, double&, double&)> getPoints, const EllipseUtils::EllipseParameters<double>* ellipseParameters)
{
	this->WriteProlog();

	this->WritePoints(getPoints);

	if (ellipseParameters != nullptr)
	{
		this->WriteEllipse(*ellipseParameters);
	}

	this->WriteEpilog();
}

void CWriteSvg::WriteProlog()
{
	static const char* Prolog =
		R"(<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width = "1456px" height = "2592px" version = "1.1"
xmlns = "http://www.w3.org/2000/svg" xmlns:xlink = "http://www.w3.org/1999/xlink">)";

	this->stream << Prolog << endl;
}

void CWriteSvg::WriteEpilog()
{
	this->stream << R"(</svg>)" << endl;
}

void CWriteSvg::WriteEllipse(const EllipseUtils::EllipseParameters<double>& ellipseParameters)
{
	this->stream << R"(<g transform = "translate()" << ellipseParameters.x0 << " " << ellipseParameters.y0 << ") rotate(" << (ellipseParameters.theta / 3.141592653589793238463) * 180 << ")\">" << endl;
	this->stream << R"(<ellipse cx=")" << 0 << R"(" cy = ")" << 0 << R"(" rx = ")" << ellipseParameters.a << R"(" ry = ")" << ellipseParameters.b << R"(" fill = "none" stroke = "purple" stroke-width = "1" />)" << endl;
	this->stream << R"(</g>)" << endl;
}

void CWriteSvg::WritePoints(const std::function<bool(int, double&, double&)>& function)
{
	for (int i = 0;; ++i)
	{
		double x, y;
		if (function(i, x, y) != true)
		{
			return;
		}

		const float DeltaX = 1;
		const float DeltaY = 1;

		float x1, y1, x2, y2;
		x1 = -DeltaX + (float)x;
		y1 = -DeltaY + (float)y;
		x2 = DeltaX + (float)x;
		y2 = DeltaY + (float)y;

		float x3, y3, x4, y4;
		x3 = DeltaX + (float)x;
		y3 = -DeltaY + (float)y;
		x4 = -DeltaX + (float)x;
		y4 = DeltaY + (float)y;

		this->stream << R"(<line x1 = ")" << x1 << R"(" y1 = ")" << y1 << R"(" x2 = ")" << x2 << R"(" y2 = ")" << y2 << R"(" stroke-width = "2.000000" stroke = "black" />)" << endl;
		this->stream << R"(<line x1 = ")" << x3 << R"(" y1 = ")" << y3 << R"(" x2 = ")" << x4 << R"(" y2 = ")" << y4 << R"(" stroke-width = "2.000000" stroke = "black" />)" << endl;
	}
}