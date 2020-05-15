#include "function.h"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <time.h>

using namespace std;
using namespace cv;

int main()
{
    time_t debut_prog = time(NULL);
    
    //Reconstitution hologramme -------------------------------------------------------------------------------------------------------------------------------------------------

    string tab_img[] =
    {
        "./Bead_simulations/data_GaussBeads_Sept2019_pure_phase_FR_mag5.67e+01_z1.25e-05.tif" ,                     //0
        "./Bead_simulations/datafull_GaussBeads_Sept2019_pure_phase_FR_mag5.67e+01_z1.25e-05.tif",                  //1
        "./Bead_simulations/datalinear_GaussBeads_Sept2019_pure_phase_FR_mag5.67e+01_z1.25e-05.tif",                //2
        "./Bead_simulations/ground_truth_GaussBeads_Sept2019_pure_phase_FR_mag5.67e+01_z1.25e-05.tif",              //3
        "./Bead_simulations/real_cplx_hologram_GaussBeads_Sept2019_pure_phase_FR_mag5.67e+01_z1.25e-05-0001.tif",   //4
        "./Bead_simulations/imag_cplx_hologram_GaussBeads_Sept2019_pure_phase_FR_mag5.67e+01_z1.25e-05-0002.tif"    //5
    };

    Parametres param1_hologramme;
    param1_hologramme.z = 12.5e-6;
    param1_hologramme.mag = 56.7;
    param1_hologramme.lambda = 532e-9;
    param1_hologramme.n0 = 1.52;
    param1_hologramme.pixel_size = 4.4e-6 / param1_hologramme.mag;

            //Reconstitution simple
    /*
    Mat hologramme = imread(tab_img[0], IMREAD_UNCHANGED);

    if (hologramme.empty())
    {
        cout << "Impossible d'ouvrir ou de lire l'image\n";
        return -1;
    }

    Mat img_reconstituee;
    reconstitution(hologramme, img_reconstituee, param1_hologramme);

    affiche_image(hologramme, "Hologramme initial"); //Affichage de l'hologramme initial

    affiche_complex(img_reconstituee, img_reconstituee, CALCUL_MODULE); //On fait le module de l'image reconstituée pour qu'elle puisse être affichée
    affiche_image(img_reconstituee, "Image finale reconstituee"); //On affiche le résultat obetenu
    */

            //Reconstitution complexe
    /*
    Mat hologramme_real = imread(tab_img[4], IMREAD_UNCHANGED); //L'hologramme partie réelle
    Mat hologramme_imag = imread(tab_img[5], IMREAD_UNCHANGED); //l'hologramme partie imaginaire
    if (hologramme_real.empty() || hologramme_imag.empty())
    {
        cout << "Impossible d'ouvrir ou de lire l'image\n";
        return -1;
    }

    Mat img_reconstituee;
    reconstitution_cmplx(hologramme_real, hologramme_imag, img_reconstituee, param1_hologramme);

    affiche_complex(hologramme, hologramme, CALCUL_MODULE);
    affiche_image(hologramme, "Hologramme initial"); //Affichage de l'hologramme initial

    affiche_complex(img_reconstituee, img_reconstituee, CALCUL_MODULE); //On fait le module de l'image reconstituée pour qu'elle puisse être affichée
    affiche_image(img_reconstituee, "Image finale reconstituee"); //On affiche le résultat obetenu
    */


    //Reconstitution avec la methode "Fienup Error-Reduction algorithm" ----------------------------------------------------------------------------------------------------------------------------

    string holo[] =
    {
        "./2019_06_07_billes1mu_63_JOSAA/Basler daA1920-30um (22030948)_20190607_122251706_0012.tiff",      //Hologramme initial
        "./2019_06_07_billes1mu_63_JOSAA/Basler daA1920-30um (22030948)_20190607_122251706_0012_crop.tiff"  //Zoom sur hologramme
    };

    Parametres param2_hologramme;
    param2_hologramme.z = 7.2822e-6;
    param2_hologramme.mag = 56.7;
    param2_hologramme.lambda = 532e-9;
    param2_hologramme.n0 = 1.52;
    param2_hologramme.pixel_size = 2.2e-6 / param2_hologramme.mag;


    int nb_repetition = 10;

    //On recupère l'hologramme
    Mat hologramme = imread(holo[1], IMREAD_UNCHANGED);

    if (hologramme.empty())
    {
        cout << "Impossible d'ouvrir ou de lire l'image\n";
        return -1;
    }

    Mat img_reconstituee;
    reconstitution_etat_art(hologramme, img_reconstituee, param2_hologramme, nb_repetition);

    affiche_image(hologramme, "Image initiale");

    affiche_complex(img_reconstituee, img_reconstituee, CALCUL_PHASE);
    affiche_image(img_reconstituee, "Image finale reconstituee"); //On affiche le résultat obetenu

    cout << "\nLe programme s'est execute en ";

    time_t fin_prog = time(NULL);
    int secondes = difftime(fin_prog, debut_prog);
    if (secondes >= 60)
    {
        int minutes = secondes / 60;
        secondes = secondes % 60;
        if (minutes >= 60)
        {
            int heures = minutes / 60;
            minutes = minutes % 60;
            cout << heures << "h" << minutes << "m" << secondes << "s !\n";
        }
        else cout << minutes << "m" << secondes << "s !\n";
    }
    else cout << secondes << " secondes !\n";

    waitKey(0); // On attend l'appui sur une touche
    destroyAllWindows(); //On ferme toutes les fenetres ouvertes
    return 0;

}