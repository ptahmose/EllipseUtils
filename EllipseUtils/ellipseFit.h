#pragma once

#include <Eigen\Eigenvalues>
#include <vector>
#include <memory>
#include "ellipseparams.h"
#include "ellipseUtilities.h"

#include <iostream>
using namespace std;

namespace EllipseUtils
{

	template <typename tFloat>
	class CEllipseFit
	{
	public:
		static EllipseParameters<tFloat> LeastSquaresFitEllipse(const std::vector<tFloat>& posX, const std::vector<tFloat>& posY)
		{
			return LeastSquaresFitEllipse((std::min)(posX.size(), posY.size()), posX, posY);
		}

		static EllipseParameters<tFloat> LeastSquaresFitEllipse(size_t count, const std::vector<tFloat>& posX, const std::vector<tFloat>& posY)
		{
			if (count > posX.size() || count > posY.size())
			{
				throw std::invalid_argument("The value specified for count is too large.");
			}

			return LeastSquaresFitEllipse(count,
				[&](size_t idx, tFloat* x, tFloat* y)->void
			{
				if (x != nullptr)
				{
					*x = posX[idx];
				}

				if (y != nullptr)
				{
					*y = posY[idx];
				}
			});
		}

		static EllipseParameters<tFloat> LeastSquaresFitEllipse(size_t pntCnt, std::function<void(size_t index, tFloat*, tFloat*)> getPntsFnc)
		{
			if (pntCnt < 6)
			{
				throw std::invalid_argument("At least 5 point must be specified.");
			}

			tFloat mx, my; tFloat minX, minY, maxX, maxY;
			CalcMeanMinMax(
				pntCnt,
				[&](size_t index)
			{
				tFloat x;
				getPntsFnc(index, &x, nullptr);
				return x;
			},
				mx, minX, maxX);
			CalcMeanMinMax(
				pntCnt,
				[&](size_t index)
			{
				tFloat y;
				getPntsFnc(index, nullptr, &y);
				return y;
			},
				my, minY, maxY);
			tFloat sx = (maxX - minX) / 2;
			tFloat sy = (maxY - minY) / 2;
			//double* designM = (double*)malloc(pntCnt * 6 * sizeof(double));

			std::unique_ptr<tFloat> designM(new tFloat[pntCnt * 6]);
			for (size_t i = 0; i < pntCnt; ++i)
			{
				tFloat x, y;
				getPntsFnc(i, &x, &y);
				x = (x - mx) / sx;
				y = (y - my) / sy;
				designM.get()[i * 6 + 0] = squared(x);
				designM.get()[i * 6 + 1] = x*y;
				designM.get()[i * 6 + 2] = squared(y);
				designM.get()[i * 6 + 3] = x;
				designM.get()[i * 6 + 4] = y;
				designM.get()[i * 6 + 5] = 1;
			}

			Eigen::Matrix<tFloat, 6, 6> scatterMatrix;

			//double* scatterM = (double*)malloc(6 * 6 * sizeof(double));
			tFloat scatterM[6 * 6];
			for (int r = 0; r < 6; ++r)
			{
				for (int c = 0; c < 6; ++c)
				{
					tFloat v = 0;
					for (size_t k = 0; k < pntCnt; ++k)
					{
						tFloat v1 = designM.get()[k * 6 + r];
						tFloat v2 = designM.get()[c + k * 6];
						v += v1*v2;
					}

					scatterM[r * 6 + c] = v;
					scatterMatrix(r, c) = v;
				}
			}

			auto upperLeft = scatterMatrix.block<3, 3>(3, 0);
			auto lowerRight = scatterMatrix.block<3, 3>(3, 3);

			cout << "scatterMatrix:" << endl << scatterMatrix << endl << endl;
			cout << "upperLeft:" << endl << upperLeft << endl << endl;
			cout << "lowerRight:" << endl << lowerRight << endl << endl;

			cout << "lowerRight^-1" << endl << lowerRight.inverse() << endl << endl;

			auto bMatrix = upperLeft * ((lowerRight.inverse()) * (upperLeft.transpose()));
			cout << "b:" << endl << bMatrix << endl << endl;


			designM.reset();

			//double* tmpBtimestmpE = (double*)malloc(3 * 3 * sizeof(double));
			tFloat tmpBtimestmpE[3 * 3];
			CalcTmpBtimesTmpE(scatterM + 3, 6 * sizeof(tFloat), scatterM + (3 * 6) + 3, 6 * sizeof(tFloat), tmpBtimestmpE);

			//double* testA = (double*)malloc(3 * 3 * sizeof(double));
			tFloat testA[3 * 3];
			CalcTestA(scatterM, 6 * sizeof(tFloat), scatterM + 3, 6 * sizeof(tFloat), scatterM + (3 * 6) + 3, 6 * sizeof(tFloat), testA);

			auto matrixa = Eigen::Matrix<tFloat, 3, 3> (scatterMatrix.block<3, 3>(0, 0)).transpose();
			auto matrixb = Eigen::Matrix<tFloat, 3, 3>(scatterMatrix.block<3, 3>(3, 0)).transpose();
			auto matrixc = Eigen::Matrix<tFloat, 3, 3>(scatterMatrix.block<3, 3>(3, 3)).transpose();
			auto matrixbtransposed = matrixb.transpose();
			cout << "matrixa:" << endl << matrixa << endl << endl;
			cout << "matrixb:" << endl << matrixb << endl << endl;
			cout << "matrixc:" << endl << matrixc << endl << endl;
			cout << "matrixbtransposed :" << endl << matrixbtransposed << endl << endl;


			Eigen::Matrix<tFloat, 3, 3> matrixconst;
			matrixconst << 0, 0, -0.5, 0, 1, 0, -0.5, 0, 0;
			cout << "matrixconst :" << endl << matrixconst << endl << endl;

			auto eigenR = matrixconst * (matrixa - matrixb*(matrixc.inverse()* /*matrixb.transpose()*/matrixbtransposed));
			cout << "testA:" << endl << eigenR << endl << endl;

			Eigen::EigenSolver<Eigen::Matrix<tFloat, 3, 3>> eigenSolver;

			Eigen::Matrix<tFloat, 3, 3> m;
			m(0, 0) = testA[0];
			m(0, 1) = testA[1];
			m(0, 2) = testA[2];
			m(1, 0) = testA[3];
			m(1, 1) = testA[4];
			m(1, 2) = testA[5];
			m(2, 0) = testA[6];
			m(2, 1) = testA[7];
			m(2, 2) = testA[8];

			eigenSolver.compute(m, true);

			int indexPositiveEigenValue = -1;
			auto eigenval = eigenSolver.eigenvalues();
			for (int i = 0; i < 3; ++i)
			{
				if (eigenval[i].real() < 0)
				{
					indexPositiveEigenValue = i;
					break;
				}
			}

			tFloat A[6];
			auto eigenVecs = eigenSolver.eigenvectors();
			A[0] = eigenVecs(0, indexPositiveEigenValue).real();
			A[1] = eigenVecs(1, indexPositiveEigenValue).real();
			A[2] = eigenVecs(2, indexPositiveEigenValue).real();

			CalcLowerHalf(scatterM + 3, 6 * sizeof(tFloat), scatterM + (3 * 6) + 3, 6 * sizeof(tFloat), A, A + 3);

			tFloat par[6];
			par[0] = A[0] * sy*sy;
			par[1] = A[1] * sx*sy;
			par[2] = A[2] * sx*sx;
			par[3] = -2 * A[0] * sy*sy*mx - A[1] * sx*sy*my + A[3] * sx*sy*sy;
			par[4] = -A[1] * sx*sy*mx - 2 * A[2] * sx*sx*my + A[4] * sx*sx*sy;
			par[5] = A[0] * sy*sy*mx*mx + A[1] * sx*sy*mx*my + A[2] * sx*sx*my*my
				- A[3] * sx*sy*sy*mx - A[4] * sx*sx*sy*my
				+ A[5] * sx*sx*sy*sy;


			EllipseAlgebraicParameters<tFloat> ep;
			ep.a = par[0];
			ep.b = par[1];
			ep.c = par[2];
			ep.d = par[3];
			ep.e = par[4];
			ep.f = par[5];

			//EllipseParameters elliParams;
			return CEllipseUtilities::AlgebraicParameterToEllipseParameters<tFloat>(ep);
		}

	private:
		template <typename number> static number squared(number n)
		{
			return (n*n);
		}

		static void CalcMeanMinMax(size_t pntCnt, std::function<tFloat(size_t index)> getPntFnc, tFloat& mean, tFloat& min, tFloat& max)
		{
			min = (std::numeric_limits<tFloat>::max)();
			max = (std::numeric_limits<tFloat>::min)();
			mean = 0;

			for (size_t i = 0; i < pntCnt; ++i)
			{
				double value = getPntFnc(i);
				if (value < min)
				{
					min = value;
				}

				if (value > max)
				{
					max = value;
				}

				mean += value;
			}

			mean = mean / pntCnt;
		}

		static void CalcTmpBtimesTmpE(const double* pB, int strideB, const double* pC, int strideC, double* pDest)
		{
			/* -> Mathematica:
			{{b11, b12, b13}, {b21, b22, b23}, {b31, b32,
			b33}}.(Inverse[{{c11, c12, c13}, {c21, c22, c23}, {c31, c32,
			c33}}].Transpose[{{b11, b12, b13}, {b21, b22, b23}, {b31, b32,
			b33}}]) // Simplify // CForm
			*/

#define b(r,c) *((double*)(((char*)pB)+(r-1)*strideB+(c-1)*sizeof(double)))
#define c(r,c) *((double*)(((char*)pC)+(r-1)*strideC+(c-1)*sizeof(double)))


			pDest[0] =
				(squared(b(1, 3))*(-(c(1, 2)*c(2, 1)) + c(1, 1)*c(2, 2)) + b(1, 3)*(b(1, 1)*(-(c(1, 3)*c(2, 2)) + c(1, 2)*c(2, 3) - c(2, 2)*c(3, 1) + c(2, 1)*c(3, 2)) + b(1, 2)*(c(1, 3)*c(2, 1) + c(1, 2)*c(3, 1) - c(1, 1)*(c(2, 3) + c(3, 2)))) + squared(b(1, 2))*(-(c(1, 3)*c(3, 1)) + c(1, 1)*c(3, 3)) + b(1, 1)*b(1, 2)*(c(2, 3)*c(3, 1) + c(1, 3)*c(3, 2) - (c(1, 2) + c(2, 1))*c(3, 3)) + squared(b(1, 1))*(-(c(2, 3)*c(3, 2)) + c(2, 2)*c(3, 3))) / (-(c(1, 3)*c(2, 2)*c(3, 1)) + c(1, 2)*c(2, 3)*c(3, 1) + c(1, 3)*c(2, 1)*c(3, 2) - c(1, 1)*c(2, 3)*c(3, 2) - c(1, 2)*c(2, 1)*c(3, 3) + c(1, 1)*c(2, 2)*c(3, 3));

			pDest[1] =
				(b(1, 3) *(-b(2, 3)* c(1, 2)* c(2, 1) + b(2, 3)* c(1, 1)* c(2, 2) + b(2, 2) *c(1, 2) *c(3, 1) - b(2, 1)* c(2, 2)* c(3, 1) -
					b(2, 2)* c(1, 1) *c(3, 2) + b(2, 1)* c(2, 1) *c(3, 2)) +
					b(1, 2) *(b(2, 3)* c(1, 3) *c(2, 1) - b(2, 3)* c(1, 1)* c(2, 3) - b(2, 2)* c(1, 3)* c(3, 1) + b(2, 1)* c(2, 3)* c(3, 1) +
						b(2, 2)* c(1, 1)* c(3, 3) - b(2, 1) *c(2, 1)* c(3, 3)) +
					b(1, 1) *(-b(2, 3)* c(1, 3)* c(2, 2) + b(2, 3)* c(1, 2)* c(2, 3) + b(2, 2)* c(1, 3)* c(3, 2) - b(2, 1)* c(2, 3)* c(3, 2) -
						b(2, 2) *c(1, 2) *c(3, 3) + b(2, 1)* c(2, 2)* c(3, 3))) / (-c(1, 3) *c(2, 2)* c(3, 1) + c(1, 2) *c(2, 3) *c(3, 1) +
							c(1, 3)* c(2, 1) *c(3, 2) - c(1, 1)* c(2, 3) *c(3, 2) - c(1, 2)* c(2, 1)* c(3, 3) + c(1, 1)* c(2, 2)* c(3, 3));

			pDest[2] =
				(b(1, 3)*(-(b(3, 3)*c(1, 2)*c(2, 1)) + b(3, 3)*c(1, 1)*c(2, 2) + b(3, 2)*c(1, 2)*c(3, 1) - b(3, 1)*c(2, 2)*c(3, 1) - b(3, 2)*c(1, 1)*c(3, 2) + b(3, 1)*c(2, 1)*c(3, 2)) + b(1, 2)*(b(3, 3)*c(1, 3)*c(2, 1) - b(3, 3)*c(1, 1)*c(2, 3) - b(3, 2)*c(1, 3)*c(3, 1) + b(3, 1)*c(2, 3)*c(3, 1) + b(3, 2)*c(1, 1)*c(3, 3) - b(3, 1)*c(2, 1)*c(3, 3)) + b(1, 1)*(-(b(3, 3)*c(1, 3)*c(2, 2)) + b(3, 3)*c(1, 2)*c(2, 3) + b(3, 2)*c(1, 3)*c(3, 2) - b(3, 1)*c(2, 3)*c(3, 2) - b(3, 2)*c(1, 2)*c(3, 3) + b(3, 1)*c(2, 2)*c(3, 3))) / (-(c(1, 3)*c(2, 2)*c(3, 1)) + c(1, 2)*c(2, 3)*c(3, 1) + c(1, 3)*c(2, 1)*c(3, 2) - c(1, 1)*c(2, 3)*c(3, 2) - c(1, 2)*c(2, 1)*c(3, 3) + c(1, 1)*c(2, 2)*c(3, 3));

			pDest[3] = (b(1, 3)*(-(b(2, 3)*c(1, 2)*c(2, 1)) + b(2, 2)*c(1, 3)*c(2, 1) + b(2, 3)*c(1, 1)*c(2, 2) - b(2, 1)*c(1, 3)*c(2, 2) - b(2, 2)*c(1, 1)*c(2, 3) + b(2, 1)*c(1, 2)*c(2, 3)) + b(1, 2)*(b(2, 3)*c(1, 2)*c(3, 1) - b(2, 2)*c(1, 3)*c(3, 1) - b(2, 3)*c(1, 1)*c(3, 2) + b(2, 1)*c(1, 3)*c(3, 2) + b(2, 2)*c(1, 1)*c(3, 3) - b(2, 1)*c(1, 2)*c(3, 3)) + b(1, 1)*(-(b(2, 3)*c(2, 2)*c(3, 1)) + b(2, 2)*c(2, 3)*c(3, 1) + b(2, 3)*c(2, 1)*c(3, 2) - b(2, 1)*c(2, 3)*c(3, 2) - b(2, 2)*c(2, 1)*c(3, 3) + b(2, 1)*c(2, 2)*c(3, 3))) /
				(-(c(1, 3)*c(2, 2)*c(3, 1)) + c(1, 2)*c(2, 3)*c(3, 1) + c(1, 3)*c(2, 1)*c(3, 2) - c(1, 1)*c(2, 3)*c(3, 2) - c(1, 2)*c(2, 1)*c(3, 3) + c(1, 1)*c(2, 2)*c(3, 3));

			pDest[4] = (squared(b(2, 3))*(-(c(1, 2)*c(2, 1)) + c(1, 1)*c(2, 2)) + b(2, 3)*(b(2, 1)*(-(c(1, 3)*c(2, 2)) + c(1, 2)*c(2, 3) - c(2, 2)*c(3, 1) + c(2, 1)*c(3, 2)) + b(2, 2)*(c(1, 3)*c(2, 1) + c(1, 2)*c(3, 1) - c(1, 1)*(c(2, 3) + c(3, 2)))) + squared(b(2, 2))*(-(c(1, 3)*c(3, 1)) + c(1, 1)*c(3, 3)) + b(2, 1)*b(2, 2)*(c(2, 3)*c(3, 1) + c(1, 3)*c(3, 2) - (c(1, 2) + c(2, 1))*c(3, 3)) + squared(b(2, 1))*(-(c(2, 3)*c(3, 2)) + c(2, 2)*c(3, 3))) /
				(-(c(1, 3)*c(2, 2)*c(3, 1)) + c(1, 2)*c(2, 3)*c(3, 1) + c(1, 3)*c(2, 1)*c(3, 2) - c(1, 1)*c(2, 3)*c(3, 2) - c(1, 2)*c(2, 1)*c(3, 3) + c(1, 1)*c(2, 2)*c(3, 3));

			pDest[5] = (b(2, 3)*(-(b(3, 3)*c(1, 2)*c(2, 1)) + b(3, 3)*c(1, 1)*c(2, 2) + b(3, 2)*c(1, 2)*c(3, 1) - b(3, 1)*c(2, 2)*c(3, 1) - b(3, 2)*c(1, 1)*c(3, 2) + b(3, 1)*c(2, 1)*c(3, 2)) + b(2, 2)*(b(3, 3)*c(1, 3)*c(2, 1) - b(3, 3)*c(1, 1)*c(2, 3) - b(3, 2)*c(1, 3)*c(3, 1) + b(3, 1)*c(2, 3)*c(3, 1) + b(3, 2)*c(1, 1)*c(3, 3) - b(3, 1)*c(2, 1)*c(3, 3)) +
				b(2, 1)*(-(b(3, 3)*c(1, 3)*c(2, 2)) + b(3, 3)*c(1, 2)*c(2, 3) + b(3, 2)*c(1, 3)*c(3, 2) - b(3, 1)*c(2, 3)*c(3, 2) - b(3, 2)*c(1, 2)*c(3, 3) + b(3, 1)*c(2, 2)*c(3, 3))) / (-(c(1, 3)*c(2, 2)*c(3, 1)) + c(1, 2)*c(2, 3)*c(3, 1) + c(1, 3)*c(2, 1)*c(3, 2) - c(1, 1)*c(2, 3)*c(3, 2) - c(1, 2)*c(2, 1)*c(3, 3) + c(1, 1)*c(2, 2)*c(3, 3));

			pDest[6] = (b(1, 3)*(-(b(3, 3)*c(1, 2)*c(2, 1)) + b(3, 2)*c(1, 3)*c(2, 1) + b(3, 3)*c(1, 1)*c(2, 2) - b(3, 1)*c(1, 3)*c(2, 2) - b(3, 2)*c(1, 1)*c(2, 3) + b(3, 1)*c(1, 2)*c(2, 3)) + b(1, 2)*(b(3, 3)*c(1, 2)*c(3, 1) - b(3, 2)*c(1, 3)*c(3, 1) - b(3, 3)*c(1, 1)*c(3, 2) + b(3, 1)*c(1, 3)*c(3, 2) + b(3, 2)*c(1, 1)*c(3, 3) - b(3, 1)*c(1, 2)*c(3, 3)) + b(1, 1)*(-(b(3, 3)*c(2, 2)*c(3, 1)) + b(3, 2)*c(2, 3)*c(3, 1) + b(3, 3)*c(2, 1)*c(3, 2) - b(3, 1)*c(2, 3)*c(3, 2) - b(3, 2)*c(2, 1)*c(3, 3) + b(3, 1)*c(2, 2)*c(3, 3))) /
				(-(c(1, 3)*c(2, 2)*c(3, 1)) + c(1, 2)*c(2, 3)*c(3, 1) + c(1, 3)*c(2, 1)*c(3, 2) - c(1, 1)*c(2, 3)*c(3, 2) - c(1, 2)*c(2, 1)*c(3, 3) + c(1, 1)*c(2, 2)*c(3, 3));

			pDest[7] = (b(2, 3)*(-(b(3, 3)*c(1, 2)*c(2, 1)) + b(3, 2)*c(1, 3)*c(2, 1) + b(3, 3)*c(1, 1)*c(2, 2) - b(3, 1)*c(1, 3)*c(2, 2) - b(3, 2)*c(1, 1)*c(2, 3) + b(3, 1)*c(1, 2)*c(2, 3)) + b(2, 2)*(b(3, 3)*c(1, 2)*c(3, 1) - b(3, 2)*c(1, 3)*c(3, 1) - b(3, 3)*c(1, 1)*c(3, 2) + b(3, 1)*c(1, 3)*c(3, 2) + b(3, 2)*c(1, 1)*c(3, 3) - b(3, 1)*c(1, 2)*c(3, 3)) +
				b(2, 1)*(-(b(3, 3)*c(2, 2)*c(3, 1)) + b(3, 2)*c(2, 3)*c(3, 1) + b(3, 3)*c(2, 1)*c(3, 2) - b(3, 1)*c(2, 3)*c(3, 2) - b(3, 2)*c(2, 1)*c(3, 3) + b(3, 1)*c(2, 2)*c(3, 3))) / (-(c(1, 3)*c(2, 2)*c(3, 1)) + c(1, 2)*c(2, 3)*c(3, 1) + c(1, 3)*c(2, 1)*c(3, 2) - c(1, 1)*c(2, 3)*c(3, 2) - c(1, 2)*c(2, 1)*c(3, 3) + c(1, 1)*c(2, 2)*c(3, 3));

			pDest[8] = (squared(b(3, 3))*(-(c(1, 2)*c(2, 1)) + c(1, 1)*c(2, 2)) + b(3, 3)*(b(3, 1)*(-(c(1, 3)*c(2, 2)) + c(1, 2)*c(2, 3) - c(2, 2)*c(3, 1) + c(2, 1)*c(3, 2)) + b(3, 2)*(c(1, 3)*c(2, 1) + c(1, 2)*c(3, 1) - c(1, 1)*(c(2, 3) + c(3, 2)))) + squared(b(3, 2))*(-(c(1, 3)*c(3, 1)) + c(1, 1)*c(3, 3)) + b(3, 1)*b(3, 2)*(c(2, 3)*c(3, 1) + c(1, 3)*c(3, 2) - (c(1, 2) + c(2, 1))*c(3, 3)) + squared(b(3, 1))*(-(c(2, 3)*c(3, 2)) + c(2, 2)*c(3, 3))) / (-(c(1, 3)*c(2, 2)*c(3, 1)) + c(1, 2)*c(2, 3)*c(3, 1) + c(1, 3)*c(2, 1)*c(3, 2) - c(1, 1)*c(2, 3)*c(3, 2) - c(1, 2)*c(2, 1)*c(3, 3) + c(1, 1)*c(2, 2)*c(3, 3));
#undef b
#undef c
		}

		static void CalcLowerHalf(const double* pB, int strideB, const double* pC, int strideC, const double* ptrAUpperHalf, double* ptrDest)
		{
#define b(r,c) *((double*)(((char*)pB)+(r-1)*strideB+(c-1)*sizeof(double)))
#define c(r,c) *((double*)(((char*)pC)+(r-1)*strideC+(c-1)*sizeof(double)))

			double a1 = ptrAUpperHalf[0]; double a2 = ptrAUpperHalf[1]; double a3 = ptrAUpperHalf[2];
			ptrDest[0] = (a1*(b(1, 3)*c(1, 3)*c(2, 2) - b(1, 3)*c(1, 2)*c(2, 3) - b(1, 2)*c(1, 3)*c(3, 2) + b(1, 1)*c(2, 3)*c(3, 2) + b(1, 2)*c(1, 2)*c(3, 3) - b(1, 1)*c(2, 2)*c(3, 3)) + a2*(b(2, 3)*c(1, 3)*c(2, 2) - b(2, 3)*c(1, 2)*c(2, 3) - b(2, 2)*c(1, 3)*c(3, 2) + b(2, 1)*c(2, 3)*c(3, 2) + b(2, 2)*c(1, 2)*c(3, 3) - b(2, 1)*c(2, 2)*c(3, 3)) + a3*(b(3, 3)*c(1, 3)*c(2, 2) - b(3, 3)*c(1, 2)*c(2, 3) - b(3, 2)*c(1, 3)*c(3, 2) + b(3, 1)*c(2, 3)*c(3, 2) + b(3, 2)*c(1, 2)*c(3, 3) - b(3, 1)*c(2, 2)*c(3, 3))) / (-(c(1, 3)*c(2, 2)*c(3, 1)) + c(1, 2)*c(2, 3)*c(3, 1) + c(1, 3)*c(2, 1)*c(3, 2) - c(1, 1)*c(2, 3)*c(3, 2) - c(1, 2)*c(2, 1)*c(3, 3) + c(1, 1)*c(2, 2)*c(3, 3));
			ptrDest[1] = (a1*(-(b(1, 3)*c(1, 3)*c(2, 1)) + b(1, 3)*c(1, 1)*c(2, 3) + b(1, 2)*c(1, 3)*c(3, 1) - b(1, 1)*c(2, 3)*c(3, 1) - b(1, 2)*c(1, 1)*c(3, 3) + b(1, 1)*c(2, 1)*c(3, 3)) + a2*(-(b(2, 3)*c(1, 3)*c(2, 1)) + b(2, 3)*c(1, 1)*c(2, 3) + b(2, 2)*c(1, 3)*c(3, 1) - b(2, 1)*c(2, 3)*c(3, 1) - b(2, 2)*c(1, 1)*c(3, 3) + b(2, 1)*c(2, 1)*c(3, 3)) + a3*(-(b(3, 3)*c(1, 3)*c(2, 1)) + b(3, 3)*c(1, 1)*c(2, 3) + b(3, 2)*c(1, 3)*c(3, 1) - b(3, 1)*c(2, 3)*c(3, 1) - b(3, 2)*c(1, 1)*c(3, 3) + b(3, 1)*c(2, 1)*c(3, 3))) / (-(c(1, 3)*c(2, 2)*c(3, 1)) + c(1, 2)*c(2, 3)*c(3, 1) + c(1, 3)*c(2, 1)*c(3, 2) - c(1, 1)*c(2, 3)*c(3, 2) - c(1, 2)*c(2, 1)*c(3, 3) + c(1, 1)*c(2, 2)*c(3, 3));
			ptrDest[2] = (a1*(b(1, 3)*c(1, 2)*c(2, 1) - b(1, 3)*c(1, 1)*c(2, 2) - b(1, 2)*c(1, 2)*c(3, 1) + b(1, 1)*c(2, 2)*c(3, 1) + b(1, 2)*c(1, 1)*c(3, 2) - b(1, 1)*c(2, 1)*c(3, 2)) + a2*(b(2, 3)*c(1, 2)*c(2, 1) - b(2, 3)*c(1, 1)*c(2, 2) - b(2, 2)*c(1, 2)*c(3, 1) + b(2, 1)*c(2, 2)*c(3, 1) + b(2, 2)*c(1, 1)*c(3, 2) - b(2, 1)*c(2, 1)*c(3, 2)) + a3*(b(3, 3)*c(1, 2)*c(2, 1) - b(3, 3)*c(1, 1)*c(2, 2) - b(3, 2)*c(1, 2)*c(3, 1) + b(3, 1)*c(2, 2)*c(3, 1) + b(3, 2)*c(1, 1)*c(3, 2) - b(3, 1)*c(2, 1)*c(3, 2))) / (-(c(1, 3)*c(2, 2)*c(3, 1)) + c(1, 2)*c(2, 3)*c(3, 1) + c(1, 3)*c(2, 1)*c(3, 2) - c(1, 1)*c(2, 3)*c(3, 2) - c(1, 2)*c(2, 1)*c(3, 3) + c(1, 1)*c(2, 2)*c(3, 3));
#undef b
#undef c
		}

		static void CalcTestA(const double* pA, int strideA, const double* pB, int strideB, const double* pC, int strideC, double* pDest)
		{
			/* Mathematica:

			{{0, 0, -(1/2)}, {0, 1, 0}, {-(1/2), 0,
			0}}.({{a11, a12, a13}, {a21, a22, a23}, {a31, a32,
			a33}} - {{b11, b12, b13}, {b21, b22, b23}, {b31, b32,
			b33}}.(Inverse[{{c11, c12, c13}, {c21, c22, c23}, {c31, c32,
			c33}}].Transpose[{{b11, b12, b13}, {b21, b22, b23}, {b31,
			b32, b33}}])) // Simplify // CForm
			*/
#define a(r,c) *((double*)(((char*)pA)+(r-1)*strideA+(c-1)*sizeof(double)))
#define b(r,c) *((double*)(((char*)pB)+(r-1)*strideB+(c-1)*sizeof(double)))
#define c(r,c) *((double*)(((char*)pC)+(r-1)*strideC+(c-1)*sizeof(double)))
			pDest[0] = (b(1, 3)*(-(b(3, 3)*c(1, 2)*c(2, 1)) + b(3, 2)*c(1, 3)*c(2, 1) + b(3, 3)*c(1, 1)*c(2, 2) - b(3, 1)*c(1, 3)*c(2, 2) - b(3, 2)*c(1, 1)*c(2, 3) + b(3, 1)*c(1, 2)*c(2, 3)) - b(1, 1)*b(3, 3)*c(2, 2)*c(3, 1) + a(3, 1)*c(1, 3)*c(2, 2)*c(3, 1) + b(1, 1)*b(3, 2)*c(2, 3)*c(3, 1) - a(3, 1)*c(1, 2)*c(2, 3)*c(3, 1) + b(1, 1)*b(3, 3)*c(2, 1)*c(3, 2) - a(3, 1)*c(1, 3)*c(2, 1)*c(3, 2) - b(1, 1)*b(3, 1)*c(2, 3)*c(3, 2) + a(3, 1)*c(1, 1)*c(2, 3)*c(3, 2) - b(1, 1)*b(3, 2)*c(2, 1)*c(3, 3) + a(3, 1)*c(1, 2)*c(2, 1)*c(3, 3) + b(1, 1)*b(3, 1)*c(2, 2)*c(3, 3) - a(3, 1)*c(1, 1)*c(2, 2)*c(3, 3) +
				b(1, 2)*(b(3, 3)*c(1, 2)*c(3, 1) - b(3, 2)*c(1, 3)*c(3, 1) - b(3, 3)*c(1, 1)*c(3, 2) + b(3, 1)*c(1, 3)*c(3, 2) + b(3, 2)*c(1, 1)*c(3, 3) - b(3, 1)*c(1, 2)*c(3, 3))) / (2.*(-(c(1, 3)*c(2, 2)*c(3, 1)) + c(1, 2)*c(2, 3)*c(3, 1) + c(1, 3)*c(2, 1)*c(3, 2) - c(1, 1)*c(2, 3)*c(3, 2) - c(1, 2)*c(2, 1)*c(3, 3) + c(1, 1)*c(2, 2)*c(3, 3)));

			pDest[1] = (b(2, 3)*(-(b(3, 3)*c(1, 2)*c(2, 1)) + b(3, 2)*c(1, 3)*c(2, 1) + b(3, 3)*c(1, 1)*c(2, 2) - b(3, 1)*c(1, 3)*c(2, 2) - b(3, 2)*c(1, 1)*c(2, 3) + b(3, 1)*c(1, 2)*c(2, 3)) - b(2, 1)*b(3, 3)*c(2, 2)*c(3, 1) + a(3, 2)*c(1, 3)*c(2, 2)*c(3, 1) + b(2, 1)*b(3, 2)*c(2, 3)*c(3, 1) - a(3, 2)*c(1, 2)*c(2, 3)*c(3, 1) + b(2, 1)*b(3, 3)*c(2, 1)*c(3, 2) - a(3, 2)*c(1, 3)*c(2, 1)*c(3, 2) - b(2, 1)*b(3, 1)*c(2, 3)*c(3, 2) + a(3, 2)*c(1, 1)*c(2, 3)*c(3, 2) - b(2, 1)*b(3, 2)*c(2, 1)*c(3, 3) + a(3, 2)*c(1, 2)*c(2, 1)*c(3, 3) + b(2, 1)*b(3, 1)*c(2, 2)*c(3, 3) - a(3, 2)*c(1, 1)*c(2, 2)*c(3, 3) +
				b(2, 2)*(b(3, 3)*c(1, 2)*c(3, 1) - b(3, 2)*c(1, 3)*c(3, 1) - b(3, 3)*c(1, 1)*c(3, 2) + b(3, 1)*c(1, 3)*c(3, 2) + b(3, 2)*c(1, 1)*c(3, 3) - b(3, 1)*c(1, 2)*c(3, 3))) / (2.*(-(c(1, 3)*c(2, 2)*c(3, 1)) + c(1, 2)*c(2, 3)*c(3, 1) + c(1, 3)*c(2, 1)*c(3, 2) - c(1, 1)*c(2, 3)*c(3, 2) - c(1, 2)*c(2, 1)*c(3, 3) + c(1, 1)*c(2, 2)*c(3, 3)));

			pDest[2] = (squared(b(3, 3))*(-(c(1, 2)*c(2, 1)) + c(1, 1)*c(2, 2)) + a(3, 3)*c(1, 3)*c(2, 2)*c(3, 1) - a(3, 3)*c(1, 2)*c(2, 3)*c(3, 1) - a(3, 3)*c(1, 3)*c(2, 1)*c(3, 2) - squared(b(3, 1))*c(2, 3)*c(3, 2) + a(3, 3)*c(1, 1)*c(2, 3)*c(3, 2) + b(3, 3)*(b(3, 1)*(-(c(1, 3)*c(2, 2)) + c(1, 2)*c(2, 3) - c(2, 2)*c(3, 1) + c(2, 1)*c(3, 2)) + b(3, 2)*(c(1, 3)*c(2, 1) + c(1, 2)*c(3, 1) - c(1, 1)*(c(2, 3) + c(3, 2)))) + a(3, 3)*c(1, 2)*c(2, 1)*c(3, 3) + squared(b(3, 1))*c(2, 2)*c(3, 3) - a(3, 3)*c(1, 1)*c(2, 2)*c(3, 3) + squared(b(3, 2))*(-(c(1, 3)*c(3, 1)) + c(1, 1)*c(3, 3)) +
				b(3, 1)*b(3, 2)*(c(2, 3)*c(3, 1) + c(1, 3)*c(3, 2) - (c(1, 2) + c(2, 1))*c(3, 3))) / (2 * (-(c(1, 3)*c(2, 2)*c(3, 1)) + c(1, 2)*c(2, 3)*c(3, 1) + c(1, 3)*c(2, 1)*c(3, 2) - c(1, 1)*c(2, 3)*c(3, 2) - c(1, 2)*c(2, 1)*c(3, 3) + c(1, 1)*c(2, 2)*c(3, 3)));

			pDest[3] = (b(1, 3)*(b(2, 3)*c(1, 2)*c(2, 1) - b(2, 2)*c(1, 3)*c(2, 1) - b(2, 3)*c(1, 1)*c(2, 2) + b(2, 1)*c(1, 3)*c(2, 2) + b(2, 2)*c(1, 1)*c(2, 3) - b(2, 1)*c(1, 2)*c(2, 3)) + b(1, 1)*b(2, 3)*c(2, 2)*c(3, 1) - a(2, 1)*c(1, 3)*c(2, 2)*c(3, 1) - b(1, 1)*b(2, 2)*c(2, 3)*c(3, 1) + a(2, 1)*c(1, 2)*c(2, 3)*c(3, 1) - b(1, 1)*b(2, 3)*c(2, 1)*c(3, 2) + a(2, 1)*c(1, 3)*c(2, 1)*c(3, 2) + b(1, 1)*b(2, 1)*c(2, 3)*c(3, 2) - a(2, 1)*c(1, 1)*c(2, 3)*c(3, 2) + b(1, 1)*b(2, 2)*c(2, 1)*c(3, 3) - a(2, 1)*c(1, 2)*c(2, 1)*c(3, 3) - b(1, 1)*b(2, 1)*c(2, 2)*c(3, 3) + a(2, 1)*c(1, 1)*c(2, 2)*c(3, 3) +
				b(1, 2)*(-(b(2, 3)*c(1, 2)*c(3, 1)) + b(2, 2)*c(1, 3)*c(3, 1) + b(2, 3)*c(1, 1)*c(3, 2) - b(2, 1)*c(1, 3)*c(3, 2) - b(2, 2)*c(1, 1)*c(3, 3) + b(2, 1)*c(1, 2)*c(3, 3))) / (-(c(1, 3)*c(2, 2)*c(3, 1)) + c(1, 2)*c(2, 3)*c(3, 1) + c(1, 3)*c(2, 1)*c(3, 2) - c(1, 1)*c(2, 3)*c(3, 2) - c(1, 2)*c(2, 1)*c(3, 3) + c(1, 1)*c(2, 2)*c(3, 3));

			pDest[4] = (squared(b(2, 3))*(-(c(1, 2)*c(2, 1)) + c(1, 1)*c(2, 2)) + a(2, 2)*c(1, 3)*c(2, 2)*c(3, 1) - a(2, 2)*c(1, 2)*c(2, 3)*c(3, 1) - a(2, 2)*c(1, 3)*c(2, 1)*c(3, 2) - squared(b(2, 1))*c(2, 3)*c(3, 2) + a(2, 2)*c(1, 1)*c(2, 3)*c(3, 2) + b(2, 3)*(b(2, 1)*(-(c(1, 3)*c(2, 2)) + c(1, 2)*c(2, 3) - c(2, 2)*c(3, 1) + c(2, 1)*c(3, 2)) + b(2, 2)*(c(1, 3)*c(2, 1) + c(1, 2)*c(3, 1) - c(1, 1)*(c(2, 3) + c(3, 2)))) + a(2, 2)*c(1, 2)*c(2, 1)*c(3, 3) + squared(b(2, 1))*c(2, 2)*c(3, 3) - a(2, 2)*c(1, 1)*c(2, 2)*c(3, 3) + squared(b(2, 2))*(-(c(1, 3)*c(3, 1)) + c(1, 1)*c(3, 3)) +
				b(2, 1)*b(2, 2)*(c(2, 3)*c(3, 1) + c(1, 3)*c(3, 2) - (c(1, 2) + c(2, 1))*c(3, 3))) / (c(1, 3)*c(2, 2)*c(3, 1) - c(1, 2)*c(2, 3)*c(3, 1) - c(1, 3)*c(2, 1)*c(3, 2) + c(1, 1)*c(2, 3)*c(3, 2) + c(1, 2)*c(2, 1)*c(3, 3) - c(1, 1)*c(2, 2)*c(3, 3));

			pDest[5] = (b(2, 1)*b(3, 3)*c(1, 3)*c(2, 2) - b(2, 1)*b(3, 3)*c(1, 2)*c(2, 3) - a(2, 3)*c(1, 3)*c(2, 2)*c(3, 1) + a(2, 3)*c(1, 2)*c(2, 3)*c(3, 1) - b(2, 1)*b(3, 2)*c(1, 3)*c(3, 2) + a(2, 3)*c(1, 3)*c(2, 1)*c(3, 2) + b(2, 1)*b(3, 1)*c(2, 3)*c(3, 2) - a(2, 3)*c(1, 1)*c(2, 3)*c(3, 2) +
				b(2, 3)*(b(3, 3)*c(1, 2)*c(2, 1) - b(3, 3)*c(1, 1)*c(2, 2) - b(3, 2)*c(1, 2)*c(3, 1) + b(3, 1)*c(2, 2)*c(3, 1) + b(3, 2)*c(1, 1)*c(3, 2) - b(3, 1)*c(2, 1)*c(3, 2)) + b(2, 1)*b(3, 2)*c(1, 2)*c(3, 3) - a(2, 3)*c(1, 2)*c(2, 1)*c(3, 3) - b(2, 1)*b(3, 1)*c(2, 2)*c(3, 3) + a(2, 3)*c(1, 1)*c(2, 2)*c(3, 3) + b(2, 2)*(-(b(3, 3)*c(1, 3)*c(2, 1)) + b(3, 3)*c(1, 1)*c(2, 3) + b(3, 2)*c(1, 3)*c(3, 1) - b(3, 1)*c(2, 3)*c(3, 1) - b(3, 2)*c(1, 1)*c(3, 3) + b(3, 1)*c(2, 1)*c(3, 3))) / (-(c(1, 3)*c(2, 2)*c(3, 1)) + c(1, 2)*c(2, 3)*c(3, 1) + c(1, 3)*c(2, 1)*c(3, 2) - c(1, 1)*c(2, 3)*c(3, 2) - c(1, 2)*c(2, 1)*c(3, 3) + c(1, 1)*c(2, 2)*c(3, 3));

			pDest[6] = (squared(b(1, 3))*(-(c(1, 2)*c(2, 1)) + c(1, 1)*c(2, 2)) + a(1, 1)*c(1, 3)*c(2, 2)*c(3, 1) - a(1, 1)*c(1, 2)*c(2, 3)*c(3, 1) - a(1, 1)*c(1, 3)*c(2, 1)*c(3, 2) - squared(b(1, 1))*c(2, 3)*c(3, 2) + a(1, 1)*c(1, 1)*c(2, 3)*c(3, 2) + b(1, 3)*(b(1, 1)*(-(c(1, 3)*c(2, 2)) + c(1, 2)*c(2, 3) - c(2, 2)*c(3, 1) + c(2, 1)*c(3, 2)) + b(1, 2)*(c(1, 3)*c(2, 1) + c(1, 2)*c(3, 1) - c(1, 1)*(c(2, 3) + c(3, 2)))) + a(1, 1)*c(1, 2)*c(2, 1)*c(3, 3) + squared(b(1, 1))*c(2, 2)*c(3, 3) - a(1, 1)*c(1, 1)*c(2, 2)*c(3, 3) + squared(b(1, 2))*(-(c(1, 3)*c(3, 1)) + c(1, 1)*c(3, 3)) +
				b(1, 1)*b(1, 2)*(c(2, 3)*c(3, 1) + c(1, 3)*c(3, 2) - (c(1, 2) + c(2, 1))*c(3, 3))) / (2 * (-(c(1, 3)*c(2, 2)*c(3, 1)) + c(1, 2)*c(2, 3)*c(3, 1) + c(1, 3)*c(2, 1)*c(3, 2) - c(1, 1)*c(2, 3)*c(3, 2) - c(1, 2)*c(2, 1)*c(3, 3) + c(1, 1)*c(2, 2)*c(3, 3)));

			pDest[7] = (-(b(1, 1)*b(2, 3)*c(1, 3)*c(2, 2)) + b(1, 1)*b(2, 3)*c(1, 2)*c(2, 3) + a(1, 2)*c(1, 3)*c(2, 2)*c(3, 1) - a(1, 2)*c(1, 2)*c(2, 3)*c(3, 1) + b(1, 1)*b(2, 2)*c(1, 3)*c(3, 2) - a(1, 2)*c(1, 3)*c(2, 1)*c(3, 2) - b(1, 1)*b(2, 1)*c(2, 3)*c(3, 2) + a(1, 2)*c(1, 1)*c(2, 3)*c(3, 2) +
				b(1, 3)*(-(b(2, 3)*c(1, 2)*c(2, 1)) + b(2, 3)*c(1, 1)*c(2, 2) + b(2, 2)*c(1, 2)*c(3, 1) - b(2, 1)*c(2, 2)*c(3, 1) - b(2, 2)*c(1, 1)*c(3, 2) + b(2, 1)*c(2, 1)*c(3, 2)) - b(1, 1)*b(2, 2)*c(1, 2)*c(3, 3) + a(1, 2)*c(1, 2)*c(2, 1)*c(3, 3) + b(1, 1)*b(2, 1)*c(2, 2)*c(3, 3) - a(1, 2)*c(1, 1)*c(2, 2)*c(3, 3) + b(1, 2)*(b(2, 3)*c(1, 3)*c(2, 1) - b(2, 3)*c(1, 1)*c(2, 3) - b(2, 2)*c(1, 3)*c(3, 1) + b(2, 1)*c(2, 3)*c(3, 1) + b(2, 2)*c(1, 1)*c(3, 3) - b(2, 1)*c(2, 1)*c(3, 3))) / (2.*(-(c(1, 3)*c(2, 2)*c(3, 1)) + c(1, 2)*c(2, 3)*c(3, 1) + c(1, 3)*c(2, 1)*c(3, 2) - c(1, 1)*c(2, 3)*c(3, 2) - c(1, 2)*c(2, 1)*c(3, 3) + c(1, 1)*c(2, 2)*c(3, 3)));

			pDest[8] = (-(b(1, 1)*b(3, 3)*c(1, 3)*c(2, 2)) + b(1, 1)*b(3, 3)*c(1, 2)*c(2, 3) + a(1, 3)*c(1, 3)*c(2, 2)*c(3, 1) - a(1, 3)*c(1, 2)*c(2, 3)*c(3, 1) + b(1, 1)*b(3, 2)*c(1, 3)*c(3, 2) - a(1, 3)*c(1, 3)*c(2, 1)*c(3, 2) - b(1, 1)*b(3, 1)*c(2, 3)*c(3, 2) + a(1, 3)*c(1, 1)*c(2, 3)*c(3, 2) + b(1, 3)*(-(b(3, 3)*c(1, 2)*c(2, 1)) + b(3, 3)*c(1, 1)*c(2, 2) + b(3, 2)*c(1, 2)*c(3, 1) - b(3, 1)*c(2, 2)*c(3, 1) - b(3, 2)*c(1, 1)*c(3, 2) + b(3, 1)*c(2, 1)*c(3, 2)) - b(1, 1)*b(3, 2)*c(1, 2)*c(3, 3) + a(1, 3)*c(1, 2)*c(2, 1)*c(3, 3) + b(1, 1)*b(3, 1)*c(2, 2)*c(3, 3) - a(1, 3)*c(1, 1)*c(2, 2)*c(3, 3) +
				b(1, 2)*(b(3, 3)*c(1, 3)*c(2, 1) - b(3, 3)*c(1, 1)*c(2, 3) - b(3, 2)*c(1, 3)*c(3, 1) + b(3, 1)*c(2, 3)*c(3, 1) + b(3, 2)*c(1, 1)*c(3, 3) - b(3, 1)*c(2, 1)*c(3, 3))) / (2.*(-(c(1, 3)*c(2, 2)*c(3, 1)) + c(1, 2)*c(2, 3)*c(3, 1) + c(1, 3)*c(2, 1)*c(3, 2) - c(1, 1)*c(2, 3)*c(3, 2) - c(1, 2)*c(2, 1)*c(3, 3) + c(1, 1)*c(2, 2)*c(3, 3)));

#undef c
#undef b
#undef a
		}
	};
}