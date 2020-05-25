
#include "fonction.h"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

int main()
{
    unsigned long long int e1 = getTickCount();
    cout << "Hello World!\n";

    //Hologram reconstitution -------------------------------------------------------------------------------------------------------------------------------------------------

    string string_table[] =
    {
        "./Bead_simulations/data_GaussBeads_Sept2019_pure_phase_FR_mag5.67e+01_z1.25e-05.tif" ,                     //0
        "./Bead_simulations/datafull_GaussBeads_Sept2019_pure_phase_FR_mag5.67e+01_z1.25e-05.tif",                  //1
        "./Bead_simulations/datalinear_GaussBeads_Sept2019_pure_phase_FR_mag5.67e+01_z1.25e-05.tif",                //2
        "./Bead_simulations/ground_truth_GaussBeads_Sept2019_pure_phase_FR_mag5.67e+01_z1.25e-05.tif",              //3
        "./Bead_simulations/real_cplx_hologram_GaussBeads_Sept2019_pure_phase_FR_mag5.67e+01_z1.25e-05-0001.tif",   //4
        "./Bead_simulations/imag_cplx_hologram_GaussBeads_Sept2019_pure_phase_FR_mag5.67e+01_z1.25e-05-0002.tif"    //5
    };

    Settings hologram_setting1;
    hologram_setting1.z = 12.5e-6;
    hologram_setting1.mag = 56.7;
    hologram_setting1.lambda = 532e-9;
    hologram_setting1.n0 = 1.52;
    hologram_setting1.pixel_size = 4.4e-6 / hologram_setting1.mag;

    //Reconstruction with the "Fienup" method -------------------------------------------------------------------------------------------------------------------------------------------

    string hologram_path[] =
    {
        "./2019_06_07_billes1mu_63_JOSAA/Basler daA1920-30um (22030948)_20190607_122251706_0012.tiff",      //Initial hologram
        "./2019_06_07_billes1mu_63_JOSAA/Basler daA1920-30um (22030948)_20190607_122251706_0012_crop.tiff"  //Zoom on the hologram
    };

    Settings hologram_setting2;
    hologram_setting2.z = 7.2822e-6;
    hologram_setting2.mag = 56.7;
    hologram_setting2.lambda = 532e-9;
    hologram_setting2.n0 = 1.52;
    hologram_setting2.pixel_size = 2.2e-6 / hologram_setting2.mag;


    int repetitions = 10;

    Mat hologram = imread(hologram_path[1], IMREAD_UNCHANGED);

    if (hologram.empty())
    {
        cout << "Cannot open or read the image\n";
        return -1;
    }

    Mat reconstitued_image;
    fienup_reconstitution(hologram, reconstitued_image, hologram_setting2, repetitions);

    image_display(hologram, "Initial image");

    complex_display(reconstitued_image, reconstitued_image, PHASE_CALCUL);
    image_display(reconstitued_image, "Reconstitued image"); //The result is displayed


    cout << "\nThe program ran in ";

    unsigned long long int e2 = getTickCount();
    unsigned long long int seconds = (e2 - e1) / getTickFrequency();
    if (seconds >= 60)
    {
        int minutes = seconds / 60;
        seconds = seconds % 60;
        if (minutes >= 60)
        {
            int hours = minutes / 60;
            minutes = minutes % 60;
            cout << hours << "h" << minutes << "m" << seconds << "s !\n";
        }
        else cout << minutes << "m" << seconds << "s !\n";
    }
    else cout << seconds << " seconds !\n";

    waitKey(0); //Waiting for a key to be pressed
    destroyAllWindows(); //We're closing all the open windows
    return 0;
}