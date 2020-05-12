#ifndef PARALLEL_PROCESS_H

#include <opencv2/core.hpp>
#include <fstream>

#include "constantes.h"
#include "propagation.h"

class ProcessImageMain : public cv::ParallelLoopBody {
	public:
		ProcessImageMain(std::vector<cv::Mat> &allres, const cv::Mat &img, std::fstream&output, int &k, int nb, double start, double step, double pp, double lambda, double zeropad) : allres_(allres), img_(img), output_(output), k_(k) {
			pp_ = pp;
			lambda_ = lambda;
			zeropad_ = zeropad;
			start_ = start;
			step_ = step;
			nb_ = nb;
		}

		virtual void operator()(const cv::Range &range) const override {
			for (int i = range.start; i < range.end; i++) {
				auto clock_beg = clock();

				double value = start_ + i * step_;
				output_ << "Writting image with depth " << value << " - " << i << " out of " << nb_ << std::endl;
				cv::Mat res = propagation(img_, lambda_, -value, pp_, zeropad_);
				allres_[i] = res;
				k_++;

				auto clock_end = clock();
				auto elapsed = 1000.*(double)(clock_end - clock_beg) / CLOCKS_PER_SEC;
				output_ << "Image " << i << " computed in " << elapsed << " ms." << " " << 100.*k_ / (double)nb_ << "% completed!" << std::endl;
			}
		}

		ProcessImageMain& operator=(const ProcessImageMain &) {
			return *this;
		};
	private:
		std::vector<cv::Mat> &allres_;
		std::fstream &output_;
		int &k_;

		const cv::Mat &img_;
		double pp_;
		double lambda_;
		double zeropad_;
		double start_, step_;
		int nb_;
};


class FresnelFunctionParallel : public cv::ParallelLoopBody {
	public:
		FresnelFunctionParallel(cv::Mat &hz, std::complex<double> c1, std::complex<double> c2, double z, double lambda) : hz_(hz) {
			c1_ = c1;
			c2_ = c2;
			lignes_ = hz.rows;
			colones_ = hz.cols;
		}

		virtual void operator()(const cv::Range &range) const override {
			for (int r = range.start; r < range.end; r++) {
				int i = r / colones_;
				int j = r % colones_;

				auto res = exp(I*2.*PI*z_ / lambda_) * c1_ * exp(c2_ * (pow(-floor(colones_ / 2.) + j, 2) + pow(-floor(lignes_ / 2.) + i, 2)));
				hz_.at<cv::Vec2d>(i, j)[0] = res.real();
				hz_.at<cv::Vec2d>(i, j)[1] = res.imag();
			}
		}

		FresnelFunctionParallel& operator=(const FresnelFunctionParallel &) {
			return *this;
		};

	private:
		cv::Mat &hz_;

		int lignes_, colones_;
		double lambda_, z_;
		std::complex<double> c1_, c2_;

};

class FresnelFunctionParallelFT : public cv::ParallelLoopBody {
public:
	FresnelFunctionParallelFT(cv::Mat &hz, double z, double lambda, double pp) : hz_(hz) {
		lignes_ = hz.rows;
		colones_ = hz.cols;

		z_ = z;
		lambda_ = lambda;
		pp_ = pp;
	}

	virtual void operator()(const cv::Range &range) const override {
		for (int r = range.start; r < range.end; r++) {
			int i = r / colones_;
			int j = r % colones_;

			auto res = exp(I*2.*PI*z_ / lambda_) * exp(-I * PI*lambda_*z_ * (pow((-floor(colones_ / 2.) + j) / colones_ / pp_, 2) + pow((floor(lignes_ / 2.) + i) / lignes_ / pp_, 2)));
			hz_.at<cv::Vec2d>(i, j)[0] = res.real();
			hz_.at<cv::Vec2d>(i, j)[1] = res.imag();
		}
	}

	FresnelFunctionParallelFT& operator=(const FresnelFunctionParallelFT &) {
		return *this;
	};

private:
	cv::Mat &hz_;

	int lignes_, colones_;
	double lambda_, z_, pp_;
	std::complex<double> c1_, c2_;

};


class ParallelComplexMultiply : public cv::ParallelLoopBody {
public:
	ParallelComplexMultiply(cv::Mat *A, cv::Mat *B, cv::Mat &C) : A_(A), B_(B), C_(C) {
		colones_ = A[0].rows;
	}

	virtual void operator()(const cv::Range &range) const override {
		for (int r = range.start; r < range.end; r++) {
			int i = r / colones_;
			int j = r % colones_;
		}
	}

	ParallelComplexMultiply& operator=(const ParallelComplexMultiply &) {
		return *this;
	};

private:
	cv::Mat &C_;

	cv::Mat *A_, *B_;
	int colones_;
};



#endif // !PARALLEL_PROCESSES_H
