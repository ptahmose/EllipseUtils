#include "stdafx.h"
#include "options.h"
#include "optionparser.h"
#include "Utilities.h"
#include <vector>
#include <iostream>
#include "../EllipseUtils/ellipseparams.h"
#include <regex>
#include <sstream>

struct Arg : public option::Arg
{
	static void printError(const char* msg1, const option::Option& opt, const char* msg2)
	{
		fprintf(stderr, "%s", msg1);
		fwrite(opt.name, opt.namelen, 1, stderr);
		fprintf(stderr, "%s", msg2);
	}

	static option::ArgStatus Required(const option::Option& option, bool msg)
	{
		if (option.arg != nullptr)
			return option::ARG_OK;

		if (msg) printError("Option '", option, "' requires an argument\n");
		return option::ARG_ILLEGAL;
	}
};


COptions::COptions()
	: cmdMode(CommandMode::None),
	fitPointsOutputMode(FitPointsOutputMode::None)
{
}

bool COptions::ParseCommandLine(int argc, char** argv)
{
	enum  optionIndex { UNKNOWN, HELP, COMMAND, INPUTPOINTS, FILENAMESVGOUTPUT, GENERATEPOINTS_ELLIPSEPARAMS };

	const option::Descriptor usage[] =
	{
		{ UNKNOWN, 0,"" , ""    ,option::Arg::None, "USAGE: example [options]\n\n"
		"Options:" },
		{ HELP,    0,"" , "help",option::Arg::None, "  --help  \tPrint usage and exit." },
		{ COMMAND,    0,"c", "command",Arg::Required, "  --command, -c  \tMay either be 'fit' or 'generate'." },
		{ INPUTPOINTS,    0,"i", "input",Arg::Required, "  --input, -i  \tThe input file." },
		{ FILENAMESVGOUTPUT,0,"s","svgoutput",Arg::Required, "  --svgoutput, -s  \tThe SVG-output filename." },
		{ GENERATEPOINTS_ELLIPSEPARAMS ,0,"e","ellipseparams",Arg::Required, "  --ellipseparams, -e  \t[only valid in case of 'generate'] The parameters of the ellipse are given as a list of 5 numbers, in the form 'x0,y0,a,b,angle'."},
		{ UNKNOWN, 0,"" ,  ""   ,option::Arg::None, "\nExamples:\n"
		"  example --unknown -- --this_is_no_option\n"
		"  example -unk --plus -ppp file1 file2\n" },
		{ 0,0,0,0,0,0 }
	};

	option::Stats stats(usage, argc, argv);

	std::vector<option::Option> options(stats.options_max);
	std::vector<option::Option> buffer(stats.buffer_max);

	option::Parser parse(usage, argc, argv, &options[0], &buffer[0]);

	if (parse.error())
		return false;

	if (options[HELP] || argc == 0) {
		option::printUsage(std::cout, usage);
		return false;
	}

	for (int i = 0; i < parse.optionsCount(); ++i)
	{
		option::Option& opt = buffer[i];
		//fprintf(stdout, "Argument #%d is ", i);
		switch (opt.index())
		{
		case COMMAND:
			this->cmdMode = ParseCommandMode(opt.arg);
			break;
		case INPUTPOINTS:
			this->filenamePoints = opt.arg;
			break;
		case FILENAMESVGOUTPUT:
			this->filenameSvgOutput = opt.arg;
			this->fitPointsOutputMode |= FitPointsOutputMode::WriteSvg;
			break;
		case GENERATEPOINTS_ELLIPSEPARAMS:
			this->ellipseParameters = ParseEllipseParameters(opt.arg);
			if (!this->ellipseParameters.IsValid())
			{
				return false;
			}

			break;
		}
	}

	if (this->cmdMode == CommandMode::FitPoints)
	{
		this->fitPointsOutputMode |= FitPointsOutputMode::WriteResultToStdout;
	}


	return true;
}

/*static*/CommandMode COptions::ParseCommandMode(const char* sz)
{
	if (CUtilities::strcmp_caseinsensitive("fit", sz) == 0)
	{
		return CommandMode::FitPoints;
	}
	else if (CUtilities::strcmp_caseinsensitive("gen", sz) == 0 || CUtilities::strcmp_caseinsensitive("generate", sz) == 0)
	{
		return CommandMode::GeneratePoints;
	}

	return CommandMode::None;
}

/*static*/EllipseUtils::EllipseParameters<double> COptions::ParseEllipseParameters(const char* sz)
{
	return ParseEllipseParameters(std::string(sz));
}

/*static*/EllipseUtils::EllipseParameters<double> COptions::ParseEllipseParameters(const std::string& sz)
{
	std::regex argsRegex("^([^,;|]*)[,;|]([^,;|]*)[,;|]([^,;|]*)[,;|]([^,;|]*)[,;|]([^,;|]*)$");
	std::smatch pieces_match;

	std::string args[5];
	bool successful = false;
	if (std::regex_match(sz, pieces_match, argsRegex))
	{
		if (pieces_match.size() == 6)
		{
			for (int i = 1; i <= 5; ++i)
			{
				args[i - 1] = pieces_match[i].str();
			}

			successful = true;
		}
	}

	EllipseUtils::EllipseParameters<double> params;
	params.Clear();
	if (successful == false)
	{
		std::cerr << "invalid ellipse-parameters: \"" << sz << "\"." << std::endl;
		return params;
	}

	static const struct
	{
		double(EllipseUtils::EllipseParameters<double>::*mfp);
		const char* argName;
	} TableForArguments[] =
	{
		{ &EllipseUtils::EllipseParameters<double>::x0, "x0" },
		{ &EllipseUtils::EllipseParameters<double>::y0, "y0" },
		{ &EllipseUtils::EllipseParameters<double>::a, "a" },
		{ &EllipseUtils::EllipseParameters<double>::b, "b" },
	};

	for (int i = 0; i < 4; ++i)
	{
		bool b = ParseDouble(args[i], &(params.*TableForArguments[i].mfp));
		if (b == false)
		{
			std::cerr << "invalid argument for '" << TableForArguments[i].argName << "' (when parsing ellipse-parameters) -> \"" << args[i] << "\"." << std::endl;
			params.Clear();
			return params;
		}
	}

	// special treatment for the angle parameter - if the number is followed by "deg", "degree" or "°", we interpret
	// the number as given in degrees; otherwise it is assumed to be given in radians
	bool angleGivenInDegrees = false;
	std::string angleStr = args[4];
	std::regex degRegex(R"(^(.*)(?:deg|degree|°)\s*$)");
	if (std::regex_match(angleStr, pieces_match, degRegex))
	{
		if (pieces_match.size() == 2)
		{
			angleStr = pieces_match[1].str();
			angleGivenInDegrees = true;
		}
	}

	bool b = ParseDouble(angleStr, &params.theta);
	if (b == false)
	{
		std::cerr << "invalid argument for 'theta' (when parsing ellipse-parameters) -> \"" << args[4] << "\"." << std::endl;
		params.Clear();
		return params;
	}

	if (angleGivenInDegrees == true)
	{
		params.theta = params.theta / 180.0 * 3.141592653589793238463;
	}

	return params;
}

/*static*/bool COptions::ParseDouble(const std::string& s, double* ptrDbl)
{
	std::istringstream iss(s);
	double parsed;
	if (!(iss >> parsed))
	{
		// couldn't parse a double
		return false;
	}

	if (!(iss >> std::ws && iss.eof()))
	{
		// something after the double that wasn't whitespace
		return false;
	}

	if (ptrDbl != nullptr)
	{
		*ptrDbl = parsed;
	}

	return true;
}
