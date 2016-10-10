#pragma once

#include <fstream>
#include <functional>

class CWriteSvg
{
private:
	std::ofstream& stream;
public:
	CWriteSvg(std::ofstream& stream);

	void Write(std::function<bool(int, double&, double&)> getPoints);
};