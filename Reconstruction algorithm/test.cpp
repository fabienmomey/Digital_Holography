
///
/// \file test.cpp
/// 
/// \author Yoann MASSET - Fabien MOMEY
/// \version Final version
/// \date June 20th 2020
/// 
/// \brief Main file for testing algorithm reconstruction
/// 
/// All necessary files are availabale on GitHub
/// 

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include "tools.h"
#include "algorithms.h"

#include <iostream>
#include <fstream>
#include <opencv2/highgui.hpp>
#include <experimental/filesystem>

using namespace std;
using namespace cv;
namespace fs = std::experimental::filesystem;

int main()
{
	double e1 = getTickCount();

	/*
	Image choice :
		1 = dephasing image
		2 = absorbing image
	*/
	int image_choice = 2;

	/*
	Method choice :
		1 = Simple backpropagation
		2 = Fienup method
		3 = Regularised inversion method (FISTA)
	*/
	int method_choice = 3;

	double start_z, end_z, step_z;
	string source_file, dest_files;
	int do_padding, object;

	Settings param;
	double extremums[2] = EXTREMUMS;
	CalculMethod calcul_method;

	cout << "\n****************************************************************************************************\n";

	switch (image_choice)
	{
	case 1:
		cout << "Dephasing hologram reconstruction ";
		source_file = "2019_06_07_billes1mu_63_JOSAA/Basler_daA1920-30um_(22030948)_20190607_122251706_0012_crop.tiff";
		dest_files = "test"; //The destination folder must be created before executing this code

		start_z = 5e-6;
		end_z = 10e-6;
		step_z = 0.3e-6;

		param.mag = 56.7;
		param.n0 = 1.52;

		param.lambda = 532e-9;
		param.pixel_size = 2.2e-6 / param.mag;

		do_padding = 1; //We do padding
		object = 0; //Dephasing object

		if (method_choice == 1) calcul_method = MODULE_CALCUL;
		else calcul_method = PHASE_CALCUL;

		extremums[0] = 0;
		extremums[1] = CV_PI / 2;

		break;

	case 2:
		cout << "Absorbing hologram reconstruction ";
		source_file = "pika_holo.png";
		dest_files = "test"; //The destination folder must be created before executing this code

		start_z = 2.5e-5;
		end_z = 4e-5;
		step_z = 1e-6;
		param.lambda = 6e-7;
		param.pixel_size = 4.4e-8;

		param.mag = 56.7;
		param.n0 = 1.52;

		do_padding = 1; //We do padding
		object = 1; //Absorbing object

		calcul_method = MODULE_CALCUL;

		if (method_choice == 3)
		{
			extremums[0] = -1;
			extremums[1] = 0;
		}
		else
		{
			extremums[0] = 0;
			extremums[1] = 1;
		}
		break;

	default:
		cout << "Can not find the image choice !" << endl;
		return -1;
		break;
	}

	int do_sqrt, repetitions;
	switch (method_choice)
	{
	case 1:
		cout << "with simple backpropagation method !\n";
		do_sqrt = 1;
		break;

	case 2:
		cout << "with Fienup method !\n";
		do_sqrt = 1;
		repetitions = 5;
		break;

	case 3:
		cout << "with regularised inversion method (FISTA) !\n";
		do_sqrt = 0;
		repetitions = 10;
		break;

	default:
		cout << "Can not apply this reconstruction method ! Choose a method between 1 and 3\n";
		return -1;
		break;
	}
	cout << "****************************************************************************************************\n\n";

	Mat hologram = imread(source_file, IMREAD_UNCHANGED);
	if (hologram.empty())
	{
		cout << "Can not open or read the image !" << endl;
		return -1;
	}
	Mat reconstituted_image;	
	
	Mat optimal_hologram;
	image_calibration(hologram, optimal_hologram, do_padding, do_sqrt);

	int initial_size[] = { hologram.rows, hologram.cols };
	int final_size[] = { optimal_hologram.rows, optimal_hologram.cols };

	//We change the size parameters
	param.width = final_size[0];
	param.height = final_size[1];
	
	int i = 0;
	//We reconstitute the hologram for each z value between start_z and end_z with a step_z between each value

	for (double z = start_z; z <= end_z; z += step_z)
	{
		param.z = z;

		switch (method_choice)
		{
		case 1:
			simple_backpropagation(optimal_hologram, reconstituted_image, param);
			break;

		case 2:
			fienup_algorithm(optimal_hologram, reconstituted_image, param, object, extremums, repetitions);
			break;

		case 3:
			RI_algorithme_FISTA(optimal_hologram, reconstituted_image, param, object, extremums, repetitions);
			break;

		default:
			cout << "Can not apply this reconstruction method ! Choose a method between 1 and 3\n";
			return -1;
			break;
		}

		int row_margin = floor(1.0 * (final_size[0] - initial_size[0]) / 2), col_margin = floor(1.0 * (final_size[1] - initial_size[1]) / 2);
		Mat(reconstituted_image, Rect(col_margin, row_margin, initial_size[1], initial_size[0])).copyTo(reconstituted_image);

		if (method_choice != 3) complex_display(reconstituted_image, reconstituted_image, calcul_method); //Converting the final complex image into a displayable image by calculating its phase or module

		normalize(reconstituted_image, reconstituted_image, 0, 65535, NORM_MINMAX); //Normalizing the image so that it can be registered under an extension on 16 bits
		reconstituted_image.convertTo(reconstituted_image, CV_16U);

		imwrite(dest_files + "/Res" + to_string(i) + ".png", reconstituted_image); //Save the final_image
		
		i++;
	}

	double e2 = getTickCount();
	double seconds = (e2 - e1) / getTickFrequency();
	cout << "\nThe program ran in " << seconds << " seconds !\n";

	return 0;
}
