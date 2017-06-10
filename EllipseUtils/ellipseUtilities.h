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

#include "ellipseBasicTypes.h"
#include <functional>
#include <Eigen/Eigenvalues>

namespace EllipseUtils
{
	class CEllipseUtilities
	{
	private:
		template<typename tFloat>
		class RunningStat
		{
		public:
			RunningStat() : m_n(0) {}

			void Clear()
			{
				m_n = 0;
			}

			void Push(tFloat x)
			{
				m_n++;

				// See Knuth TAOCP vol 2, 3rd edition, page 232
				if (m_n == 1)
				{
					m_oldM = m_newM = x;
					m_oldS = 0.0;
				}
				else
				{
					m_newM = m_oldM + (x - m_oldM) / m_n;
					m_newS = m_oldS + (x - m_oldM)*(x - m_newM);

					// set up for next iteration
					m_oldM = m_newM;
					m_oldS = m_newS;
				}
			}

			size_t NumDataValues() const { return m_n; }
			tFloat Mean() const { return (m_n > 0) ? m_newM : 0.0; }
			tFloat Variance() const { return ((m_n > 1) ? m_newS / (m_n - 1) : 0.0); }
			tFloat StandardDeviation() const { return std::sqrt(Variance()); }
		private:
			size_t m_n;
			tFloat m_oldM, m_newM, m_oldS, m_newS;
		};
	public:
		template<typename tFloat>
		class CDistanceToEllipse
		{
		private:
			Eigen::Matrix<tFloat, 3, 3>	m, mInv;
		public:
			CDistanceToEllipse() = delete;
			CDistanceToEllipse(const EllipseParameters<tFloat>& params)
			{
				Eigen::Matrix<tFloat, 3, 3> s, r, t;

				// the scaling matrix
				s << params.a, 0, 0,
					0, params.b, 0,
					0, 0, 1;
				auto sinTheta = std::sin(params.theta);
				auto cosTheta = std::cos(params.theta);

				// the rotation matrix
				r << cosTheta, -sinTheta, 0,
					sinTheta, cosTheta, 0,
					0, 0, 1;

				// the translation matrix
				t << 1, 0, params.x0,
					0, 1, params.y0,
					0, 0, 1;

				this->m.noalias() = t*r*s;
				this->mInv.noalias() = this->m.inverse();
			}

			/// <summary>	Calculate the radial distance (which is a good approximation to the distance). </summary>
			/// <param name="x">			The x coordinate of the query point. </param>
			/// <param name="y">			The y coordinate of the query point. </param>
			/// <param name="pRx">			[out] If non-null, the x-coordinate for the "radial point". </param>
			/// <param name="pRy">			[out] If non-null, the y-coordinate for the "radial point". </param>
			/// <param name="pDistance">	[out] If non-null, the distance of the point to the radial point. </param>
			void Calc(tFloat x, tFloat y, tFloat* pRx, tFloat* pRy, tFloat* pDistance, bool* isInsideOfEllipse = nullptr)
			{
				Eigen::Matrix<tFloat, 3, 1> p(x, y, 1);

				// transform the query-point into a coordinate-system where the ellipse is a unit-2-sphere
				auto pT = this->mInv * p;

				auto lengthOfpT = std::sqrt(pT(0)*pT(0) + pT(1)*pT(1));
				if (isInsideOfEllipse != nullptr)
				{
					*isInsideOfEllipse = (lengthOfpT <= 1) ? true : false;
				}

				// then... the nearest point is just a point on this circle ("in the direction" of the query-point)
				Eigen::Matrix<tFloat, 3, 1> qT = pT / lengthOfpT;
				qT(2) = 1;

				// then, transform this point (on the circle) back
				auto q = this->m * qT;

				if (pRx != nullptr)
				{
					*pRx = q(0);
				}
				if (pRy != nullptr)
				{
					*pRy = q(1);
				}

				if (pDistance != nullptr)
				{
					tFloat dx = x - q(0);
					tFloat dy = y - q(1);
					*pDistance = std::sqrt(dx*dx + dy*dy);
				}
			}
		};

		/*static const double PI = 3.141592653589793238463;
		static const float  PI_F = 3.14159265358979f;
		static const double PI_2 = 3.141592653589793238463 / 2;
		static const float  PI_2_F = 3.14159265358979f / 2;*/

		template <typename tFloat>
		static EllipseParameters<tFloat> AlgebraicParameterToEllipseParameters(const EllipseAlgebraicParameters<tFloat>& p1)
		{
			EllipseParameters<tFloat> p2;
			tFloat sigma = 4 * (p1.c*p1.d*p1.d + p1.a*p1.e*p1.e - p1.b*p1.d*p1.e - p1.f*(4 * p1.a*p1.c - p1.b*p1.b)) / squared(4 * p1.a*p1.c - p1.b*p1.b);

			tFloat num = (4 * p1.a*p1.c - p1.b*p1.b);
			p2.x0 = (p1.b*p1.e - 2 * p1.c*p1.d) / num;
			p2.y0 = (p1.b*p1.d - 2 * p1.a*p1.e) / num;

			tFloat v1 = sigma*p1.a + sigma*p1.c + sqrt(squared(sigma*p1.a - sigma*p1.c) + squared(sigma*p1.b));
			tFloat v2 = sigma*p1.a + sigma*p1.c - sqrt(squared(sigma*p1.a - sigma*p1.c) + squared(sigma*p1.b));

			p2.a = sqrt(abs((sigma*p1.a + sigma*p1.c + sqrt(squared(sigma*p1.a - sigma*p1.c) + squared(sigma*p1.b))) / 2));
			p2.b = sqrt(abs((sigma*p1.a + sigma*p1.c - sqrt(squared(sigma*p1.a - sigma*p1.c) + squared(sigma*p1.b))) / 2));

			p2.theta = atan2(p1.b, p1.a - p1.c) / 2;

			// that's the angle between the x-axis and the ellipse's major axis
			// all we have to do is to check what the major axis is...
			if (sigma > 0)
			{
				p2.theta += (tFloat)1.57079632679489661923/*M_PI_2*/;
			}

			return p2;
		}

		/// Calculates the axis aligned bounding box which covers the specified ellipse.
		/// \param ellP The ellipse.
		/// \return The calculated axis aligned bounding box.
		template <typename tFloat>
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


		/// Calculate the coordinates of a point on the ellipse, as specified by the parameter t. The parameter t varies from 0 to 2*Pi.
		/// \param 		    ellipseParams The parameters of the ellipse.
		/// \param 		    t			  The parameter which is used to parametrize the ellipse, which varies from 0 to 2*Pi (aka parametric angle).
		/// \param [in,out] pX			  If non-null, the x-coordinate will be stored here.
		/// \param [in,out] pY			  If non-null, the y-coordinate will be stored here.
		template <typename tFloat>
		static void CalcPointOnEllipse(const EllipseParameters<tFloat>& ellipseParams, tFloat t, tFloat* pX, tFloat* pY)
		{
			tFloat sinTheta = sin(ellipseParams.theta);
			tFloat cosTheta = cos(ellipseParams.theta);
			tFloat sinT = sin(t);
			tFloat cosT = cos(t);

			if (pX != nullptr)
			{
				*pX = ellipseParams.x0 + ellipseParams.a*cosT*cosTheta - ellipseParams.b*sinT*sinTheta;
			}

			if (pY != nullptr)
			{
				*pY = ellipseParams.y0 + ellipseParams.a*cosT*sinTheta + ellipseParams.b*sinT*cosTheta;
			}
		}

		template <typename tFloat>
		static void SampleEllipse(const EllipseParameters<tFloat>& ellipseParams, int numberOfPointsToSample, std::function<bool(tFloat, tFloat)> funcPt)
		{
			tFloat increment = (2 * (tFloat)3.141592653589793238463) / (numberOfPointsToSample);
			tFloat x, y;
			for (int i = 0; i < numberOfPointsToSample; ++i)
			{
				tFloat t = i*increment;
				CalcPointOnEllipse<tFloat>(ellipseParams, t, &x, &y);
				bool b = funcPt(x, y);
				if (b == false)
				{
					break;
				}
			}
		}

		template <typename tFloat>
		static void EstimateErrorOfFit(const EllipseParameters<tFloat>& ellP, std::function<bool(size_t index, tFloat*, tFloat*)> getPntsFnc, tFloat* meanDistance, tFloat* stdDevDistance, tFloat* maxDistance)
		{
			CEllipseUtilities::CDistanceToEllipse<tFloat> dist(ellP);
			CEllipseUtilities::RunningStat<tFloat> stat;
			double _maxDistance = -1;
			for (int i = 0; ; ++i)
			{
				tFloat x, y, d;
				if (getPntsFnc(i, &x, &y) != true)
					break;
				dist.Calc(x, y, nullptr, nullptr, &d);
				stat.Push(d);
				if (_maxDistance < d)
				{
					_maxDistance = d;
				}
			}

			if (meanDistance != nullptr) { *meanDistance = stat.Mean(); }
			if (stdDevDistance != nullptr) { *stdDevDistance = stat.StandardDeviation(); }
			if (maxDistance != nullptr) { *maxDistance = _maxDistance; }
		}
	private:
		template <typename number> static number squared(number n)
		{
			return (n*n);
		}
	};
}