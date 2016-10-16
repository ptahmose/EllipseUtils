#include "stdafx.h"
#include "options.h"
#include "optionparser.h"
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
		if (option.arg != 0)
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
		}
	}

	if (this->cmdMode == CommandMode::FitPoints)
	{
		this->fitPointsOutputMode |= FitPointsOutputMode::WriteResultToStdout;
	}


	return true;
}

CommandMode COptions::ParseCommandMode(const char* sz)
{
	if (_stricmp("fit", sz) == 0)
	{
		return CommandMode::FitPoints;
	}
	else if (_stricmp("gen", sz) == 0 || _stricmp("generate", sz) == 0)
	{
		return CommandMode::GeneratePoints;
	}

	return CommandMode::None;
}

EllipseUtils::EllipseParameters<double> COptions::ParseEllipseParameters(const char* sz)
{
	return ParseEllipseParameters(std::string(sz));
}

EllipseUtils::EllipseParameters<double> COptions::ParseEllipseParameters(const std::string& sz)
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
		//for (size_t i = 0; i < pieces_match.size(); ++i)
		//{
		//	std::ssub_match sub_match = pieces_match[i];
		//	std::string piece = sub_match.str();
		//	std::cout << "  submatch " << i << ": " << piece << '\n';
		//}
	}

	EllipseUtils::EllipseParameters<double> params;
	params.Clear();
	if (successful == false)
	{
		std::cerr << "invalid ellipse-parameters: \"" << sz << "\"" << std::endl;
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
			std::cerr << "invalid argument for '" << TableForArguments[i].argName << "' (when parsing ellipse-parameters) -> \"" << args[i] << "\"" << std::endl;
		}
	}
	/*
			b = ParseDouble(args[1], &params.y0);
			if (b == false)
			{
				std::cerr << "invalid argument for 'y0' (when parsing ellipse-parameters) -> \"" << args[1] << "\"" << std::endl;
			}

			b = ParseDouble(args[0], &params.a);
			if (b == false)
			{
				std::cerr << "invalid argument for 'a' (when parsing ellipse-parameters) -> \"" << args[2] << "\"" << std::endl;
			}

			b = ParseDouble(args[0], &params.b);
			if (b == false)
			{
				std::cerr << "invalid argument for 'b' (when parsing ellipse-parameters) -> \"" << args[3] << "\"" << std::endl;
			}

			b = ParseDouble(args[0], &params.theta);
	*/
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