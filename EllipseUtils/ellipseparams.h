#pragma once

#include <limits>
#include <math.h>

namespace EllipseUtils
{

	/// <summary>	The ellipse algebraic parameters -> a x^2 + b x y + c y^2 + d x + e y + f = 0. </summary>
	template<typename tFloat>
	struct EllipseAlgebraicParameters
	{
		tFloat a, b, c, d, e, f;

		bool IsEllipse() const
		{
			return this->b*this->b - 4 * this->a*this->c < 0;
		}
	};

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

		tFloat GetEccentricity() const
		{
			return sqrt(1 - this->b*this->b / (this->a*this->a));
		}

		bool IsValid() const { return std::isnormal(this->x0); }
	};

	typedef EllipseParameters<float> EllipseParametersF;
	typedef EllipseParameters<double> EllipseParametersD;

	typedef EllipseAlgebraicParameters<float> EllipseAlgebraicParametersF;
	typedef EllipseAlgebraicParameters<double> EllipseAlgebraicParametersD;

}
