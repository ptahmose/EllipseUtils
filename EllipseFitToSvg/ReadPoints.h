#pragma once

#include <string>
#include <fstream>
#include <functional>
#include <vector>
#include <tuple>

class CReadPoints
{
public:
	CReadPoints();

	void Read(std::ifstream& stream,std::function<void(double, double)> addPoint);

	std::tuple<std::vector<double>, std::vector<double>> ReadD(std::ifstream& stream);
private:
	static bool SplitIntoTwoStrings(const std::string str, std::string& pt1, std::string& pt2);
	static bool String_to_double(const std::string& s, double& d);
};