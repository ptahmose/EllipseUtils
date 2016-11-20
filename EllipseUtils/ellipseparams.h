/*
*
* Copyright (C) 2016 Juergen Bohl  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*/

#pragma once

#include <limits>
#include <cmath>

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
