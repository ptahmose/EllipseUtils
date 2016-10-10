#include "stdafx.h"
#include "WriteSvg.h"

CWriteSvg::CWriteSvg(std::ofstream& stream) :stream(stream)
{
}

void CWriteSvg::Write(std::function<bool(int, double&, double&)> getPoints)
{

}