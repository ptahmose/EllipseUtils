#include "stdafx.h"
#include <iterator>
#include <sstream>
#include "ReadPoints.h"

CReadPoints::CReadPoints()
{
}

void CReadPoints::Read(std::istream& stream, std::function<void(double, double)> addPoint)
{
	std::string line;
	while (std::getline(stream, line))
	{
		std::string strPt1, strPt2;
		bool b = SplitIntoTwoStrings(line, strPt1, strPt2);
		if (b)
		{
			double px, py;
			if (String_to_double(strPt1, px) == true && String_to_double(strPt2, py))
			{
				addPoint(px, py);
			}
		}
	}
}

std::tuple<std::vector<double>, std::vector<double>> CReadPoints::ReadD(std::istream& stream)
{
	std::vector<double> xPos;
	std::vector<double> yPos;
	this->Read(
		stream,
		[&](double x, double y)->void
	{
		xPos.push_back(x);
		yPos.push_back(y);
	});

	return std::make_tuple(xPos, yPos);
}

bool CReadPoints::SplitIntoTwoStrings(const std::string str, std::string& pt1, std::string& pt2)
{
	std::string::const_iterator it = str.cbegin();
	for (;; ++it)
	{
		if (it == str.cend())
			return false;
		if ((*it) != ' ' && (*it) != '\t')
			break;
	}

	for (;; ++it)
	{
		if (it == str.cend())
			return false;
		char c = *it;
		if (c == ' ' || c == ',' || c == ';')
			break;
		pt1 += c;
	}

	if (*it == ' ')
	{
		for (++it;; ++it)
		{
			if (it == str.cend())
				return false;

			char c = *it;
			if (c != ' ' && c != ',' && c != ';')
			{
				break;
			}
		}
	}

	std::copy(it, str.cend(), std::back_inserter(pt2));
	return true;
}

bool CReadPoints::String_to_double(const std::string& s, double& d)
{
	std::istringstream i(s);
	if (!(i >> d))
	{
		return false;
	}

	return true;
}
