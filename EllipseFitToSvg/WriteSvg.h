#pragma once

#include <fstream>
#include <functional>
#include "../EllipseUtils/ellipseUtils.h"

class CWriteSvg
{
private:
	std::ostream& stream;
public:
	CWriteSvg(std::ostream& stream);

	void Write(std::function<bool(int, double&, double&)> getPoints, const EllipseUtils::EllipseParameters<double>* ellipseParameters);

private:
	void WriteProlog();
	void WriteEpilog();
	void WritePoints(const std::function<bool(int, double&, double&)>& function);
	void WriteEllipse(const EllipseUtils::EllipseParameters<double>& ellipseParameters);
};