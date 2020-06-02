
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include "function.h"

#include <iostream>
#include <fstream>
#include <opencv2/highgui.hpp>
#include <experimental/filesystem>

using namespace std;
using namespace cv;
namespace fs = std::experimental::filesystem;

/*
    The different arguments to put in input to execute this program :

    argv[0]: The name of the executable 
    argv[1]: The path to the source image
    argv[2]: The path to the destination folder
    argv[3]: The beginning of the interval of z
    argv[4]: The end of the interval of z
    argv[5]: The step between each value of z
    argv[6]: The wavelength used
    argv[7]: The pixel size
    argv[8]: If you want to do padding on the image or not: 1 = padding
    argv[9]: The "type" of the object: 0 = phase object or 1 = absorbing object
    argv[10]: The minimum value of the constraint (if type_object = phase_object else 0)
    argv[11]: The maximum value of the constraint (if type_object = phase_object else 1)
*/

int main(int argc, char* argv[])
{
    
    //Hologram reconstruction with the "Fienup" method -------------------------------------------------------------------------------------------------------------------------------------------

    double e1 = getTickCount();
    
    bool test = false;
    fstream output, params;

    string source_file, dest_files;
    double start_z, end_z, step_z, lambda, pixel;
    int do_padding, object;
    
    //If all parameters are filled in
    if (argc >= 10)
    {
        source_file = argv[1];
        dest_files = argv[2];
        start_z = atof(argv[3]);
        end_z = atof(argv[4]);
        step_z = atof(argv[5]);
        lambda = atof(argv[6]);
        pixel = atof(argv[7]);
        do_padding = atoi(argv[8]);
        object = atoi(argv[9]);

        string logpath = "output/log/fienup_" + dest_files + ".txt"; //The path to the log file
        output.open(logpath, ios::out); //Open the log file to write the state of the program
        output << "---------- Start - Fienup reconstitution ---------- " << endl;

    }
    //Else : load default parameters
    else
    {
        source_file = "uploads/Reticle_LaHC.png"; //2019_06_07_billes1mu_63_JOSAA/Basler_daA1920-30um_(22030948)_20190607_122251706_0012_crop.tiff
        dest_files = "test"; //output/propagation/
        start_z = 5e-6;
        end_z = 10e-6;
        step_z = 0.5e-6;
        lambda = 532e-9;
        pixel = 3.88e-8;
        do_padding = 1;
        object = 1;

        test = true;
        output.basic_ios<char>::rdbuf(cout.rdbuf());
    }
    
    double extremums[2] = EXTREMUMS;
    //We modify the minimum and maximum value of the constraint
    if (object == 0)
    {
        extremums[0] = atof(argv[10]);
        extremums[1] = atof(argv[11]);
    }
    else
    {
        extremums[0] = 0;
        extremums[1] = 1;
    }

    CalculMethod calcul_method;
    if (object == 0) calcul_method = PHASE_CALCUL;
    else calcul_method = MODULE_CALCUL;


    string pathname = "output/propagation/" + dest_files + "/"; //The path to the destination files

    output << "Filename: " << source_file << endl;
    output << "Destination folder: " << pathname << endl;
    output << "Starting depth: " << start_z << endl;
    output << "Ending depth: " << end_z << endl;
    output << "Step: " << step_z << endl;
    output << "Lambda: " << lambda << endl;
    output << "Pixel size: " << pixel << endl;
    output << "Zero padding: " << do_padding << endl;
    output << "Object type: " << object << endl;
    output << "Min value constraint: " << extremums[0] << endl;
    output << "Max value constraint: " << extremums[1] << endl;
    output << "------------------------------------" << endl;


    //The initial hologram 
    Mat hologram = imread(source_file, IMREAD_UNCHANGED);
    if (hologram.empty())
    {
        output << "Can not open or read the image" << endl;
        return -1;
    }


    fs::create_directories(fs::path(pathname));
    output << "Creating output directory: " << pathname << endl;


    Settings param;
    param.lambda = lambda;
    param.pixel_size = pixel;


    param.mag = 56.7;
    param.n0 = 1.52;
    

    int repetitions = 5;
    Mat reconstitued_image;
    string dest_name;
    
    int i = 0;

    //We reconstitute the hologram for each z value between start_z and end_z with a step_z between each value

    for (double z = start_z; z <= end_z; z+=step_z) 
    {
        param.z = z;
        fienup_reconstitution(hologram, reconstitued_image, param, object, extremums, repetitions, do_padding); //Hologram reconstitution
        complex_display(reconstitued_image, reconstitued_image, calcul_method); //Converting the final complex image into a displayable image by calculating its phase or module

        normalize(reconstitued_image, reconstitued_image, 0, 65535, NORM_MINMAX); //Normalizing the image so that it can be registered under an extension on 16 bits
        reconstitued_image.convertTo(reconstitued_image, CV_16U);

        dest_name = pathname + "Res" + to_string(i) + ".png";
        imwrite(dest_name, reconstitued_image); //Save the final_image
        i++;
        output << dest_name << " saved" << endl;
    }

    params.open(pathname + "param.html", ios::out); //To write the parameters used

    params << "<p>File name: <span id = 'filename'>" << source_file << "</span><br>" << endl;
    params << "<span id='destname' style='display:none;'>" << dest_files << "</span>" << endl;
    params << "Starting depth: <span id='startz'>" << start_z << "</span> m <br>" << endl;
    params << "Ending depth: <span id='endz'>" << end_z << "</span> m <br>" << endl;
    params << "Step: <span id='stepz'> " << step_z << "</span> m <br>" << endl;
    params << "Lambda: " << lambda << " m <br>" << endl;
    params << "Pixel size: " << pixel << " m <br>" << endl;
    params << "Padding: <span id='padding'>" << do_padding << "</span><br>" << endl;
    params << "Object type: <span id='type'>" << object << "</span><br>" << endl;
    params << "Min value constraint: <span id='min'>" << extremums[0] << "</span><br>" << endl;
    params << "Max value constraint: <span id='max'>" << extremums[1] << "</span><br></p>" << endl;

    params.close();

    output << "Completed" << endl;

    double e2 = getTickCount();
    double seconds = (e2 - e1) / getTickFrequency();
    cout << "\nThe program ran in " << seconds << " seconds !\n";

    output << "Elapsed time : " << seconds << " seconds" << endl;

    if (!test)
        output.close();
    else
        system("pause");

    return 0;
}