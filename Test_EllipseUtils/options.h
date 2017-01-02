#pragma once


class COptions
{
private:
	bool verboseLog;
public:
	bool ParseCommandLine(int argc, char** argv);
	
	bool VerboseLog() const { return this->verboseLog; }
};
