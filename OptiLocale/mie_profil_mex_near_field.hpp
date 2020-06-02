//Standard Librairies
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <boost/math/special_functions/legendre.hpp>
#include <Eigen/Dense>

double atan2_d(double y, double x);
double sin_d(double x);
double cos_d(double x);
std::complex<double> exp_d(std::complex<double> x);
int n_max(double alpha);
Eigen::MatrixXd cart_to_spher(Eigen::MatrixXd x, Eigen::MatrixXd y, Eigen::MatrixXd z, double xe, double ye);
Eigen::MatrixXcd psi_xi(unsigned int nmax, std::complex<double> z);
Eigen::MatrixXcd psi_xi_debug(unsigned int nmax, std::complex<double> z);
Eigen::MatrixXcd an_bn(unsigned int nmax, std::complex<double> alpha, std::complex<double> M);
Eigen::MatrixXcd an_bn_debug(unsigned int nmax, std::complex<double> alpha, std::complex<double> M);
Eigen::MatrixXcd c_w(int n);
Eigen::MatrixXcd cw_mip1_2np1(int nmax);
Eigen::MatrixXcd mip1_only(int nmax);
Eigen::MatrixXd legendre_vector(int n_max, Eigen::MatrixXd x);
Eigen::MatrixXd legendre_vector_debug(int n_max, Eigen::MatrixXd x);
Eigen::MatrixXd derivative_legendre_vector(int nmax, Eigen::MatrixXd x, Eigen::MatrixXd leg_vec);
Eigen::MatrixXd derivative_legendre_vector_debug(int n_max, Eigen::MatrixXd x, Eigen::MatrixXd leg_vec);
Eigen::MatrixXcd compute_psi_xi_kr(int n_max, Eigen::MatrixXd kr);
Eigen::MatrixXcd compute_psi_xi_kr_debug(int n_max, Eigen::MatrixXd kr);
Eigen::MatrixXd Mie(double a, double k, Eigen::MatrixXd x, Eigen::MatrixXd y, Eigen::MatrixXd z, double xe, double ye, std::complex<double> M);
Eigen::MatrixXcd sph_bessel_vector(int nmax, std::complex<double> z);
Eigen::MatrixXcd sph_bessel_vector_debug(int nmax, std::complex<double> z);