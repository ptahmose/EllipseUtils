#pragma once

template<typename tFloat>
struct  EllipseParameters
{
	tFloat x0, y0;

	/// <summary>	The length of the semi-minor axis. </summary>
	tFloat a;

	/// <summary>	The length of the semi-major axis. </summary>
	tFloat b;

	/// <summary>	The angle  between the x-axis and the major-axis. < / summary>
	tFloat theta;

	void Clear() { this->x0 = this->y0 = this->a = this->b = this->theta = std::numeric_limits<tFloat>::quiet_NaN(); }

	double GetEccentricity() const
	{
		return sqrt(1 - this->b*this->b / (this->a*this->a));
	}

	bool IsValid() const { return std::isnormal(this->x0); }
};

typedef EllipseParameters<float> EllipseParametersF;
typedef EllipseParameters<double> EllipseParametersD;
