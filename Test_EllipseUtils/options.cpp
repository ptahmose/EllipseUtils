#include "stdafx.h"
#include "options.h"
#include "optionparser.h"
#include <iostream>

#define PROGRAM_NAME "Test_EllipseUtils"

bool COptions::ParseCommandLine(int argc, char** argv)
{
	enum  optionIndex { UNKNOWN, HELP, VERBOSE };

	static const option::Descriptor usage[] =
	{
		{ UNKNOWN, 0, ""  , "",        option::Arg::None, "USAGE: " PROGRAM_NAME " [options]\n\n"	
			"This program will execute a couple of unit-tests and print success or failure.\n" "Options:" },
		{ HELP,    0, "h" , "help",    option::Arg::None, "  --help  \tPrint usage and exit." },
		{ VERBOSE, 0, "v" , "verbose", option::Arg::None, "  --verbose, -v  \tVerbose output." },
		{ UNKNOWN, 0, ""  ,  "",       option::Arg::None, "\nExamples:\n"
		"  " PROGRAM_NAME " --verbose \n"
		"  " PROGRAM_NAME " -h\n" },
		{ 0,0,0,0,0,0 }
	};

	option::Stats stats(usage, argc, argv);

	std::vector<option::Option> options(stats.options_max);
	std::vector<option::Option> buffer(stats.buffer_max);

	option::Parser parse(usage, argc, argv, &options[0], &buffer[0]);

	if (parse.error())
	{
		return false;
	}

	if (options[HELP])
	{
		option::printUsage(std::cout, usage);
		return false;
	}

	if (options[VERBOSE])
	{
		this->verboseLog = true;
	}

	return true;
}
