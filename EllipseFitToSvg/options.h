#pragma once
#include <string>
#include <type_traits>
#include "../EllipseUtils/ellipseparams.h"

enum class CommandMode
{
	None,
	FitPoints,
	GeneratePoints
};

enum class FitPointsOutputMode :std::uint8_t
{
	None = 0,
	WriteResultToStdout = 1,
	WriteSvg = 2
};

inline FitPointsOutputMode operator | (FitPointsOutputMode lhs, FitPointsOutputMode rhs)
{
	return (FitPointsOutputMode)(static_cast<std::underlying_type<FitPointsOutputMode>::type>(lhs) | static_cast< std::underlying_type<FitPointsOutputMode>::type> (rhs));
}

inline FitPointsOutputMode operator & (FitPointsOutputMode lhs, FitPointsOutputMode rhs)
{
	return (FitPointsOutputMode)(static_cast<std::underlying_type<FitPointsOutputMode>::type>(lhs) & static_cast< std::underlying_type<FitPointsOutputMode>::type> (rhs));
}


inline FitPointsOutputMode& operator |= (FitPointsOutputMode& lhs, FitPointsOutputMode rhs)
{
	lhs = (FitPointsOutputMode)(static_cast<std::underlying_type<FitPointsOutputMode>::type>(lhs) | static_cast<std::underlying_type<FitPointsOutputMode>::type>(rhs));
	return lhs;
}

class COptions
{
private:
	std::string filenamePoints;
	std::string filenameSvgOutput;
	CommandMode cmdMode;
	FitPointsOutputMode fitPointsOutputMode;
	EllipseUtils::EllipseParameters<double> ellipseParameters;
public:
	COptions();

	bool ParseCommandLine(int argc, char** argv);

	CommandMode GetCommandMode() const { return this->cmdMode; }
	const std::string& GetFilenameForPoints() const { return this->filenamePoints; }
	FitPointsOutputMode GetFitPointsOutputMode() const { return this->fitPointsOutputMode; }
	const std::string& GetFilenameForSvgOutput() const { return this->filenameSvgOutput; }
	const EllipseUtils::EllipseParameters<double>& GetEllipseParameters() const { return this->ellipseParameters; }
private:
	CommandMode ParseCommandMode(const char* sz);
	EllipseUtils::EllipseParameters<double> ParseEllipseParameters(const char* sz);
	EllipseUtils::EllipseParameters<double> ParseEllipseParameters(const std::string& sz);

	static bool ParseDouble(const std::string& s,double* ptrDbl);
};
