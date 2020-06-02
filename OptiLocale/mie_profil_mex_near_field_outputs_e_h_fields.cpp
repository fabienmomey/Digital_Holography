/*******************************************************************************************/
/*                                                                                         */
/*                         MIE MODEL (NEAR FIELD FOR PLANAR WAVE)                          */
/* Based on: Loic Méès fortran90 code & articles                                           */
/* Last updates:                                                                           */
/*                 --> Aug.2016 (of) --> C++ Code creation                                 */
/*                 --> Aug.2016 (of) --> acceleration (vectorial Bessel)                   */
/*                 --> Nov.2016 (of) --> bug correction (shift + refractive index)         */ 
/*				   --> Dec.2016 (of) --> real / imag. parts of fields		               */
/*                 --> Nov.2017 (of) --> remove verbose mode (for Corinne)                 */
/*                 --> Aug.2018 (of) --> near field + openmp + backward recurrence         */
/*                                                                                         */                                                                                                  
/*******************************************************************************************/

//Standard Librairies
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <iostream>
#include <functional>
#include <complex>

//Headers
#include "mie_profil_mex_near_field.hpp"


//External Libraries
#include <boost/math/special_functions/legendre.hpp>
#include <Eigen/Dense>

//Namespace declarations
namespace bm = boost::math;
using namespace Eigen;
using namespace std;

#undef I
#define pi 3.141592653589793238462643383279502884197169399375
//#define eps 0.00000001
#define eps 0.00001

#define EPSILON  1e-6
#define EPSILON2  1e-16

/************************************ model calculation ************************************/

double atan2_d(double y, double x)
{
	return atan2(y, x);
}

double sin_d(double x)
{
	double sin_x = sin(x);
	if (sin_x <= eps)
		return eps;
	else
		return sin_x;
}

double cos_d(double x)
{
	double cos_x = cos(x);
	if (cos_x >= sqrt(1. - eps*eps))
		return (double)sqrt((double)1. - eps*eps);
	else
		return cos_x;
}

std::complex<double> exp_d(std::complex<double> x)
{
	return exp(x);
}

int n_max(double alpha)
{
	return floor(alpha + 4.0*pow(alpha, (double)1 / 3) + 2.);
}

Eigen::MatrixXcd c_w(int nmax)
{
	Eigen::MatrixXcd result(nmax, 1);
	result.setConstant(0);
    complex<double> i_complex(0.0,1.0);
    #pragma omp parallel for
	for (int i = 1; i <= nmax; ++i)
		result(i-1, 0) = std::pow(-i_complex, i+1.) * (2. * i + 1.) / (i * (i + 1.));

	return result;
}

Eigen::MatrixXcd mip1_only(int nmax)
{
	Eigen::MatrixXcd result(nmax, 1);
	result.setConstant(0);
    complex<double> i_complex(0.0,1.0);
    #pragma omp parallel for
	for (int i = 1; i <= nmax; ++i)    
        result(i-1, 0) = std::pow(-i_complex, i+1);
    
    //result(0,0) = 0; // useless since we keep values given for n=1
    
    return result;
}

Eigen::MatrixXcd cw_mip1_2np1(int nmax)
{
	Eigen::MatrixXcd result(nmax, 3); result.setZero();
    complex<double> i_complex(0.0,1.0);
    #pragma omp parallel for
	for (int i = 1; i <= nmax; ++i)
    {
		result(i-1, 0) = std::pow(-i_complex, i+1) * (2. * i + 1) / (i * (i + 1.));
        result(i-1, 1) = std::pow(-i_complex, i+1);
        result(i-1, 2) = (2 * i + 1);
    }
    
    //result(0,0) = 0; result(0,1) = 0; result(0,2) = 0; // useless since we keep values given for n=1

    return result;
}

Eigen::MatrixXd cart_to_spher(Eigen::MatrixXd x, Eigen::MatrixXd y, Eigen::MatrixXd z, double xe, double ye)
{    
	Eigen::MatrixXd x_xe = (x.array() + xe).matrix();
	Eigen::MatrixXd y_ye = (y.array() + ye).matrix();

	Eigen::MatrixXd r02 = x_xe.cwiseProduct(x_xe) + y_ye.cwiseProduct(y_ye);

	Eigen::MatrixXd r = (r02 + z.cwiseProduct(z)).cwiseSqrt();
	Eigen::MatrixXd t = (((r02.cwiseSqrt()))).binaryExpr(z, std::ptr_fun(atan2_d));
	Eigen::MatrixXd phi = pi * (y_ye.binaryExpr(x_xe, std::ptr_fun(atan2_d)));

	Eigen::MatrixXd cyl(r.rows() + t.rows() + phi.rows(), x.cols());
	cyl << r,
		t,
		phi;

	return cyl;
}

Eigen::MatrixXcd sph_bessel_vector(int nmax, std::complex<double> z)
{
	int n_born = floor(1.0 * nmax);

	Eigen::MatrixXcd bessel_vector(3, n_born + 1); //1st line : bessel_sph_1 (j), 2nd line : bessel_sph_2 (y), 3rd line : hankel_sph_2 (h)

    //***** compute j_n(z) *****//
    if(nmax > abs(z))
    {
        //Initialization
        bessel_vector(0, n_born) = (double)eps; bessel_vector(0, n_born - 1) = (double)1;
        //Backward loop
        //printf("compute j_n: backward loop \n");
        for (int k = (n_born - 2); k >= 0; --k)
            bessel_vector(0, k) = ((std::complex<double>(2 * k + 3)) / z)*bessel_vector(0, k + 1) - bessel_vector(0, k + 2);

        std::complex<double> factor_bessel_sph_1 = (sin(z) / z) / bessel_vector(0, 0);
        bessel_vector.block(0, 0, 1, bessel_vector.cols()) = bessel_vector.block(0, 0, 1, bessel_vector.cols()) * factor_bessel_sph_1;
    } 
    else 
    {
        //Initialization
        bessel_vector(0, 0) = sin(z)/z; bessel_vector(0, 1) = (sin(z)/(z*z)) - cos(z)/z;
        //Forward loop
        //printf("compute j_n: forward loop \n");
        for (int k = 2; k <= n_born; ++k)
            bessel_vector(0, k) = ((2.*(double)k - 1.0)/z)*bessel_vector(0, k - 1) - bessel_vector(0, k - 2);
    }

    //***** compute y_n(z) *****//
    //Initialization
    bessel_vector(1, 0) = -cos(z) / z; bessel_vector(1, 1) = (-cos(z) / (z*z)) - sin(z) / z;
	//Forward loop
	for (int k = 2; k <= n_born; ++k)
		bessel_vector(1, k) = ((std::complex<double>(2 * k - 1)) / z)*bessel_vector(1, k - 1) - bessel_vector(1, k - 2);

    //***** compute h_n(z) *****//
	bessel_vector.block(2, 0, 1, bessel_vector.cols()) = bessel_vector.block(0, 0, 1, bessel_vector.cols()) - 1i*bessel_vector.block(1, 0, 1, bessel_vector.cols());

	return bessel_vector;
}

Eigen::MatrixXcd sph_bessel_vector_debug(int nmax, std::complex<double> z)
{
	int n_born = floor(1.0 * nmax);

	Eigen::MatrixXcd bessel_vector(n_born + 1, 3); //1st column : bessel_sph_1 (j), 2nd column : bessel_sph_2 (y), 3rd column : hankel_sph_2 (h)

    //***** compute j_n(z) *****//
    //***** compute y_n(z) *****//
    if(nmax > abs(z))
    {
        //***** compute j_n(z) *****//
        //Initialization
        bessel_vector(n_born, 0) = (double)eps; bessel_vector(n_born - 1, 0) = (double)1;
        //Backward loop
        //printf("compute j_n: backward loop \n");
        for (int k = (n_born - 2); k >= 0; --k)
            bessel_vector(k, 0) = ((std::complex<double>(2 * k + 3)) / z)*bessel_vector(k + 1, 0) - bessel_vector(k + 2, 0);

        std::complex<double> factor_bessel_sph_1 = (sin(z) / z) / bessel_vector(0, 0);
        bessel_vector.block(0, 0, bessel_vector.rows(), 1) = bessel_vector.block(0, 0, bessel_vector.rows(), 1) * factor_bessel_sph_1;
        
        //***** compute y_n(z) *****//
        //Initialization
        bessel_vector(0, 1) = -cos(z) / z; bessel_vector(1, 1) = (-cos(z) / (z*z)) - sin(z) / z;
        //Forward loop
        for (int k = 2; k <= n_born; ++k)
            bessel_vector(k, 1) = ((std::complex<double>(2 * k - 1)) / z)*bessel_vector(k - 1, 1) - bessel_vector(k - 2, 1);
    } 
    else 
    {
        //Initialization
        bessel_vector(0, 0) = sin(z)/z; bessel_vector(1, 0) = (sin(z)/(z*z)) - cos(z)/z;
        bessel_vector(0, 1) = -cos(z) / z; bessel_vector(1, 1) = (-cos(z) / (z*z)) - sin(z) / z;
        //Forward loop
        //printf("compute j_n: forward loop \n");
        for (int k = 2; k <= n_born; ++k)
        {
            bessel_vector(k, 0) = ((2.*(double)k - 1.0)/z)*bessel_vector(k - 1, 0) - bessel_vector(k - 2, 0);
            bessel_vector(k, 1) = ((std::complex<double>(2 * k - 1)) / z)*bessel_vector(k - 1, 1) - bessel_vector(k - 2, 1);
        }
    }

    //***** compute h_n(z) *****//
	bessel_vector.block(0, 2, bessel_vector.rows(), 1) = bessel_vector.block(0, 0, bessel_vector.rows(), 1) - 1i*bessel_vector.block(0, 1, bessel_vector.rows(), 1);

	return bessel_vector;
}

Eigen::MatrixXcd psi_xi(unsigned int nmax, std::complex<double> z)
{
	Eigen::MatrixXcd psi_n(1, nmax + 1); psi_n.setZero();
	Eigen::MatrixXcd psi_n_d(1, nmax + 1); psi_n_d.setZero();

	Eigen::MatrixXcd xi_n(1, nmax + 1); xi_n.setZero();
	Eigen::MatrixXcd xi_n_d(1, nmax + 1); xi_n_d.setZero();

	Eigen::MatrixXcd bessel = sph_bessel_vector(nmax + 1, z);
	psi_n = z * bessel.block(0, 1, 1, nmax+1);
    
	xi_n = z * bessel.block(2, 1, 1, nmax+1);
    
	for (int k = 0; k < psi_n.cols() - 1; ++k)
	{
		psi_n_d(0, k) = ((std::complex<double>(k+2)) / z) * psi_n(0, k) - psi_n(0, k + 1);
		xi_n_d(0, k) = ((std::complex<double>(k+2)) / z) * xi_n(0, k) - xi_n(0, k + 1);
	}

	Eigen::MatrixXcd result(psi_n.rows() + psi_n_d.rows() + xi_n.rows() + xi_n_d.rows(), psi_n.cols());
	result << psi_n,
		psi_n_d,
		xi_n,
		xi_n_d;
    
	return result;
}

Eigen::MatrixXcd psi_xi_debug(unsigned int nmax, std::complex<double> z)
{
	Eigen::MatrixXcd psi_n(nmax + 1, 1); psi_n.setZero();
	Eigen::MatrixXcd psi_n_d(nmax + 1, 1); psi_n_d.setZero();

	Eigen::MatrixXcd xi_n(nmax + 1, 1); xi_n.setZero();
	Eigen::MatrixXcd xi_n_d(nmax + 1, 1); xi_n_d.setZero();

	Eigen::MatrixXcd bessel = sph_bessel_vector_debug(nmax + 1, z);
	psi_n = z * bessel.block(1, 0, nmax+1, 1);
    
	xi_n = z * bessel.block(1, 2, nmax+1, 1);

    #pragma omp parallel for
	for (int k = 0; k < psi_n.rows() - 1; ++k)
	{
		psi_n_d(k, 0) = ((std::complex<double>(k+2)) / z) * psi_n(k, 0) - psi_n(k + 1, 0);
		xi_n_d(k, 0) = ((std::complex<double>(k+2)) / z) * xi_n(k, 0) - xi_n(k + 1, 0);
	}

	Eigen::MatrixXcd result(psi_n.rows() + psi_n_d.rows() + xi_n.rows() + xi_n_d.rows(), psi_n.cols());
	result << psi_n,
		psi_n_d,
		xi_n,
		xi_n_d;
    
	return result;
}

Eigen::MatrixXcd an_bn(unsigned int nmax, std::complex<double> alpha, std::complex<double> M)
{
	std::complex<double> beta = alpha * M;

	Eigen::MatrixXcd psi_xi_alpha = psi_xi(nmax + 1, alpha).block(0, 0, 4, nmax);
    Eigen::MatrixXcd psi_xi_beta = psi_xi(nmax + 1, beta).block(0, 0, 4, nmax);
    
	Eigen::MatrixXcd num1 = psi_xi_alpha.block(0, 0, 1, psi_xi_alpha.cols()).cwiseProduct(psi_xi_beta.block(1, 0, 1, psi_xi_beta.cols()));
    Eigen::MatrixXcd num2 = psi_xi_alpha.block(1, 0, 1, psi_xi_alpha.cols()).cwiseProduct(psi_xi_beta.block(0, 0, 1, psi_xi_beta.cols()));
	Eigen::MatrixXcd den1 = psi_xi_alpha.block(2, 0, 1, psi_xi_alpha.cols()).cwiseProduct(psi_xi_beta.block(1, 0, 1, psi_xi_beta.cols()));
	Eigen::MatrixXcd den2 = psi_xi_alpha.block(3, 0, 1, psi_xi_alpha.cols()).cwiseProduct(psi_xi_beta.block(0, 0, 1, psi_xi_beta.cols()));

	Eigen::MatrixXcd an = (num1 - M*num2).cwiseQuotient(den1 - M*den2);
	Eigen::MatrixXcd bn = (M*num1 - num2).cwiseQuotient(M*den1 - den2);

	Eigen::MatrixXcd an_bn(an.rows() + bn.rows(), an.cols());
	an_bn << an,
		bn;
    
	return an_bn;
}

Eigen::MatrixXcd an_bn_debug(unsigned int nmax, std::complex<double> alpha, std::complex<double> M)
{
	std::complex<double> beta = alpha * M;
    Eigen::MatrixXcd psi_xi_alpha;
    Eigen::MatrixXcd psi_xi_beta;
    #pragma omp parallel sections
    {
        psi_xi_alpha = psi_xi_debug(nmax + 1, alpha);
        #pragma omp section  
        psi_xi_beta = psi_xi_debug(nmax + 1, beta);
    }
    
	Eigen::MatrixXcd num1 = psi_xi_alpha.block(0, 0, nmax, 1).cwiseProduct(psi_xi_beta.block(nmax+2, 0, nmax, 1));
    Eigen::MatrixXcd num2 = psi_xi_alpha.block(nmax+2, 0, nmax, 1).cwiseProduct(psi_xi_beta.block(0, 0, nmax, 1));
	Eigen::MatrixXcd den1 = psi_xi_alpha.block(2*(nmax+2), 0, nmax, 1).cwiseProduct(psi_xi_beta.block(nmax+2, 0, nmax, 1));
	Eigen::MatrixXcd den2 = psi_xi_alpha.block(3*(nmax+2), 0, nmax, 1).cwiseProduct(psi_xi_beta.block(0, 0, nmax, 1));

    Eigen::MatrixXcd an, bn;
    #pragma omp parallel sections
    {    
		an = (num1 - M*num2).cwiseQuotient(den1 - M*den2);
        #pragma omp section          
        bn = (M*num1 - num2).cwiseQuotient(M*den1 - den2);
    }
    
	Eigen::MatrixXcd an_bn(an.rows() + bn.rows(), an.cols());
	an_bn << an,
		bn;
    
	return an_bn;
}

Eigen::MatrixXd legendre_vector(int n_max, Eigen::MatrixXd x)
{
	Eigen::MatrixXd result(x.rows(), n_max);
    
	result.block(0, 0, result.rows(), 1) = -((1 - (x.cwiseProduct(x)).array()).matrix()).cwiseSqrt();
	result.block(0, 1, result.rows(), 1) = -3 * x.cwiseProduct(((1 - (x.cwiseProduct(x)).array()).matrix()).cwiseSqrt());
    
	for (int k = 3; k <= n_max; ++k)
		result.block(0, k - 1, result.rows(), 1) = ((2. * k - 1.) / (k - 1.)*x).cwiseProduct(result.block(0, k - 2, result.rows(), 1)) - ((double)k / (k - 1.))*result.block(0, k - 3, result.rows(), 1);

	return result;
}

Eigen::MatrixXd legendre_vector_debug(int n_max, Eigen::MatrixXd x)
{
	Eigen::MatrixXd result(n_max, x.cols());
	
    result.block(0, 0, 1, result.cols()) = -((1 - (x.cwiseProduct(x)).array()).matrix()).cwiseSqrt();
	result.block(1, 0, 1, result.cols()) = -3 * x.cwiseProduct(((1 - (x.cwiseProduct(x)).array()).matrix()).cwiseSqrt());
    
	for (int k = 3; k <= n_max; ++k)
		result.block(k - 1, 0, 1, result.cols()) = ((2. * k - 1.) / (k - 1.)*x).cwiseProduct(result.block(k - 2, 0, 1, result.cols())) - ((double)k / (k - 1.))*result.block(k - 3, 0, 1, result.cols());
    
	return result;
}

Eigen::MatrixXd derivative_legendre_vector(int n_max, Eigen::MatrixXd x, Eigen::MatrixXd leg_vec)
{
	Eigen::MatrixXd result(x.rows(), n_max);
	Eigen::MatrixXd den_mat(x.rows(), x.cols());

	den_mat = ((x.cwiseProduct(x)).array() - 1).matrix();
    
    #pragma omp parallel for
	for (int k = 1; k <= n_max; ++k)
		result.block(0, k - 1, result.rows(), 1) = (k*leg_vec.block(0, k, result.rows(), 1) - (k + 1)* x.cwiseProduct(leg_vec.block(0, k - 1, result.rows(), 1))).cwiseQuotient(den_mat);

	return result;
}

Eigen::MatrixXd derivative_legendre_vector_debug(int n_max, Eigen::MatrixXd x, Eigen::MatrixXd leg_vec)
{
	Eigen::MatrixXd result(n_max, x.cols());
	Eigen::MatrixXd den_mat(x.rows(), x.cols());

	den_mat = ((x.cwiseProduct(x)).array() - 1).matrix();

    #pragma omp parallel for
	for (int k = 1; k <= n_max; ++k)
		result.block(k - 1, 0, 1, result.cols()) = (k*leg_vec.block(k, 0, 1, result.cols()) - (k + 1)* x.cwiseProduct(leg_vec.block(k - 1, 0, 1, result.cols()))).cwiseQuotient(den_mat);
    
	return result;
}

Eigen::MatrixXcd compute_psi_xi_kr(int n_max, Eigen::MatrixXd kr)
{
    Eigen::MatrixXcd psi_psid_xi_xid_kr(n_max,4*kr.cols()); psi_psid_xi_xid_kr.setZero();    
    Eigen::MatrixXcd psi_kr(n_max,kr.cols()); psi_kr.setZero();
    Eigen::MatrixXcd psi_d_kr(n_max,kr.cols()); psi_d_kr.setZero();
    Eigen::MatrixXcd xi_kr(n_max,kr.cols()); xi_kr.setZero();
    Eigen::MatrixXcd xi_d_kr(n_max,kr.cols()); xi_d_kr.setZero();

    #pragma omp parallel for
    for (int index_kr = 0; index_kr < kr.cols(); ++index_kr)
    {
        Eigen::MatrixXcd current_psi_xi_kr = psi_xi(n_max, kr(0,index_kr));
        psi_kr.block(0,index_kr,n_max,1) = current_psi_xi_kr.block(0,0,1,n_max).transpose();
        psi_d_kr.block(0,index_kr,n_max,1) = current_psi_xi_kr.block(1,0,1,n_max).transpose();
        xi_kr.block(0,index_kr,n_max,1) = current_psi_xi_kr.block(2,0,1,n_max).transpose();
        xi_d_kr.block(0,index_kr,n_max,1) = current_psi_xi_kr.block(3,0,1,n_max).transpose();     
    }
    
    psi_psid_xi_xid_kr.block(0,0,n_max,kr.cols()) = psi_kr;
    psi_psid_xi_xid_kr.block(0,kr.cols(),n_max,kr.cols()) = psi_d_kr;
    psi_psid_xi_xid_kr.block(0,2*kr.cols(),n_max,kr.cols()) = xi_kr;
    psi_psid_xi_xid_kr.block(0,3*kr.cols(),n_max,kr.cols()) = xi_d_kr;
        
    return psi_psid_xi_xid_kr;
}

Eigen::MatrixXcd compute_psi_xi_kr_debug(int n_max, Eigen::MatrixXd kr)
{
    Eigen::MatrixXcd psi_psid_xi_xid_kr(n_max,4*kr.cols()); psi_psid_xi_xid_kr.setZero();    
    Eigen::MatrixXcd psi_kr(n_max,kr.cols()); psi_kr.setZero();
    Eigen::MatrixXcd psi_d_kr(n_max,kr.cols()); psi_d_kr.setZero();
    Eigen::MatrixXcd xi_kr(n_max,kr.cols()); xi_kr.setZero();
    Eigen::MatrixXcd xi_d_kr(n_max,kr.cols()); xi_d_kr.setZero();

    Eigen::MatrixXcd current_psi_xi_kr = psi_xi_debug(n_max, kr(0,0));

    #pragma omp parallel for
    for (int index_kr = 0; index_kr < kr.cols(); ++index_kr)
    {
        Eigen::MatrixXcd current_psi_xi_kr = psi_xi_debug(n_max, kr(0,index_kr));        
        psi_kr.block(0,index_kr,n_max,1) = current_psi_xi_kr.block(0,0,n_max,1);
        psi_d_kr.block(0,index_kr,n_max,1) = current_psi_xi_kr.block(n_max+1,0,n_max,1);
        xi_kr.block(0,index_kr,n_max,1) = current_psi_xi_kr.block(2*(n_max+1),0,n_max,1);
        xi_d_kr.block(0,index_kr,n_max,1) = current_psi_xi_kr.block(3*(n_max+1),0,n_max,1);     
    }
    
    psi_psid_xi_xid_kr.block(0,0,n_max,kr.cols()) = psi_kr;
    psi_psid_xi_xid_kr.block(0,kr.cols(),n_max,kr.cols()) = psi_d_kr;
    psi_psid_xi_xid_kr.block(0,2*kr.cols(),n_max,kr.cols()) = xi_kr;
    psi_psid_xi_xid_kr.block(0,3*kr.cols(),n_max,kr.cols()) = xi_d_kr;
        
    return psi_psid_xi_xid_kr;
}

Eigen::MatrixXd Mie(double a, double k, Eigen::MatrixXd x, Eigen::MatrixXd y, Eigen::MatrixXd z, double xe, double ye, std::complex<double> M)
{
	double alpha = k*a;
	if (alpha > 2299.)
	{
		printf("Warning ! : alpha = %f > 2300. alpha tresholded to 2300.", alpha);
		alpha = 2299.;
	}

	//printf("...n_max computation...\n");
	int nmax = n_max(alpha);
    
	//printf("...spherical coordinates computation...\n");
	Eigen::MatrixXd r_t_phi = cart_to_spher(x, y, z, xe, ye);
    
    //printf("...cosinus and sinus in spherical coordinates...\n");
    Eigen::MatrixXcd sin_phi = ((r_t_phi.block(2, 0, 1, r_t_phi.cols())).unaryExpr(std::ptr_fun(sin_d))).cast <std::complex<double> >();
    Eigen::MatrixXcd cos_phi = ((r_t_phi.block(2, 0, 1, r_t_phi.cols())).unaryExpr(std::ptr_fun(cos_d))).cast <std::complex<double> >();
    Eigen::MatrixXd sin_t = (r_t_phi.block(1, 0, 1, r_t_phi.cols())).unaryExpr(std::ptr_fun(sin_d));
	Eigen::MatrixXd cos_t = (r_t_phi.block(1, 0, 1, r_t_phi.cols())).unaryExpr(std::ptr_fun(cos_d));
    //sin_t.cast <std::complex<double> >(); // cast seems not be required
    //cos_t.cast <std::complex<double> >(); // cast seems not be required
    
	//printf("...c_w computation...\n");
    Eigen::MatrixXcd C_W_mip1_2np1 = cw_mip1_2np1(nmax);
    Eigen::MatrixXcd C_W = C_W_mip1_2np1.block(0,0,nmax,1);
    Eigen::MatrixXcd mip1 = C_W_mip1_2np1.block(0,1,nmax,1);
    Eigen::MatrixXcd dnp1 = C_W_mip1_2np1.block(0,2,nmax,1);
    
	//printf("...an_bn computation...\n");
	//Eigen::MatrixXcd an_bn_vector = an_bn(nmax, alpha, M);
	Eigen::MatrixXcd an_bn_vector = an_bn_debug(nmax, alpha, M);
    
	//printf("...legendre polynomials computation...\n");
 	Eigen::MatrixXd P1_ = legendre_vector_debug(nmax + 1, cos_t);
 	Eigen::MatrixXd P1 = P1_.block(0, 0, P1_.rows() - 1, P1_.cols());  

	//printf("...derivatives legendre polynomials computation...\n");
    Eigen::MatrixXd P1_d = derivative_legendre_vector_debug(nmax, cos_t, P1_);
        
	//printf("...pi_n & tau_n computation...\n");
    Eigen::MatrixXd sin_t_replicate = sin_t.replicate(P1.rows(), 1);
    Eigen::MatrixXd pi_n = P1.cwiseQuotient(sin_t_replicate);
    Eigen::MatrixXd tau_n = P1_d.cwiseProduct(-sin_t_replicate);
    
	//printf("...wave vector computation...\n");
	Eigen::MatrixXd kr = k * r_t_phi.block(0, 0, 1, r_t_phi.cols());
    Eigen::MatrixXd k2r2 = kr.cwiseProduct(kr);
    Eigen::MatrixXd inv_k2r2 = k2r2.cwiseInverse();
    Eigen::MatrixXd inv_kr = kr.cwiseInverse();
    
    //printf("...psi & xi = f(kr)...\n");
    Eigen::MatrixXcd psi_psid_xi_xid_kr = compute_psi_xi_kr_debug(nmax, kr);
    Eigen::MatrixXcd psi_kr = psi_psid_xi_xid_kr.block(0,0,nmax,kr.cols());
    Eigen::MatrixXcd psi_d_kr = psi_psid_xi_xid_kr.block(0,kr.cols(),nmax,kr.cols());
    Eigen::MatrixXcd xi_kr = psi_psid_xi_xid_kr.block(0,2*kr.cols(),nmax,kr.cols());
    Eigen::MatrixXcd xi_d_kr = psi_psid_xi_xid_kr.block(0,3*kr.cols(),nmax,kr.cols());
    
    //printf("...vectors extension...\n");
    Eigen::MatrixXcd C_W_et, mip1_et, dnp1_et, an_vector_et, bn_vector_et;
    #pragma omp parallel sections
    {    
        C_W_et = C_W.replicate(1, x.cols());
        #pragma omp section
        mip1_et = mip1.replicate(1, x.cols());
        #pragma omp section        
        dnp1_et = dnp1.replicate(1, x.cols());

        // 	Eigen::MatrixXcd an_vector_et = ((an_bn_vector.block(0, 0, 1, an_bn_vector.cols())).transpose()).replicate(1, x.cols());
        // 	Eigen::MatrixXcd bn_vector_et = ((an_bn_vector.block(1, 0, 1, an_bn_vector.cols())).transpose()).replicate(1, x.cols());
        #pragma omp section                
        an_vector_et = an_bn_vector.block(0, 0, nmax, 1).replicate(1, x.cols());
        #pragma omp section        
        bn_vector_et = an_bn_vector.block(nmax, 0, nmax, 1).replicate(1, x.cols());
    }
    
    //printf("...precompute terms...\n");
    Eigen::MatrixXcd an_vector_et_xi_kr, bn_vector_et_xi_kr, an_vector_et_xi_d_kr, bn_vector_et_xi_d_kr, C_W_et_pi_n, C_W_et_tau_n, mip1_et_dnp1_et_P1, exp_K;
    #pragma omp parallel sections
    {
        an_vector_et_xi_kr = an_vector_et.cwiseProduct(xi_kr);
        #pragma omp section
        bn_vector_et_xi_kr = bn_vector_et.cwiseProduct(xi_kr);
        #pragma omp section        
        an_vector_et_xi_d_kr = an_vector_et.cwiseProduct(xi_d_kr);
        #pragma omp section        
        bn_vector_et_xi_d_kr = bn_vector_et.cwiseProduct(xi_d_kr);
        #pragma omp section        
        C_W_et_pi_n = C_W_et.cwiseProduct(pi_n);
        #pragma omp section        
        C_W_et_tau_n = C_W_et.cwiseProduct(tau_n);
        #pragma omp section        
        mip1_et_dnp1_et_P1 = mip1_et.cwiseProduct(dnp1_et).cwiseProduct(P1);
        #pragma omp section        
        exp_K = (-1i*kr.cwiseProduct(cos_t)).unaryExpr(std::ptr_fun(exp_d));
    }
    
    Eigen::MatrixXcd Er_s, Et_s, Ep_s, Hr_s, Ht_s, Hp_s, Er_i, Et_i, Ep_i, Hr_i, Ht_i, Hp_i, Er_t, Et_t, Ep_t, Hr_t, Ht_t, Hp_t;
    #pragma omp parallel sections
    {
        //printf("...compute electrical scattered fields in spherical coords...\n");
        Er_s = (-inv_k2r2.cwiseProduct(cos_phi)).cwiseProduct((mip1_et_dnp1_et_P1.cwiseProduct(an_vector_et_xi_kr)).colwise().sum());
        #pragma omp section
        Et_s = (-inv_kr.cwiseProduct(cos_phi)).cwiseProduct((C_W_et_tau_n.cwiseProduct(an_vector_et_xi_d_kr) - 1i*C_W_et_pi_n.cwiseProduct(bn_vector_et_xi_kr)).colwise().sum());
        #pragma omp section
        Ep_s = (-inv_kr.cwiseProduct(sin_phi)).cwiseProduct((-C_W_et_pi_n.cwiseProduct(an_vector_et_xi_d_kr) + 1i*C_W_et_tau_n.cwiseProduct(bn_vector_et_xi_kr)).colwise().sum());

        //printf("...compute magnetic scattered fields in spherical coords...\n");
        #pragma omp section
        Hr_s = (-inv_k2r2.cwiseProduct(sin_phi)).cwiseProduct((mip1_et_dnp1_et_P1.cwiseProduct(bn_vector_et_xi_kr)).colwise().sum());
        #pragma omp section
        Ht_s = (inv_kr.cwiseProduct(sin_phi)).cwiseProduct((1i*C_W_et_pi_n.cwiseProduct(an_vector_et_xi_kr) - C_W_et_tau_n.cwiseProduct(bn_vector_et_xi_d_kr)).colwise().sum());
        #pragma omp section
        Hp_s = (inv_kr.cwiseProduct(cos_phi)).cwiseProduct((1i*C_W_et_tau_n.cwiseProduct(an_vector_et_xi_kr) - C_W_et_pi_n.cwiseProduct(bn_vector_et_xi_d_kr)).colwise().sum());
        
        //printf("...compute electrical incident fields in spherical coords...\n");
        #pragma omp section
        Er_i = cos_phi.cwiseProduct(sin_t).cwiseProduct(exp_K);
        #pragma omp section
        Et_i = cos_phi.cwiseProduct(cos_t).cwiseProduct(exp_K);
        #pragma omp section
        Ep_i = -sin_phi.cwiseProduct(exp_K);

        //printf("...compute magnetic incident fields in spherical coords...\n");
        #pragma omp section        
        Hr_i = sin_phi.cwiseProduct(sin_t).cwiseProduct(exp_K);
        #pragma omp section        
        Ht_i = sin_phi.cwiseProduct(cos_t).cwiseProduct(exp_K);
        #pragma omp section
        Hp_i = cos_phi.cwiseProduct(exp_K);
    }
        
    #pragma omp parallel sections
    {
        //printf("...compute total electrical fields in spherical coords...\n");
        Er_t = Er_s + Er_i;
        #pragma omp section        
        Et_t = Et_s + Et_i;
        #pragma omp section        
        Ep_t = Ep_s + Ep_i;

        //printf("...compute total magnetic fields in spherical coords...\n");
        #pragma omp section        
        Hr_t = Hr_s + Hr_i;
        #pragma omp section        
        Ht_t = Ht_s + Ht_i;
        #pragma omp section        
        Hp_t = Hp_s + Hp_i;
    }
    
    Eigen::MatrixXcd Ex_t, Ey_t, Ez_t, Hx_t, Hy_t, Hz_t;
    Eigen::MatrixXd I_near_field;
    #pragma omp parallel sections
    {    
        //printf("...compute total electrical fields in cartesian coords...\n");
        Ex_t = cos_phi.cwiseProduct(sin_t).cwiseProduct(Er_t) + cos_phi.cwiseProduct(cos_t).cwiseProduct(Et_t) - sin_phi.cwiseProduct(Ep_t);
        #pragma omp section                
        Ey_t = sin_phi.cwiseProduct(sin_t).cwiseProduct(Er_t) + sin_phi.cwiseProduct(cos_t).cwiseProduct(Et_t) + cos_phi.cwiseProduct(Ep_t);
        #pragma omp section                
        Ez_t = cos_t.cwiseProduct(Er_t) - sin_phi.cwiseProduct(Et_t);

        //printf("...compute total magnetic fields in cartesian coords...\n");
        #pragma omp section                
        Hx_t = cos_phi.cwiseProduct(sin_t).cwiseProduct(Hr_t) + cos_phi.cwiseProduct(cos_t).cwiseProduct(Ht_t) - sin_phi.cwiseProduct(Hp_t);
        #pragma omp section                
        Hy_t = sin_phi.cwiseProduct(sin_t).cwiseProduct(Hr_t) + sin_phi.cwiseProduct(cos_t).cwiseProduct(Ht_t) + cos_phi.cwiseProduct(Hp_t);
        #pragma omp section                
        Hz_t = cos_t.cwiseProduct(Hr_t) - sin_phi.cwiseProduct(Ht_t);    

        //printf("...compute intensity...\n");
        #pragma omp section                
        I_near_field = 0.5*((Et_t.cwiseProduct(Hp_t.conjugate()) - Ep_t.cwiseProduct(Ht_t.conjugate())).cwiseProduct(cos_t) - (Ep_t.cwiseProduct(Hr_t.conjugate()) - Er_t.cwiseProduct(Hp_t.conjugate())).cwiseProduct(sin_t)).real();
    }
    
    //prinff("...save fields...\n");
    /*Eigen::MatrixXd I_and_fields(13, I_near_field.cols());
    I_and_fields << I_near_field, 
                    Ex_t.real(), 
                    Ex_t.imag(),
                    Ey_t.real(),
                    Ey_t.imag(),
                    Ez_t.real(), 
                    Ez_t.imag(),
                    Hx_t.real(), 
                    Hx_t.imag(),
                    Hy_t.real(), 
                    Hy_t.imag(),
                    Hz_t.real(), 
                    Hz_t.imag();*/
        
	return I_near_field;
    
}
