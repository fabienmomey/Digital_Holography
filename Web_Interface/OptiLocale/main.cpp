#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/eigen.hpp>
#include <complex>

#include <unsupported/Eigen/NonLinearOptimization>
#include <unsupported/Eigen/NumericalDiff>


#include "mie_profil_mex_near_field.hpp"
#include "optilocale.h"

#define PI 3.141592653589793238462643383279502884

std::string pad_left(std::string const& str, size_t s, char c = '0')
{
	if (str.size() < s) return std::string(s - str.size(), c) + str;
	else return str;
}

// Generic functor
template<typename _Scalar, int NX = Eigen::Dynamic, int NY = Eigen::Dynamic>
struct Functor
{
	typedef _Scalar Scalar;
	enum {
		InputsAtCompileTime = NX,
		ValuesAtCompileTime = NY
	};
	typedef Eigen::Matrix<Scalar, InputsAtCompileTime, 1> InputType;
	typedef Eigen::Matrix<Scalar, ValuesAtCompileTime, 1> ValueType;
	typedef Eigen::Matrix<Scalar, ValuesAtCompileTime, InputsAtCompileTime> JacobianType;

	int m_inputs, m_values;

	Functor() : m_inputs(InputsAtCompileTime), m_values(ValuesAtCompileTime) {}
	Functor(int inputs, int values) : m_inputs(inputs), m_values(values) {}

	int inputs() const { return m_inputs; }
	int values() const { return m_values; }

};

struct CostFunction : Functor<double>
{
	CostFunction(cv::Mat image, double n0, double n1i, double lambda, double pix, double xe, double ye, double factorZ, double factorR, double factorN, int N, int output_size) : Functor<double>(5, output_size), cv_holo_(image) {
		n0_ = n0;
		n1i_ = n1i;
		lambda_ = lambda;
		pix_ = pix;
		xe_ = xe;
		ye_ = ye;
		factorZ_ = factorZ;
		factorR_ = factorR;
		factorN_ = factorN;
		N_ = N;
		output_size_ = output_size;

		cv::cv2eigen(cv_holo_, holo_);
		holo_ = (holo_.array() - holo_.mean()).matrix();
		w_ = Eigen::MatrixXd::Ones(holo_.rows(), holo_.cols());

	}

	Eigen::MatrixXd computeCostFunction(const Eigen::VectorXd &x1) const {
		double x = x1(0);
		double y = x1(1);
		double z = x1(2);
		double r = x1(3);
		double n1r = x1(4);

		int width = holo_.cols();
		int height = holo_.rows();


		// Calculs des entrées
		Eigen::MatrixXd X(1, N_);
		Eigen::MatrixXd Y(1, N_);
		Eigen::MatrixXd Z(1, N_);
		Eigen::MatrixXd I(1, N_);
		Eigen::MatrixXd R(height, width);

		for (int i = 0; i < R.rows(); i++) {
			for (int j = 0; j < R.cols(); j++) {
				int a = -floor(R.cols() / 2) + j;
				int b = -floor(R.rows() / 2) + i;
				R(i, j) = sqrt(pow(a - y / pix_, 2) + pow(b - x / pix_, 2));
			}
		}
		
		double k = 2 * PI*n0_ / lambda_;
		std::complex<double> M(n1r / n0_, n1i_ / n0_);

		double x_min = 0;
		double x_max = R.maxCoeff();

		double step_x = pix_*(x_max - x_min) / (N_ - 1);

		for (int p = 0; p < N_; p++)
			X(0, p) = double(x_min + step_x * p);

		Y.setConstant(0);
		Z.setConstant(z);
		I = Mie(r, k, X, Y, Z, xe_, ye_, M);

		Eigen::MatrixXd ind(height, width);
		ind << (R.array() * (N_ - 1) / (x_max)).matrix();

		Eigen::MatrixXd res(holo_.rows(), holo_.cols());

		for (int i = 0; i < res.rows(); i++) {
			for (int j = 0; j < res.cols(); j++) {
				res(i, j) = I(0, floor(ind(i, j) + 0.5));
			}
		}

		res = (res.array() - res.mean()).matrix();

		Eigen::MatrixXd corr = corr_w_dc(w_, res, holo_);
		Eigen::MatrixXd pp = holo_ - (corr(1, 0) + corr(0, 0) * res.array()).matrix();

		res = ((res.array() - res.minCoeff()) / (res.maxCoeff() - res.minCoeff())).matrix();

		return pp;
	}

	int operator()(const Eigen::VectorXd &x1, Eigen::VectorXd &fvec) const
	{
		Eigen::MatrixXd pp = computeCostFunction(x1);
		fvec = Eigen::Map<Eigen::VectorXd>(pp.data(), output_size_);

		return 0;
	}

	double n0_;
	double n1i_;
	double lambda_;
	double pix_;
	double xe_;
	double ye_;
	double factorZ_;
	double factorR_;
	double factorN_;
	int N_;
	int output_size_;

	cv::Mat cv_holo_;
	Eigen::MatrixXd holo_;
	Eigen::MatrixXd w_;
};

int main(int argc, char** argv) {
	double x;
	double y ;
	double z;
	double r;
	double n1r;
	double n1i;

	double n0;
	double lambda;
	double pix;
	double xe;
	double ye;
	int N;
	std::string id, filename;
	cv::Mat cv_holo;
	bool pause = false;

	std::fstream output, results;

	auto endt = std::chrono::system_clock::now();
	std::time_t end_time = std::chrono::system_clock::to_time_t(endt);

	auto clock_beg = clock();
	// Calcul de la date
#ifdef _WIN32 || _WIN64
	char str[26];
	ctime_s(str, 26, &end_time);
	tm timePtr;
	localtime_s(&timePtr, &end_time);
#else
	char *str;
	str = std::ctime(&end_time);
	tm *tprt = localtime(&end_time);
	tm timePtr = *tprt;
#endif
	// si le programme est exécuté avec des paramètres
	if (argc == 15) {
		// Récupération des paramètres
		filename = argv[1];
		cv_holo = cv::imread(filename, cv::IMREAD_GRAYSCALE);
		id = argv[2];

		// Sortie dans un fichier log
		std::string isodate = pad_left(std::to_string(timePtr.tm_year + 1900), 4) + pad_left(std::to_string(timePtr.tm_mon + 1), 2) + pad_left(std::to_string(timePtr.tm_mday), 2);
		std::string logname = "optilocale_" + isodate + "_" + id + ".txt";
		std::string logpath = "output/log/" + logname;
		output.open(logpath, std::ios::out);

		output << "Start - Optimisation locale : " << str << "------------------ " << std::endl;
		

		x = std::stod(argv[3]);
		y = std::stod(argv[4]);
		z = std::stod(argv[5]);
		r = std::stod(argv[6]);
		n1r = std::stod(argv[7]);
		n1i = std::stod(argv[8]);

		lambda = std::stod(argv[9]);
		pix = std::stod(argv[10]);
		n0 = std::stod(argv[11]);
		xe = std::stod(argv[12]);
		ye = std::stod(argv[13]);
		N = std::stoi(argv[14]);

		// Conversion pixel en mètres
		x *= pix;
		y *= pix;
	}
	// sinon, utilisation des paramètres par défaut
	else {
		// Sortie dans la console (sortie standard)
		output.basic_ios<char>::rdbuf(std::cout.rdbuf());

		x = 1.1995e-5 + 0.001e-5;
		y = 2.1722e-5 + 0.001e-5;
		z = 1.0017e-5 - 0.1e-5;
		r = 5.5993e-7 + 0.1e-7;
		n1r = 1.6348 - 0.1;
		n1i = 0;

		lambda = 532e-9;
		pix = 2.2e-6 / 56.7;
		n0 = 1.5193;
		xe = 0;
		ye = 0;
		N = 10800;

		// Image
		cv_holo = cv::imread("uploads/Basler.png", cv::IMREAD_GRAYSCALE);
		pause = true;
	}

	double factors[] = { 0.0000000187360689, 0.00000005842192278, 0.00000001310390391, 0.0069802928063578, 10, 10 };
	double factorZ = factors[1] / factors[0];
	double factorR = factors[2] / factors[0];
	double factorN = factors[3] / factors[0];

	// parameters
	/*double x = 1.1995e-5;
	double y = 2.1722e-5;
	double z = 1.0017e-5;
	double r = 5.5993e-7;
	double n1r = 1.6348;*/

	Eigen::VectorXd x1(5);
	x1(0) = x;
	x1(1) = y;
	x1(2) = z;
	x1(3) = r;
	x1(4) = n1r;
	output << "Filename: " << filename << std::endl;
	output << "Destination: output/optilocale/" << id <<+ ".txt" << std::endl;
	output << "x: " << x << std::endl;
	output << "y: " << y << std::endl;
	output << "z: " << z << std::endl;
	output << "r: " << r << std::endl;
	output << "n1 (Re): " << n1r << std::endl;
	output << "n1 (Im): " << n1i << std::endl;
	output << "lambda: " << lambda << std::endl;
	output << "pix: " << pix << std::endl;
	output << "n0: " << n0 << std::endl;
	output << "xe: " << xe << std::endl;
	output << "ye: " << ye << std::endl;
	output << "N: " << N << std::endl;

	// Paramétrage de l'optimiseur
	CostFunction functor(cv_holo, n0, n1i, lambda, pix, xe, ye, factorZ, factorR, factorN, N, cv_holo.rows * cv_holo.cols);
	
	Eigen::NumericalDiff<CostFunction> numDiff(functor);
	Eigen::LevenbergMarquardt<Eigen::NumericalDiff<CostFunction>, double> lm(numDiff);
	lm.parameters.maxfev = 10000;
	lm.parameters.xtol = factors[4]*factors[0];
	lm.parameters.ftol = 1e-40;

	output << "maxfev : " << lm.parameters.maxfev << std::endl;
	output << "xtol : " << lm.parameters.xtol << std::endl;
	output << "ftol : " << lm.parameters.ftol << std::endl;
	output << "gtol : " << lm.parameters.ftol << std::endl;
	output << "------------------------------------" << std::endl;

	/*x1(2) /= factorZ;
	x1(3) /= factorR;
	x1(4) /= factorN;*/

	// Optimisation
	int ret = lm.minimize(x1);


	std::cout << "Iterations: " << lm.iter << std::endl;
	std::string message;
	switch (ret) {
	case Eigen::LevenbergMarquardtSpace::Status::CosinusTooSmall:
		message = "Cosinus too small";
		break;
	case Eigen::LevenbergMarquardtSpace::Status::FtolTooSmall:
		message = "Ftol too small";
		break;
	case Eigen::LevenbergMarquardtSpace::Status::GtolTooSmall:
		message = "Gtol too small";
		break;
	case Eigen::LevenbergMarquardtSpace::Status::ImproperInputParameters:
		message = "Improper input parameters";
		break;
	case Eigen::LevenbergMarquardtSpace::Status::NotStarted:
		message = "Not started";
		break;
	case Eigen::LevenbergMarquardtSpace::Status::RelativeErrorAndReductionTooSmall:
		message = "Relative error and reduction too small";
		break;
	case Eigen::LevenbergMarquardtSpace::Status::RelativeErrorTooSmall:
		message = "Relative error too small";
		break;
	case Eigen::LevenbergMarquardtSpace::Status::RelativeReductionTooSmall:
		message = "Relative reduction too small";
		break;
	case Eigen::LevenbergMarquardtSpace::Status::Running:
		message = "Running";
		break;
	case Eigen::LevenbergMarquardtSpace::Status::TooManyFunctionEvaluation:
		message = "Too many function evaluation";
		break;
	case Eigen::LevenbergMarquardtSpace::Status::UserAsked:
		message = "User asked";
		break;
	case Eigen::LevenbergMarquardtSpace::Status::XtolTooSmall:
		message = "Xtol too small";
		break;
	}
	output << "Ended with message: " << message << std::endl;
	

	/*x1(2) *= factorZ;
	x1(3) *= factorR;
	x1(4) *= factorN;*/

	// Calcul du temps écoulé
	endt = std::chrono::system_clock::now();
	end_time = std::chrono::system_clock::to_time_t(endt);
#ifdef _WIN32 || _WIN64
	ctime_s(str, 26, &end_time);
#else
	str = std::ctime(&end_time);
#endif
	

	if (pause) {
		output << "ID: " << id << std::endl;
		output << "x: " << x1(0) << std::endl;
		output << "y: " << x1(1) << std::endl;
		output << "z: " << x1(2) << std::endl;
		output << "r: " << x1(3) << std::endl;
		output << "n: " << x1(4) << std::endl;
		output << "Completed - " << str << std::endl;
		std::cout << "press [ENTER] to continue " << std::endl;
		std::cin.get();
	}
	else {
		results.open("output/optilocale/"+id+".txt", std::ios::out);
		results << "ID: " << id << std::endl;
		results << "x: " << x1(0) << std::endl;
		results << "y: " << x1(1) << std::endl;
		results << "z: " << x1(2) << std::endl;
		results << "r: " << x1(3) << std::endl;
		results << "n: " << x1(4) << std::endl;
		output << "Completed - " << str << std::endl;
	}

	

	/*CostFunction functor2(cv_holo, n0, n1i, lambda, pix, xe, ye, factorZ, factorR, factorN, N, cv_holo.rows * cv_holo.cols);
	cv::Mat cv_res;
	Eigen::MatrixXd pp = functor2.computeCostFunction(x1);

	cv::eigen2cv(pp, cv_res);
	double mini, maxi;
	cv::minMaxIdx(cv_res, &mini, &maxi);
	std::cout << mini << " " << maxi << std::endl;

	cv::imshow("Mie Model", cv_res);
	cv::waitKey();*/


	return 0;
}