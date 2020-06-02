#include <iostream>
#include <chrono>
#include <ctime>
#include <fstream>
#include <experimental/filesystem>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "parallelProcess.h"

namespace fs = std::experimental::filesystem;

std::string remove_extension(const std::string& filename) {
	size_t lastdot = filename.find_last_of(".");
	if (lastdot == std::string::npos) return filename;
	return filename.substr(0, lastdot);
}

std::string pad_left(std::string const& str, size_t s, char c = '0')
{
	if (str.size() < s) return std::string(s - str.size(), c) + str;
	else return str;
}


int main(int argc, char** argv) {
	std::fstream output, params;
	
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
	double start, end, step, lambda, pp;
	int zeropad;
	bool test = false;
	std::string filename, destname;

	// Si le programme est exécuté avec des paramètres
	if (argc == 9) {
		// Chargement des paramètres
		filename = argv[1];
		destname = argv[2];
		// Sortie : fichier log
		std::string isodate = pad_left(std::to_string(timePtr.tm_year + 1900), 4) + pad_left(std::to_string(timePtr.tm_mon + 1), 2) + pad_left(std::to_string(timePtr.tm_mday), 2);
		std::string logname = "propag_" + isodate + "_" + destname + ".txt";
		std::string logpath = "output/log/" + logname;
		output.open(logpath, std::ios::out);
		output << "Start - Back propagation : " << str << "------------------ " << std::endl;

		start = std::stod(argv[3]);
		end = std::stod(argv[4]);
		step = std::stod(argv[5]);

		lambda = std::stod(argv[6]);
		pp = std::stod(argv[7]);

		zeropad = std::stoi(argv[8]);
	}
	// Sinon, chargement des paramètres par défauts
	else {
		test = true;
		output.basic_ios<char>::rdbuf(std::cout.rdbuf());

		filename = "uploads/Reticle_LaHC.png";
		destname = "test";

		start = 240e-3;
		end = 340e-3;;
		step = 5e-3;

		lambda = 0.532e-6;
		pp = 7e-6;

		zeropad = 1;
	}

	output << "Filename : " << filename << std::endl;
	output << "Destination folder : output/propagation/" << destname << std::endl;
	output << "Starting depth : " << start << std::endl;
	output << "Ending depth : " << end << std::endl;
	output << "step : " << step << std::endl;
	output << "Lambda : " << lambda << std::endl;
	output << "Pixel size : " << pp << std::endl;
	output << "Zero padding : " << zeropad << std::endl;
	output << "------------------------------------" << std::endl;

	// Chargement de l'image
	output << "Loading " << filename << std::endl;
	cv::Mat img;
	img = cv::imread(filename, cv::IMREAD_GRAYSCALE);
	std::vector<cv::Mat> allres;
	
	// Création du répertoire de sortie
	fs::path p(filename);

	std::string pathname = "output/propagation/" + destname + "/";
	fs::create_directories(fs::path(pathname));

	output << "Creating output directory " << pathname << std::endl;

	const int nb = (end - start) / step + 1;
	allres.resize(nb);
	int k = 0;

	// Parallélisation du traitement
	ProcessImageMain pm(allres, img, output, k, nb, start, step, pp, lambda, zeropad);
	parallel_for_(cv::Range(0, nb), pm);
		
	// Normalisation sur toutes les images
	double valMax, valMin;
	valMax = std::numeric_limits<double>::min();
	valMin = std::numeric_limits<double>::max();
	for (size_t i = 0; i < allres.size(); i++) {
		double mini, maxi;
		cv::minMaxIdx(allres[i], &mini, &maxi);
		if (mini < valMin) valMin = mini;
		if (maxi > valMax) valMax = maxi;

		output << i << " - Max value : " << valMax << " _ Min value : " << valMin << std::endl;
	}

	for (size_t i = 0; i < allres.size(); i++) {
		allres[i] += valMin;
		allres[i] /= valMax - valMin;
		allres[i] *= 255;
		allres[i].convertTo(allres[i], CV_8U);
		cv::imwrite(pathname + "res" + std::to_string(i) + ".png", allres[i]);
		output << pathname + "res" + std::to_string(i) + ".png saved" << std::endl;
	}

	// Ecriture des paramètres
	params.open(pathname + "param.html", std::ios::out);

	params << "<p>File name : <span id = 'filename'>" << filename << "</span><br>" << std::endl;
	params << "<span id='destname' style='display:none;'>" << destname << "</span>" << std::endl;
	params << "Starting depth : <span id='startz'>" << start << "</span> m <br>"<< std::endl;
	params << "Ending depth : <span id='endz'>" << end << "</span> m <br>" << std::endl;
	params << "Step : <span id='stepz'> " << step << "</span> m <br>" << std::endl;
	params << "Lambda : " << lambda << " m <br>" << std::endl;
	params << "Pixel size : " << pp << " m <br></p>" << std::endl;

	params.close();

	// Calcul du temps écoulé
	endt = std::chrono::system_clock::now();
	end_time = std::chrono::system_clock::to_time_t(endt);
#ifdef _WIN32 || _WIN64
	ctime_s(str, 26, &end_time);
#else
	str = std::ctime(&end_time);
#endif
	output << "Completed - " << str << std::endl;

	auto clock_end = clock();

	auto elapsed_msecs = double(clock_end - clock_beg) / CLOCKS_PER_SEC * 1000.;
	output << "Elapsed time : " << elapsed_msecs << " ms" << std::endl;

	if (!test) 
		output.close();
	else
		system("pause");

	return 0;
}