#pragma once

#include "ellipseBasicTypes.h"

namespace EllipseUtils
{


	class CEllipseUtilities
	{
	public:
		/*static const double PI = 3.141592653589793238463;
		static const float  PI_F = 3.14159265358979f;
		static const double PI_2 = 3.141592653589793238463 / 2;
		static const float  PI_2_F = 3.14159265358979f / 2;*/

		template <typename tFloat>
		static EllipseParameters<tFloat> AlgebraicParameterToEllipseParameters(const EllipseAlgebraicParameters<tFloat>& p1)
		{
			EllipseParameters<tFloat> p2;
			tFloat sigma = 4 * (p1.c*p1.d*p1.d + p1.a*p1.e*p1.e - p1.b*p1.d*p1.e - p1.f*(4 * p1.a*p1.c - p1.b*p1.b)) / squared(4 * p1.a*p1.c - p1.b*p1.b);
			//double oneoversigma = 1 / sigma;

			tFloat num = (4 * p1.a*p1.c - p1.b*p1.b);
			p2.x0 = (p1.b*p1.e - 2 * p1.c*p1.d) / num;
			p2.y0 = (p1.b*p1.d - 2 * p1.a*p1.e) / num;

			tFloat v1 = sigma*p1.a + sigma*p1.c + sqrt(squared(sigma*p1.a - sigma*p1.c) + squared(sigma*p1.b));
			tFloat v2 = sigma*p1.a + sigma*p1.c - sqrt(squared(sigma*p1.a - sigma*p1.c) + squared(sigma*p1.b));

			p2.a = sqrt(abs((sigma*p1.a + sigma*p1.c + sqrt(squared(sigma*p1.a - sigma*p1.c) + squared(sigma*p1.b))) / 2));
			p2.b = sqrt(abs((sigma*p1.a + sigma*p1.c - sqrt(squared(sigma*p1.a - sigma*p1.c) + squared(sigma*p1.b))) / 2));

			// p2.theta = acot((p1.a - p1.c) / p1.b) / 2;  --> why not working?
			p2.theta = atan2(p1.b, p1.a - p1.c) / 2;

			// that's the angle between the x-axis and the ellipse's major axis
			// all we have to do is to check what the major axis is...
			if (sigma > 0)
			{
				p2.theta += (tFloat)1.57079632679489661923/*M_PI_2*/;
			}

			return p2;
		}

		template <typename tFloat>

		/// Calculates the axis aligned bounding box which covers the specified ellipse.
		/// \param ellP The ellipse.
		/// \return The calculated axis aligned bounding box.
		static Rect<tFloat> CalcAxisAlignedBoundingBox(const EllipseParameters<tFloat>& ellP)
		{
			tFloat a = ellP.a* cos(ellP.theta);
			tFloat b = ellP.b * sin(ellP.theta);
			tFloat c = ellP.a * sin(ellP.theta);
			tFloat d = ellP.b * cos(ellP.theta);
			tFloat width = sqrt(a*a + b*b) * 2;
			tFloat height = sqrt(c*c + d*d) * 2;
			tFloat x = ellP.x0 - width / 2;
			tFloat y = ellP.y0 - height / 2;
			return Rect<tFloat>{ x, y, width, height };
		}

	private:
		template <typename number> static number squared(number n)
		{
			return (n*n);
		}
	};
}