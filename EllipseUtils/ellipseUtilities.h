#pragma once

namespace EllipseUtils
{


	template <typename tFloat>
	class CEllipseUtilities
	{
	public:
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
				p2.theta += 1.57079632679489661923 /*M_PI_2*/;
			}

			return p2;
		}

		template <typename number> static number squared(number n)
		{
			return (n*n);
		}
	};
}