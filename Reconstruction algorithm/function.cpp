#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include "function.h"

using namespace std;
using namespace cv;

void calcul_histogramme(const Mat& image_initiale, Mat& image_hist)
{
    int nb_source = 1;
    int dims = 1;
    int taille_hist = 256; //les 256 valeurs que peut prendre un pixel
    float plage_data[] = { 0, 255 }; //Chaque pixel peut prendre une valeur entre 0 et 256
    const float* plage_hist = { plage_data };
    bool uniforme = true; //La plage de valeur est uniforme, va de 1 en 1

    int graduation = 5; //Taille d'une graduation
    int largeur_hist = graduation * taille_hist;
    int hauteur_hist = 1000;
    image_hist = Mat(hauteur_hist, largeur_hist, CV_8UC3, Scalar(0, 0, 0));

    if (image_initiale.channels() == 1) //Si on a une image en niveau de gris
    {
        Mat hist_ndg;

        calcHist(&image_initiale, nb_source, 0, Mat(), hist_ndg, dims, &taille_hist, &plage_hist, uniforme, false);

        float maxi = 0;
        for (int i = 0; i < taille_hist; i++) //On cherche la plus grande valeur dans tout les histogrammes pour adapter la hauteur de l'image finale
        {
            if (hist_ndg.at<float>(i) > maxi) maxi = hist_ndg.at<float>(i);
        }
        assert(maxi != 0); //On vérifie que le max ne soit pas 0, si c'est le cas, le programme se stoppe ici

        for (int i = 1; i < taille_hist; i++) //On trace chaque ligne entre le point précédent et le point actuel pour obtenir l'histogramme
        {
            line(image_hist, Point(graduation * (i - 1), hauteur_hist * (1 - hist_ndg.at<float>(i - 1) / maxi)), Point(graduation * (i), hauteur_hist * (1 - hist_ndg.at<float>(i) / maxi)), Scalar(255, 255, 255), 2, 8, 0);
        }
    }

    else if (image_initiale.channels() == 3) //Si on a une image en couleur
    {
        //On crée une matrice pour chaque canal de couleur
        Mat r(image_initiale.rows, image_initiale.cols, CV_8UC1);
        Mat g(image_initiale.rows, image_initiale.cols, CV_8UC1);
        Mat b(image_initiale.rows, image_initiale.cols, CV_8UC1);

        Mat bgr[] = { b, g, r };

        split(image_initiale, bgr); //On sépare l'image dans ses différents cannaux

        //On réalise la même méthode que pour l'image en niveau de gris pour chaque canaux
        Mat hist_blue, hist_green, hist_red;

        calcHist(&b, nb_source, 0, Mat(), hist_blue, dims, &taille_hist, &plage_hist, uniforme, false);
        calcHist(&g, nb_source, 0, Mat(), hist_green, dims, &taille_hist, &plage_hist, uniforme, false);
        calcHist(&r, nb_source, 0, Mat(), hist_red, dims, &taille_hist, &plage_hist, uniforme, false);

        float maxi = 0;
        for (int i = 0; i < taille_hist; i++) //On cherche la plus grande valeur dans tout les histogrammes
        {
            if (hist_blue.at<float>(i) > maxi) maxi = hist_blue.at<float>(i);
            if (hist_green.at<float>(i) > maxi) maxi = hist_green.at<float>(i);
            if (hist_red.at<float>(i) > maxi) maxi = hist_red.at<float>(i);
        }
        assert(maxi != 0);

        for (int i = 1; i < taille_hist; i++)
        {
            line(image_hist, Point(graduation * (i - 1), hauteur_hist * (1 - hist_blue.at<float>(i - 1) / maxi)), Point(graduation * (i), hauteur_hist * (1 - hist_blue.at<float>(i) / maxi)), Scalar(255, 0, 0), 2, 8, 0);
            line(image_hist, Point(graduation * (i - 1), hauteur_hist * (1 - hist_green.at<float>(i - 1) / maxi)), Point(graduation * (i), hauteur_hist * (1 - hist_green.at<float>(i) / maxi)), Scalar(0, 255, 0), 2, 8, 0);
            line(image_hist, Point(graduation * (i - 1), hauteur_hist * (1 - hist_red.at<float>(i - 1) / maxi)), Point(graduation * (i), hauteur_hist * (1 - hist_red.at<float>(i) / maxi)), Scalar(0, 0, 255), 2, 8, 0);
        }
    }

    else
    {
        cout << "Erreur : l'histogramme ne paut pas être tracé\n";
    }
}

void my_multiply(const Mat& mat_simple, const Mat& mat_complexe, Mat& mat_res)
{
    Mat mat_split[2];
    split(mat_complexe, mat_split);
    multiply(mat_simple, mat_split[0], mat_split[0]);
    multiply(mat_simple, mat_split[1], mat_split[1]);
    merge(mat_split, 2, mat_res);
}

void multiplier_cmplx(const Mat& mat1, const Mat& mat2, Mat& mat_res)
{
    assert(mat1.size() == mat2.size()); //On vérifie que les 2 images ont bien la même taille

    Mat complexe1[2], complexe2[2], res[2];
    Mat temp1, temp2;
    //mat_res = Mat(mat1.size(), CV_64FC2);

    //On sépare chaque matrice facteur dans une double matrice (partie réelle et imaginaire)
    split(mat1, complexe1);
    split(mat2, complexe2);

    //(A + iB) * (X + iY) = (AX - BY) + i(AY + BX)

    //Pour le calcul de la partie réelle : Re = (AX - BY)
    multiply(complexe1[0], complexe2[0], temp1);
    multiply(complexe1[1], complexe2[1], temp2);
    res[0] = temp1 - temp2;

    //Pour le calcul de la partie imaginaire : Im = (AY + BX)
    multiply(complexe1[1], complexe2[0], temp1);
    multiply(complexe1[0], complexe2[1], temp2);
    res[1] = temp1 + temp2;

    merge(res, 2, mat_res);
}

void multiplier_cmplx(const complex<double>& scalar_cmplx, const Mat& mat_complexe, Mat& mat_res)
{
    Mat complexe[2], res[2];
    Mat temp1, temp2;

    double real = scalar_cmplx.real(), imag = scalar_cmplx.imag();

    //On sépare la matrice complexe dans une double matrice (partie réelle et imaginaire)
    split(mat_complexe, complexe);

    //(A + iB) * (X + iY) = (AX - BY) + i(AY + BX)

    //Pour le calcul de la partie réelle : Re = (AX - BY)
    multiply(real, complexe[0], temp1);
    multiply(imag, complexe[1], temp2);
    res[0] = temp1 - temp2;

    //Pour le calcul de la partie imaginaire : Im = (AY + BX)
    multiply(imag, complexe[0], temp1);
    multiply(real, complexe[1], temp2);
    res[1] = temp1 + temp2;

    merge(res, 2, mat_res);
}

void padding(const Mat& image_initiale, const int x, const int y, const int bordure, Mat& image_resize)
{
    int marge_top, marge_bottom, marge_left, marge_right;

    marge_top = floor(1.0 * (x - image_initiale.rows) / 2);
    marge_left = floor(1.0 * (y - image_initiale.cols) / 2);

    if ((x - image_initiale.rows) % 2 == 1) marge_bottom = marge_top + 1;
    else marge_bottom = marge_top;

    if ((y - image_initiale.cols) % 2 == 1) marge_right = marge_left + 1;
    else marge_right = marge_left;

    //On place l'image initiale au milieu de l'image finale avec les bonnes dimensions et rempli l'espace "vide" avec des pixels de la valeur choisie
    copyMakeBorder(image_initiale, image_resize, marge_top, marge_bottom, marge_left, marge_right, BORDER_CONSTANT, Scalar::all(bordure));
}

void optimal_TF(const Mat& image_initiale, const int bordure, Mat& image_finale)
{
    //On récupère la taille optimale pour la TF
    int nb_lignes = getOptimalDFTSize(image_initiale.rows);
    int nb_col = getOptimalDFTSize(image_initiale.cols);

    //On utilise la fonction padding pour redimensionner l'image
    padding(image_initiale, nb_lignes, nb_col, bordure, image_finale);
}

void affiche_complex(const Mat& image_initiale, Mat& image_finale, MethodeCalcul method, bool echelle_log)
{
    Mat plans[2];
    split(image_initiale, plans); // On obtient ainsi plans[0] = partie réelle, plans[1] = partie imaginaire

    Mat res_img;
    if (method == CALCUL_MODULE) magnitude(plans[0], plans[1], res_img); //On calcul le module du nombre complexe
    else if (method == CALCUL_PHASE)
    {
        phase(plans[0], plans[1], res_img); //On calcul la phase du nombre complexe
        //res_img = res_img - CV_PI; //La fonction phase() calcul un angle en 0 et 2PI, il suffit de soustraire PI pour revenir dans l'intervale [-PI, PI]
    }

    if (echelle_log) log(res_img + Scalar::all(1), image_finale); //On passe en échelle logarithmique : log(1 + module)
    else res_img.copyTo(image_finale);

    //On obtient ainsi les valeurs hautes en blanc et les valeurs basses en noir

    normalize(image_finale, image_finale, 0, 1, NORM_MINMAX);
}

void change_quadrants(const Mat& image_initiale, Mat& image_finale)
{
    // On replace l'origine du repère au centre de l'image
    int origine_x = image_initiale.cols / 2;
    int origine_y = image_initiale.rows / 2;

    image_initiale.copyTo(image_finale);

    // On crée les 4 quadrants de l'image pour les utiliser plus facilement
    Mat q0(image_finale, Rect(0, 0, origine_x, origine_y));   //En haut à gauche
    Mat q1(image_finale, Rect(origine_x, 0, origine_x, origine_y));  //En haut à droite
    Mat q2(image_finale, Rect(0, origine_y, origine_x, origine_y));  //En bas à gauche
    Mat q3(image_finale, Rect(origine_x, origine_y, origine_x, origine_y)); //En bas à droite

    //Comme on a changé de place l'origine, il faut replacer les quadrants par rapport à la nouvelle origine
    //Il faut faire une "symétrie" par rapport à la nouvelle origine
    Mat temp;
    //On échange le quadrant en haut à gauche avec celui en bas à droite
    q0.copyTo(temp);
    q3.copyTo(q0);
    temp.copyTo(q3);
    //On échange le quadrant en haut à droite avec celui en bas à gauche
    q1.copyTo(temp);
    q2.copyTo(q1);
    temp.copyTo(q2);
}

void affiche_image(const Mat& image, string nom, bool norm)
{
    namedWindow(nom, WINDOW_AUTOSIZE); // On crée la fenetre
    if (norm)
    {
        Mat temp;
        normalize(image, temp, 0, 255, NORM_MINMAX); //NORM_MINMAX
        imshow(nom, temp); // On affiche l'image dans cette fenetre
    }
    else imshow(nom, image); // On affiche l'image dans cette fenetre
}

void exp_complex(const Mat& mat_initiale, Mat& mat_finale)
{
    mat_finale = Mat(mat_initiale.size(), CV_64FC2);

    for (int i = 0; i < mat_initiale.rows; i++)
    {
        for (int j = 0; j < mat_initiale.cols; j++)
        {
            mat_finale.at<Vec2d>(i, j)[0] = cos(mat_initiale.at<double>(i, j));
            mat_finale.at<Vec2d>(i, j)[1] = sin(mat_initiale.at<double>(i, j));
        }
    }
}

void fresnel_propagator(const Parametres& param, Mat& Hz, Mat& H_z)
{
    complex<double> calc_inter;
    double k0 = 2 * param.n0 * CV_PI / param.lambda; //Constante k0

    Vec2d* Hz_temp, * H_z_temp;

    Hz = Mat(param.width, param.height, CV_64FC2);
    H_z = Mat(param.width, param.height, CV_64FC2);

    double z_shannon = MAX(param.width, param.height) * pow(param.pixel_size, 2) / param.lambda; //On définit le critère de Shannon pour utiliser la méthode la plus adaptée

    if (param.z >= z_shannon)
    {
        //Réponse impulsionnelle analytique dans le domaine spatial + FFT
        cout << "Reponse impulsionnelle\n";
        double x, y;
        
        complex<double> n0_sur_i_pi_lambda_z = param.n0 / 1i / param.lambda / param.z;

        for (int i = 0; i < param.width; i++)
        {
            x = (i - (floor(0.5 * ((double)param.width - 1)) + 1)) * param.pixel_size;
            for (int j = 0; j < param.height; j++)
            {
                y = (j - (floor(0.5 * ((double)param.height - 1)) + 1)) * param.pixel_size;

                calc_inter = exp(param.n0 * 1i * CV_PI / param.lambda / param.z * (pow(x, 2) + pow(y, 2))) * exp(1i * k0 * param.z);
               
                //Calcul de Hz (propagation)
                Hz_temp = &Hz.at<Vec2d>(i, j);
                (*Hz_temp)[0] = calc_inter.real() * n0_sur_i_pi_lambda_z.real() - calc_inter.imag() * n0_sur_i_pi_lambda_z.imag();
                (*Hz_temp)[1] = calc_inter.real() * n0_sur_i_pi_lambda_z.imag() + calc_inter.imag() * n0_sur_i_pi_lambda_z.real();

                //Calcul de H_z (rétro-propagation) : même méthode que pour Hz mais en remplaçant z par -z
                H_z_temp = &H_z.at<Vec2d>(i, j);
                (*H_z_temp)[0] = calc_inter.real() * (-n0_sur_i_pi_lambda_z.real()) - calc_inter.imag() * n0_sur_i_pi_lambda_z.imag();
                (*H_z_temp)[1] = calc_inter.real() * (-n0_sur_i_pi_lambda_z.imag()) + calc_inter.imag() * n0_sur_i_pi_lambda_z.real();
            }
        }
        double pixel_size_carre = pow(param.pixel_size, 2);

        change_quadrants(Hz, Hz);
        dft(Hz, Hz, DFT_COMPLEX_OUTPUT);
        multiply(pixel_size_carre, Hz, Hz);

        change_quadrants(H_z, H_z);
        dft(H_z, H_z, DFT_COMPLEX_OUTPUT);
        multiply(pixel_size_carre, H_z, H_z);
    }
    else
    {
        //Fonction de transfert analytique dans le domaine de Fourier
        cout << "Fonction de transfert\n";
        double u, v;
        complex<double> i_pi_lambda_z = -1i * CV_PI * param.lambda * param.z / param.n0;

        for (int i = 0; i < param.width; i++)
        {
            u = (i - (floor(0.5 * ((double)param.width - 1)) + 1)) / param.width / param.pixel_size;
            for (int j = 0; j < param.height; j++)
            {
                v = (j - (floor(0.5 * ((double)param.height - 1)) + 1)) / param.height / param.pixel_size;

                //Calcul de Hz (propagation)

                calc_inter = exp(i_pi_lambda_z * (pow(u, 2) + pow(v, 2))) * exp(1i * k0 * param.z);

                Hz_temp = &Hz.at<Vec2d>(i, j);
                (*Hz_temp)[0] = calc_inter.real();
                (*Hz_temp)[1] = calc_inter.imag();

                
                //Calcul de H_z (rétro-propagation) : même méthode que pour Hz mais en remplaçant z par -z

                H_z_temp = &H_z.at<Vec2d>(i, j);
                (*H_z_temp)[0] = calc_inter.real();
                (*H_z_temp)[1] = -calc_inter.imag();
            }
        }
        change_quadrants(Hz, Hz);
        change_quadrants(H_z, H_z);
    }
}

void kernel_propagation_fresnel(const Parametres& param, Mat& Hz, Mat& H_z, bool flag_phaseref, TYPE_HOLOGRAM type_hologram, bool flag_linearize, TYPE_OBJ type_obj)
{
    KERNEL_TYPE type_kernel = HZ;

    if (type_hologram == INTENSITE && flag_linearize)
    {
        flag_phaseref = false;
        type_kernel = GZ_DEPHASING;

        if (type_obj == ABSORBING) type_kernel = GZ_ABSORBING;
    }

    complex<double> calc_inter;
    double k0 = 2 * param.n0 * CV_PI / param.lambda; //Constante k0
    Vec2d* temp;
    Hz = Mat(param.width, param.height, CV_64FC2);
    Mat mat_split[2];

    double z_shannon = MAX(param.width, param.height) * pow(param.pixel_size, 2) / param.lambda; //On définit le critère de Shannon pour utiliser la méthode la plus adaptée

    if (param.z >= z_shannon)
    {
        //Réponse impulsionnelle analytique dans le domaine spatial + FFT
        cout << "Reponse impulsionnelle\n";

        double x, y;
        complex<double> n0_sur_i_lambda_z = param.n0 / 1i / param.lambda / param.z;

        //Mat hz(param.width, param.height, CV_64FC2);

        for (int i = 0; i < param.width; i++)
        {
            x = (i - (floor(0.5 * ((double)param.width - 1)) + 1)) * param.pixel_size;
            for (int j = 0; j < param.height; j++)
            {
                y = (j - (floor(0.5 * ((double)param.height - 1)) + 1)) * param.pixel_size;

                calc_inter = exp(param.n0 * 1i * CV_PI / param.lambda / param.z * (pow(x, 2) + pow(y, 2)));

                temp = &Hz.at<Vec2d>(i, j);
                (*temp)[0] = calc_inter.real();
                (*temp)[1] = calc_inter.imag();
            }
        }
        multiplier_cmplx(n0_sur_i_lambda_z, Hz, Hz);

        if (flag_phaseref)
        {
            calc_inter = exp(1i * k0 * param.z);
            multiplier_cmplx(calc_inter, Hz, Hz);
        }

        if (type_kernel != HZ)
        {
            split(Hz, mat_split);
            if (type_kernel == GZ_DEPHASING)
            {
                mat_split[0] = 0;
                mat_split[1] = -2 * mat_split[1];
            }
            if (type_kernel == GZ_ABSORBING)
            {
                mat_split[0] = 2 * mat_split[0];
                mat_split[1] = 0;
            }
            merge(mat_split, 2, Hz);
        }

        change_quadrants(Hz, Hz);
        dft(Hz, Hz, DFT_COMPLEX_OUTPUT);
        multiply(pow(param.pixel_size, 2), Hz, Hz);

        split(Hz, mat_split);
        mat_split[1] = -mat_split[1];
        merge(mat_split, 2, H_z);
    }
    else
    {
        //Fonction de transfert analytique dans le domaine de Fourier
        cout << "Fonction de transfert\n";

        double u, v;
        complex<double> i_pi_lambda_z = -1i * CV_PI * param.lambda * param.z / param.n0;

        for (int i = 0; i < param.width; i++)
        {
            u = (i - (floor(0.5 * ((double)param.width - 1)) + 1)) / param.width / param.pixel_size;
            for (int j = 0; j < param.height; j++)
            {
                v = (j - (floor(0.5 * ((double)param.height - 1)) + 1)) / param.height / param.pixel_size;

                calc_inter = exp(i_pi_lambda_z * (pow(u, 2) + pow(v, 2)));

                temp = &Hz.at<Vec2d>(i, j);
                (*temp)[0] = calc_inter.real();
                (*temp)[1] = calc_inter.imag();
            }
        }
        change_quadrants(Hz, Hz);

        if (flag_phaseref)
        {
            calc_inter = exp(1i * k0 * param.z);
            multiplier_cmplx(calc_inter, Hz, Hz);
        }

        if (type_kernel != HZ)
        {
            split(Hz, mat_split);
            if (type_kernel == GZ_DEPHASING)
            {
                mat_split[0] = 0;
                mat_split[1] = -2 * mat_split[1];
            }
            if (type_kernel == GZ_ABSORBING)
            {
                mat_split[0] = 2 * mat_split[0];
                mat_split[1] = 0;
            }
            merge(mat_split, 2, Hz);
        }

        change_quadrants(Hz, Hz);
        dft(Hz, Hz, DFT_COMPLEX_OUTPUT);
        multiply(pow(param.pixel_size, 2), Hz, Hz);

        split(Hz, mat_split);
        mat_split[1] = -mat_split[1];
        merge(mat_split, 2, H_z);
    }
}

void reconstitution(const Mat& hologramme, Mat& img_reconstituee, Parametres& param, int bordure)
{
    int taille_initiale[] = { hologramme.rows, hologramme.cols };

    hologramme.convertTo(hologramme, CV_64F);
    sqrt(hologramme, hologramme); //On fait la racine carré de l'hologramme, car les données récupérées sont le carré du module

    Mat hologramme_optimal;
    padding(hologramme, taille_initiale[0] * 2, taille_initiale[1] * 2, bordure, hologramme_optimal); //On double la taille de l'image pour éviter le repliement

    optimal_TF(hologramme_optimal, bordure, hologramme_optimal);

    int taille_finale[] = { hologramme_optimal.rows, hologramme_optimal.cols };

    //On modifie les paramètres de la taille
    param.width = taille_finale[0];
    param.height = taille_finale[1];

    hologramme_optimal.convertTo(hologramme_optimal, CV_64FC1);

    Mat Hz, H_z;
    fresnel_propagator(param, Hz, H_z); //On récupère les 2 kernel en fonction des paramètres ci-dessus

    Mat fourier_hologramme;
    dft(hologramme_optimal, fourier_hologramme, DFT_COMPLEX_OUTPUT); //On réalise la transformée de Fourier de l'hologramme

    Mat produit;
    multiplier_cmplx(fourier_hologramme, H_z, produit); //On multiplie le TF avec le kernel (entrée et sortie complexes)

    Mat TF_inverse;
    idft(produit, TF_inverse, DFT_COMPLEX_OUTPUT); //On réalise la transformée de Fourier inverse du produit

    //On reduit l'image reconstituée pour ne garder que la partie centrale qui correspond au résultat
    int marge_rows = floor(1.0 * (taille_finale[0] - taille_initiale[0]) / 2), marge_cols = floor(1.0 * (taille_finale[1] - taille_initiale[1]) / 2);
    Mat(TF_inverse, Rect(marge_rows, marge_cols, taille_initiale[0], taille_initiale[1])).copyTo(img_reconstituee);

}

void reconstitution_cmplx(const Mat& hologramme_real, const Mat& hologramme_imag, Mat& img_reconstituee, Parametres& param, int bordure_Re, int bordure_Im)
{
    //Pour la reconstitution d'un hologramme complexe, c'est exactement la même méthode qu'un hologramme simple

    Mat hologramme_cmplx[] = { hologramme_real, hologramme_imag }; //On crée l'hologramme complexe sous forme Re + i*Im

    Mat hologramme;
    merge(hologramme_cmplx, 2, hologramme);

    int taille_initiale[] = { hologramme.rows, hologramme.cols };

    Mat hologramme_optimal_cmplx[2];

    //On multiplie par 2 la taille de l'hologramme pour éviter le repliement
    padding(hologramme_cmplx[0], taille_initiale[0] * 2, taille_initiale[1] * 2, bordure_Re, hologramme_optimal_cmplx[0]);
    optimal_TF(hologramme_optimal_cmplx[0], bordure_Re, hologramme_optimal_cmplx[0]);

    padding(hologramme_cmplx[1], taille_initiale[0] * 2, taille_initiale[1] * 2, bordure_Im, hologramme_optimal_cmplx[1]);
    optimal_TF(hologramme_optimal_cmplx[1], bordure_Im, hologramme_optimal_cmplx[1]);

    Mat hologramme_optimal;
    merge(hologramme_optimal_cmplx, 2, hologramme_optimal);

    int taille_finale[] = { hologramme_optimal.rows, hologramme_optimal.cols };

    //On modifie les paramètres de la taille
    param.width = taille_finale[0];
    param.height = taille_finale[1];

    hologramme_optimal.convertTo(hologramme_optimal, CV_64FC2);

    Mat Hz, H_z;
    fresnel_propagator(param, Hz, H_z); //On récupère les 2 kernel en fonction des paramètres ci-dessus

    Mat fourier_hologramme;
    dft(hologramme_optimal, fourier_hologramme, DFT_COMPLEX_INPUT | DFT_COMPLEX_OUTPUT); //On réalise la transformée de Fourier de l'hologramme (dans ce cas, l'entrée et la sortie sont complexes)

    Mat produit;
    multiplier_cmplx(fourier_hologramme, H_z, produit); //On multiplie le TF avec le kernel (entrée et sortie complexes)

    Mat TF_inverse;
    idft(produit, TF_inverse, DFT_COMPLEX_OUTPUT); //On réalise la transformée de Fourier inverse du produit

    //On reduit l'image reconstituée pour ne garder que la partie centrale qui correspond au résultat
    int marge_rows = floor(1.0 * (taille_finale[0] - taille_initiale[0]) / 2), marge_cols = floor(1.0 * (taille_finale[1] - taille_initiale[1]) / 2);
    Mat(TF_inverse, Rect(marge_rows, marge_cols, taille_initiale[0], taille_initiale[1])).copyTo(img_reconstituee);
}

void reconstitution_fienup(const Mat& hologramme, Mat& img_reconstituee, Parametres& param, int nb_repetition)
{
    double e1, e2, secondes;

    Mat temp;
    hologramme.convertTo(temp, CV_64F);
    sqrt(temp, temp); //On fait la racine carré de l'hologramme, car les données récupérées sont le carré du module

    int taille_initiale[] = { hologramme.rows, hologramme.cols };

    Scalar moyenne = mean(temp); //On calcule la moyenne de l'image
    temp /= moyenne; //On normalise l'image en la divisant par sa moyenne
    int bordure = 1; //On met une bordure de 1 car on a normalisé en divisant par la moyenne

    Mat hologramme_optimal;
    padding(temp, taille_initiale[0] * 2, taille_initiale[1] * 2, bordure, hologramme_optimal); //On double la taille de l'image pour éviter le repliement
    optimal_TF(hologramme_optimal, bordure, hologramme_optimal);

    int taille_finale[] = { hologramme_optimal.rows, hologramme_optimal.cols };

    //On modifie les paramètres de la taille
    param.width = taille_finale[0];
    param.height = taille_finale[1];

    Mat Hz, H_z;
    e1 = getTickCount();
    //fresnel_propagator(param, Hz, H_z); //On récupère les 2 kernel en fonction des paramètres ci-dessus
    kernel_propagation_fresnel(param, Hz, H_z);

    e2 = getTickCount();
    secondes = (e2 - e1) / getTickFrequency();
    cout << "Fresnel : " << secondes << " secondes\n";

    Mat hologramme_iteration; //La matrice qui sera récupérée et modifiée à chaque itération
    hologramme_optimal.copyTo(hologramme_iteration);

    Mat TF, produit_TF, TF_inverse; //Matrice pour Fourier
    Mat mat_split[2], module1;

    for (int i = 0; i < nb_repetition; i++)
    {
        e1 = getTickCount();
        //Retro - propagation
        dft(hologramme_iteration, TF, DFT_COMPLEX_OUTPUT);
        multiplier_cmplx(TF, H_z, produit_TF);
        idft(produit_TF, TF_inverse, DFT_COMPLEX_OUTPUT);

        //On recrée une image en passant le module à 1 : module1 = TF_inverse / abs(TF_inverse) avec partie réelle et partie imaginaire positives
        split(TF_inverse, mat_split);
        max(0, mat_split[0], mat_split[0]); //On force la partie réelle positive
        max(0, mat_split[1], mat_split[1]); //On force la partie imaginaire positive
        magnitude(mat_split[0], mat_split[1], temp); //Calcul du module
        divide(1, temp, temp);
        merge(mat_split, 2, TF_inverse);
        my_multiply(temp, TF_inverse, module1);

        //Propagation
        dft(module1, TF, DFT_COMPLEX_OUTPUT);
        multiplier_cmplx(TF, Hz, produit_TF);
        idft(produit_TF, TF_inverse, DFT_COMPLEX_OUTPUT);

        //On remet les données initiales de l'hologramme : hologramme_iteration = hologramme_optimal * TF_inverse / abs(TF_inverse)
        split(TF_inverse, mat_split);
        magnitude(mat_split[0], mat_split[1], temp); //Calcul du module
        divide(hologramme_optimal, temp, temp);
        my_multiply(temp, TF_inverse, hologramme_iteration);

        e2 = getTickCount();
        secondes = (e2 - e1) / getTickFrequency();
        cout << "itération " << i << " : " << secondes << " secondes\n";
    }

    //On reduit l'image reconstituée pour ne garder que la partie centrale qui correspond au résultat
    int marge_rows = floor(1.0 * (taille_finale[0] - taille_initiale[0]) / 2), marge_cols = floor(1.0 * (taille_finale[1] - taille_initiale[1]) / 2);
    Mat(module1, Rect(marge_rows, marge_cols, taille_initiale[0], taille_initiale[1])).copyTo(img_reconstituee);
}

