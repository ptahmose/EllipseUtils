#pragma once

#include <tuple>
#include <functional>
#include "../EllipseUtils/ellipseUtils.h"

class CWriteSvg
{
private:
	std::ostream& stream;
public:
	CWriteSvg(std::ostream& stream);

	void Write(std::function<bool(size_t, double&, double&)> getPoints, const EllipseUtils::EllipseParameters<double>* ellipseParameters);

private:
	void WriteProlog(const EllipseUtils::Rect<int>& viewBox);
	void WriteEpilog();
	void WritePoints(const std::function<bool(size_t, double&, double&)>& function);
	void WriteEllipse(const EllipseUtils::EllipseParameters<double>& ellipseParameters);
	EllipseUtils::Rect<int> CalcViewbox(const std::function<bool(size_t, double&, double&)>& function, const EllipseUtils::EllipseParameters<double>* ellipse_parameters);
};