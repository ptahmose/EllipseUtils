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

#include <vector>
#include <Eigen/Eigenvalues>
#include "ellipseparams.h"
#include "ellipseUtilities.h"

namespace EllipseUtils
{
	template <typename tFloat>
	class CEllipseFit
	{
	private:
		static const Eigen::Matrix<tFloat, 3, 3> MatrixConstant;
	public:
		static EllipseParameters<tFloat> LeastSquaresFitEllipse(const std::vector<tFloat>& posX, const std::vector<tFloat>& posY)
		{
			return CEllipseFit::LeastSquaresFitEllipse((std::min)(posX.size(), posY.size()), posX, posY);
		}

		static EllipseParameters<tFloat> LeastSquaresFitEllipse(size_t count, const std::vector<tFloat>& posX, const std::vector<tFloat>& posY)
		{
			if (count > posX.size() || count > posY.size())
			{
				throw std::invalid_argument("The value specified for count is too large.");
			}

			DataOnTwoVectors d(count, posX, posY);
			return CEllipseFit::LeastSquaresFitEllipse(d);
		}

		template <typename tGetData>
		static EllipseParameters<tFloat> LeastSquaresFitEllipse(const tGetData& gd)
		{
			return CEllipseUtilities::AlgebraicParameterToEllipseParameters<tFloat>(CEllipseFit::LeastSquaresFitEllipseAlgebraicParameters(gd));
		}

		template <typename tGetData>
		static EllipseAlgebraicParameters<tFloat> LeastSquaresFitEllipseAlgebraicParameters(const tGetData& gd)
		{
			if (gd.Count() < 6)
			{
				throw std::invalid_argument("At least 5 point must be specified.");
			}

			// http://autotrace.sourceforge.net/WSCG98.pdf
			tFloat mx, my, minX, minY, maxX, maxY;
			CalcMeanMinMax<tGetData>(gd, mx, minX, maxX, my, minY, maxY);

			tFloat sx = (maxX - minX) / 2;
			tFloat sy = (maxY - minY) / 2;

			Eigen::Matrix<tFloat, 6, 6> scatterMatrix;
			CalcValueInformation calcVInfo{ mx,sx,my,sy };
			for (int r = 0; r < 6; ++r)
			{
				for (int c = r; c < 6; ++c)
				{
					tFloat v = 0;
					for (size_t k = 0; k < gd.Count(); ++k)
					{
						tFloat v1 = calcValue(calcVInfo, gd, k, r);
						tFloat v2 = calcValue(calcVInfo, gd, k, c);
						v += v1*v2;
					}

					scatterMatrix(r, c) = v;
					if (r != c)
					{
						scatterMatrix(c, r) = v;
					}
				}
			}

#if __GNUC__
			const auto matrixb = scatterMatrix.block(3, 3, 3, 0);
#else
			const auto matrixb = scatterMatrix.block<3, 3>(3, 0);
#endif
			const auto matrixbtransposed = matrixb.transpose();
#if __GNUC__
			const auto matrixCInverse = ((scatterMatrix).block(3, 3, 3, 3)).inverse();
#else
			const auto matrixCInverse = ((scatterMatrix).block<3, 3>(3, 3)).inverse();
#endif

#if __GNUC__
			const auto eigenR = CEllipseFit::MatrixConstant * (((scatterMatrix).block(3, 3, 0, 0) - matrixbtransposed * matrixCInverse * matrixb).transpose());
#else
			const auto eigenR = CEllipseFit::MatrixConstant * (((scatterMatrix).block<3, 3>(0, 0) - matrixbtransposed * matrixCInverse * matrixb).transpose());
#endif

			Eigen::Array22f m;
			m << 1, 2, 3, 4;
			auto bl = m.block<2, 2>(1, 1);

			Eigen::EigenSolver<Eigen::Matrix<tFloat, 3, 3>> eigenSolver;
			eigenSolver.compute(eigenR, true);

			int indexPositiveEigenValue = -1;
			const auto eigenval = eigenSolver.eigenvalues();
			for (int i = 0; i < 3; ++i)
			{
				if (eigenval[i].real() < 0)
				{
					indexPositiveEigenValue = i;
					break;
				}
			}

			auto eigenVectors = eigenSolver.eigenvectors();
#if __GNUC__
			const Eigen::Matrix<tFloat, 3, 1> eigenVec = ((eigenSolver.eigenvectors()).block(3, 1, 0, indexPositiveEigenValue)).real();
#else
			const Eigen::Matrix<tFloat, 3, 1> eigenVec = ((eigenSolver.eigenvectors()).block<3, 1>(0, indexPositiveEigenValue)).real();
#endif
			const auto tv0 = -((matrixCInverse.transpose() * matrixb)*eigenVec);

			return EllipseAlgebraicParameters<tFloat>
			{
				eigenVec(0) * sy*sy,
					eigenVec(1) * sx*sy,
					eigenVec(2) * sx*sx,
					-2 * eigenVec(0) * sy*sy*mx - eigenVec(1) * sx*sy*my + tv0(0) * sx*sy*sy,
					-eigenVec(1) * sx*sy*mx - 2 * eigenVec(2) * sx*sx*my + tv0(1) * sx*sx*sy,
					eigenVec(0) * sy*sy*mx*mx + eigenVec(1) * sx*sy*mx*my + eigenVec(2) * sx*sx*my*my - tv0(0) * sx*sy*sy*mx - tv0(1) * sx*sx*sy*my + tv0(2) * sx*sx*sy*sy
			};
		}
	private:
		template <typename tGetData>
		static void CalcMeanMinMax(const tGetData& g, tFloat& meanX, tFloat& minX, tFloat& maxX, tFloat& meanY, tFloat& minY, tFloat& maxY)
		{
			minX = minY = (std::numeric_limits<tFloat>::max)();
			maxX = maxX = (std::numeric_limits<tFloat>::min)();
			meanX = meanY = 0;

			for (size_t i = 0; i < g.Count(); ++i)
			{
				auto value = g(i);
				if (std::get<0>(value) < minX)
				{
					minX = std::get<0>(value);
				}

				if (std::get<0>(value) > maxX)
				{
					maxX = std::get<0>(value);
				}

				if (std::get<1>(value) < minY)
				{
					minY = std::get<1>(value);
				}

				if (std::get<1>(value) > maxY)
				{
					maxY = std::get<1>(value);
				}

				meanX += std::get<0>(value);
				meanY += std::get<1>(value);
			}

			meanX /= g.Count();
			meanY /= g.Count();
		}

		struct CalcValueInformation
		{
			tFloat mx, sx, my, sy;
		};

		template <typename tGetData>
		static tFloat calcValue(const CalcValueInformation& info, const tGetData& gd, size_t n, int c)
		{
			switch (c)
			{
			case 0:
			{
				auto point = gd(n);
				tFloat x = (std::get<0>(point) - info.mx) / info.sx;
				return x*x;
			}
			case 1:
			{
				auto point = gd(n);
				tFloat x = (std::get<0>(point) - info.mx) / info.sx;
				tFloat y = (std::get<1>(point) - info.my) / info.sy;
				return x*y;
			}
			case 2:
			{
				auto point = gd(n);
				tFloat y = (std::get<1>(point) - info.my) / info.sy;
				return y*y;
			}
			case 3:
			{
				auto point = gd(n);
				tFloat x = (std::get<0>(point) - info.mx) / info.sx;
				return x;
			}
			case 4:
			{
				auto point = gd(n);
				tFloat y = (std::get<1>(point) - info.my) / info.sy;
				return y;
			}
			case 5:
				return 1;
			default:
#if defined(_MSC_VER)
				__assume(0);
#elif defined(__GNUC__)
				do { __builtin_unreachable(); } while (0);
#endif
				return 0;
			}
		}

		class DataOnTwoVectors
		{
		private:
			const std::vector<tFloat>& posX;
			const std::vector<tFloat>& posY;
			size_t size;
		public:
			DataOnTwoVectors(size_t s, const std::vector<tFloat>& x, const std::vector<tFloat>& y)
				: posX(x), posY(y), size(s)
			{}

			std::tuple<tFloat, tFloat> operator()(size_t n) const { return std::make_tuple(this->posX[n], this->posY[n]); }
			size_t Count() const { return this->size; }
		};
	};

	template <typename tFloat>
	const Eigen::Matrix<tFloat, 3, 3> CEllipseFit<tFloat>::MatrixConstant = (Eigen::Matrix<tFloat, 3, 3>() << 0, 0, (tFloat)-0.5, 0, 1, 0, (tFloat)-0.5, 0, 0).finished();

	typedef CEllipseFit<float> CEllipseFitF;
	typedef CEllipseFit<double> CEllipseFitD;
}