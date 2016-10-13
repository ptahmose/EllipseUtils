#include "stdafx.h"
#include "options.h"
#include "optionparser.h"
#include <vector>
#include <iostream>

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
	: cmdMode(CommandMode::None)
{
}

bool COptions::ParseCommandLine(int argc, char** argv)
{
	enum  optionIndex { UNKNOWN, HELP, COMMAND , INPUTPOINTS};

	const option::Descriptor usage[] =
	{
		{ UNKNOWN, 0,"" , ""    ,option::Arg::None, "USAGE: example [options]\n\n"
		"Options:" },
		{ HELP,    0,"" , "help",option::Arg::None, "  --help  \tPrint usage and exit." },
		{ COMMAND,    0,"c", "command",Arg::Required, "  --command, -c  \tMay either be 'fit' or 'generate'." },
		{ INPUTPOINTS,    0,"i", "input",Arg::Required, "  --input, -i  \tThe input file." },
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
		fprintf(stdout, "Argument #%d is ", i);
		switch (opt.index())
		{
		case COMMAND:
			this->cmdMode = ParseCommandMode(opt.arg);
			break;
		case INPUTPOINTS:
			this->filenamePoints = opt.arg;
			break;
		}
	}


	return true;
}

CommandMode COptions::ParseCommandMode(const char* sz)
{
	if (_stricmp("fit",sz)==0)
	{
		return CommandMode::FitPoints;
	}
	else if (_stricmp("gen", sz) == 0|| _stricmp("generate", sz) == 0)
	{
		return CommandMode::GeneratePoints;
	}

	return CommandMode::None;
}