#ifndef PROPAGATION_H
#define PROPAGATION_H

#include <opencv2/core.hpp>
#include "constantes.h"

cv::Mat propagation(const cv::Mat &in, const double lambda, const double z, const double pp, const int zero_pad = 0);
double findMaxComplex(const cv::Mat &in);
cv::Mat complexMultiply(const cv::Mat &A, const cv::Mat &B);
cv::Mat fresnelFunction(int lignes, int colones, double z, double lambda, double pp);
cv::Mat fresnelFunctionFT(int lignes, int colones, double z, double lambda, double pp);


#endif