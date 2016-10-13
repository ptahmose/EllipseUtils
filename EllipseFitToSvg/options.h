#pragma once
#include <string>

enum class CommandMode
{
	None,
	FitPoints,
	GeneratePoints
};

class COptions
{
private:
	std::string filenamePoints;
	std::string filenameSvgOutput;
	CommandMode cmdMode;
public:
	COptions();

	bool ParseCommandLine(int argc, char** argv);

private:
	CommandMode ParseCommandMode(const char* sz);
};
