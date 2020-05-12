#include "propagation.h"

#include <cmath>
#include <omp.h>
#include <ctime>
#include <iostream>
#include "parallelProcess.h"

cv::Mat propagation(const cv::Mat & in, const double lambda, const double z, const double pp, const int zero_pad)
{
	cv::Mat A, croppedin;
	int L, C;
	L = std::min(in.rows, 1024);
	C = std::min(in.cols, 1024);
	cv::Rect croppedrect((in.cols - C) / 2, (in.rows - L) / 2, C, L);

	croppedin = in(croppedrect);

	double zNS = L * (pp*pp) / lambda;

	if (zero_pad == 0) {
		cv::Mat newin;
		cv::Mat HZ, fft, a;
		croppedin.convertTo(newin, CV_64F);
		newin /= 255;

		if (abs(z) >= zNS) {
			auto hz = fresnelFunction(L, C, z, lambda, pp);
			cv::dft(hz, HZ, cv::DFT_COMPLEX_OUTPUT);
			HZ = HZ / findMaxComplex(HZ);
		}
		else {
			HZ = fresnelFunctionFT(L, C, z, lambda, pp);
		}

		cv::dft(newin, fft, cv::DFT_COMPLEX_OUTPUT);
		a = complexMultiply(HZ, fft);
		cv::dft(a, A, cv::DFT_INVERSE | cv::DFT_SCALE | cv::DFT_COMPLEX_OUTPUT);
	}
	else {
		cv::Mat newin;
		cv::Rect center(C - floor(C / 2.), L - floor(L / 2.), C, L);
		newin = cv::Mat::ones(2 * L, 2 * C, CV_64F) * cv::mean(croppedin).val[0] / 255.;

		for (int i = 0; i < L; i++) {
			for (int j = 0; j < C; j++) {
				newin.at<double>(i + L - floor(L / 2.), j + C - floor(C / 2.)) = croppedin.at<unsigned char>(i, j) / 255.;
			}
		}
		
		cv::Mat HZ, fft, a;
		if (abs(z) >= zNS) {
			auto hz = fresnelFunction(2 * L, 2 * C, z, lambda, pp);
			cv::dft(hz, HZ, cv::DFT_COMPLEX_OUTPUT);
			HZ = HZ / findMaxComplex(HZ);
		}
		else {
			HZ = fresnelFunctionFT(2 * L, 2 * C, z, lambda, pp);
		}

		cv::dft(newin, fft, cv::DFT_COMPLEX_OUTPUT);
		a = complexMultiply(HZ, fft);
		cv::dft(a, A, cv::DFT_INVERSE | cv::DFT_SCALE | cv::DFT_COMPLEX_OUTPUT);

		if (zero_pad == 1) A = A(center);
	}

	cv::Mat mats[2];
	cv::split(A, mats);
	cv::magnitude(mats[0], mats[1], A);

	cv::multiply(A, A, A);

	return A;
}

cv::Mat complexMultiply(const cv::Mat &A, const cv::Mat &B) {
	cv::Mat planeA[2], planeB[2], planeC[2];
	cv::Mat C = cv::Mat::zeros(A.rows, A.cols, CV_64FC2);

	cv::split(A, planeA);
	cv::split(B, planeB);


	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			C.at<cv::Vec2d>(i, j)[0] = planeA[0].at<double>(i, j)*planeB[0].at<double>(i, j) - planeA[1].at<double>(i, j)*planeB[1].at<double>(i, j);
			C.at<cv::Vec2d>(i, j)[1] = planeA[0].at<double>(i, j)*planeB[1].at<double>(i, j) + planeA[1].at<double>(i, j)*planeB[0].at<double>(i, j);
		}
	}

	return C;
}

double findMaxComplex(const cv::Mat & in)
{
	cv::Mat planes[2];
	cv::split(in, planes);

	double minValue, maxValue;
	cv::minMaxIdx(planes[0], &minValue, &maxValue);

	return maxValue;
}



cv::Mat fresnelFunction(int lignes, int colones, double z, double lambda, double pp)
{
	cv::Mat hz;

	hz = cv::Mat::zeros(lignes, colones, CV_64FC2);

	std::complex<double> c1 = 1 / lambda / z / I;
	std::complex<double> c2 = I * PI * (pp*pp) / lambda / z;

	FresnelFunctionParallel ffp(hz, c1, c2, z, lambda);
	parallel_for_(cv::Range(0, lignes*colones), ffp);

	// rearrange the quadrants of Fourier image  so that the origin is at the image center
	int cx = hz.cols / 2;
	int cy = hz.rows / 2;

	cv::Mat q0(hz, cv::Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
	cv::Mat q1(hz, cv::Rect(cx, 0, cx, cy));  // Top-Right
	cv::Mat q2(hz, cv::Rect(0, cy, cx, cy));  // Bottom-Left
	cv::Mat q3(hz, cv::Rect(cx, cy, cx, cy)); // Bottom-Right

	cv::Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
	q2.copyTo(q1);
	tmp.copyTo(q2);

	return hz;
}

cv::Mat fresnelFunctionFT(int lignes, int colones, double z, double lambda, double pp)
{
	cv::Mat hz;

	hz = cv::Mat::zeros(lignes, colones, CV_64FC2);

	FresnelFunctionParallelFT ffp(hz, z, lambda, pp);
	parallel_for_(cv::Range(0, lignes*colones), ffp);

	// rearrange the quadrants of Fourier image  so that the origin is at the image center
	int cx = hz.cols / 2;
	int cy = hz.rows / 2;

	cv::Mat q0(hz, cv::Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
	cv::Mat q1(hz, cv::Rect(cx, 0, cx, cy));  // Top-Right
	cv::Mat q2(hz, cv::Rect(0, cy, cx, cy));  // Bottom-Left
	cv::Mat q3(hz, cv::Rect(cx, cy, cx, cy)); // Bottom-Right

	cv::Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
	q2.copyTo(q1);
	tmp.copyTo(q2);

	return hz;
}

