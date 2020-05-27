
#include "function.h"

#include <iostream>
//#include <stdio.h>
//#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{

    //Hologram reconstruction with the "Fienup" method -------------------------------------------------------------------------------------------------------------------------------------------

    double e1 = getTickCount();
    
    string source_file, dest_files;
    double start_z, end_z, step_z, lambda, pixel;
    int do_padding;
    
    //If all parameters are filled in
    if (argc >= 9)
    {
        source_file = argv[1];
        dest_files = argv[2];
        start_z = atof(argv[3]);
        end_z = atof(argv[4]);
        step_z = atof(argv[5]);
        lambda = atof(argv[6]);
        pixel = atof(argv[7]);
        do_padding = atoi(argv[8]);
    }
    //Else : load default parameters
    else
    {
        source_file = "2019_06_07_billes1mu_63_JOSAA/Basler daA1920-30um (22030948)_20190607_122251706_0012_crop.tiff";
        dest_files = "output/propagation/";
        start_z = 5e-6;
        end_z = 10e-6;
        step_z = 0.5e-6;
        lambda = 532e-9;
        pixel = 3.88e-8;
        do_padding = 1;
    }
    
    //The initial hologram 
    Mat hologram = imread(source_file, IMREAD_UNCHANGED);
    if (hologram.empty())
    {
        cout << "Cannot open or read the image\n";
        return -1;
    }

    Settings param;
    param.lambda = lambda;
    param.pixel_size = pixel;

    param.mag = 56.7;
    param.n0 = 1.52;
    
    /*
    CalculMethod calcul_method;
    if (argv[9] == 1) calcul_method = PHASE_CALCUL;
    else calcul_method = MODULE_CALCUL;
    */

    int repetitions = 5;
    Mat reconstitued_image;
    string dest_name;
    
    int i = 1;

    //We reconstitute the hologram for each z value between start_zand end_z with a step_z between each value

    for (double z = start_z; z <= end_z; z+=step_z)
    {
        param.z = z;
        fienup_reconstitution(hologram, reconstitued_image, param, repetitions, do_padding); //Hologram reconstitution
        complex_display(reconstitued_image, reconstitued_image, PHASE_CALCUL); //Converting the final complex image into a displayable image by calculating its phase
        normalize(reconstitued_image, reconstitued_image, 0, 65535, NORM_MINMAX); //Normalizing the image so that it can be registered under an extension on 16 bits
        reconstitued_image.convertTo(reconstitued_image, CV_16U);
        dest_name = dest_files + "Reconstitued_image_" + to_string(i) + ".png";
        imwrite(dest_name, reconstitued_image); //Save the final_image
        i++;
    }

    double e2 = getTickCount();
    double seconds = (e2 - e1) / getTickFrequency();
    cout << "\nThe program ran in " << seconds << " seconds !\n";

    return 0;
}