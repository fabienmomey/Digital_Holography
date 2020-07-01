
///
/// \file algorithms.cpp 
/// 
/// \author Yoann MASSET - Fabien MOMEY
/// \version Final version
/// \date June 20th 2020
/// 
/// \brief File grouping the definitions of reconstruction algorithm functions
/// 

#include "tools.h"
#include "algorithms.h"

#include <iostream>
#include <opencv2/opencv.hpp> 
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

#pragma region Simple Backpropagation

void simple_backpropagation(const Mat& optimal_hologram, Mat& reconstituted_image, Settings& setting)
{
    Mat Hz, H_z; //Kernels
    Mat FT, FT_product, inverse_FT; //Fourier matrix
    int final_size[] = { optimal_hologram.rows, optimal_hologram.cols };

    get_fresnel_propagator(setting, Hz, H_z); //We recover the 2 kernel according to the above parameters

    dft(optimal_hologram, FT, DFT_COMPLEX_OUTPUT);
    complex_multiply(FT, H_z, FT_product);
    idft(FT_product, inverse_FT, DFT_COMPLEX_OUTPUT);

    reconstituted_image = inverse_FT / (final_size[0] * final_size[1]);
}

#pragma endregion


#pragma region Fienup Algorithm

void fienup_algorithm(const Mat& optimal_hologram, Mat& reconstituted_image, Settings& setting, int object, double complex_extremum[], int repetitions)
{
    double e1, e2, seconds;

    int final_size[] = { optimal_hologram.rows, optimal_hologram.cols };

    Mat Hz, H_z;
    e1 = getTickCount();

    get_fresnel_propagator(setting, Hz, H_z); //We recover the 2 kernel according to the above parameters

    e2 = getTickCount();
    seconds = (e2 - e1) / getTickFrequency();
    cout << "Fresnel : " << seconds << " seconds\n";

    Mat hologram_iteration; //The matrix that will be retrieved and modified at each iteration
    optimal_hologram.copyTo(hologram_iteration);

    Mat FT, FT_product, inverse_FT; //Fourier matrix
    Mat mat_split[2], mat_end, temp;

    for (int i = 0; i < repetitions; i++)
    {
        e1 = getTickCount();

        //Backpropagation
        dft(hologram_iteration, FT, DFT_COMPLEX_OUTPUT);
        complex_multiply(FT, H_z, FT_product);
        idft(FT_product, inverse_FT, DFT_COMPLEX_OUTPUT);

        inverse_FT = inverse_FT / (final_size[0] * final_size[1]);

        //We recreate an image by applying the constraint
        split(inverse_FT, mat_split);
        if (object == 0)
        {
            //If we have phase object, we set the real part to 1 and apply the constraint on imaginary part
            mat_split[0] = max(mat_split[0], 1); //Set the real part to 1
            mat_split[0] = min(mat_split[0], 1); //Set the real part to 1
            mat_split[1] = max(mat_split[1], complex_extremum[0]); //The imaginary part is compared with the lower limit of the interval (min)
            mat_split[1] = min(mat_split[1], complex_extremum[1]); //The imaginary part is compared with the upper limit of the interval (max)
        }
        else
        {
            //If we have absorbing object, we set the imaginary part to 0 and apply the constraint on real part
            mat_split[0] = max(mat_split[0], complex_extremum[0]); //The real part is compared with the lower limit of the interval (min)
            mat_split[0] = min(mat_split[0], complex_extremum[1]); //The real part is compared with the upper limit of the interval (max)
            mat_split[1] = max(mat_split[1], 0); //Set the imaginary part to 0
            mat_split[1] = min(mat_split[1], 0); //Set the imaginary part to 0
        }
        merge(mat_split, 2, mat_end);

        //Propagation
        dft(mat_end, FT, DFT_COMPLEX_OUTPUT);
        complex_multiply(FT, Hz, FT_product);
        idft(FT_product, inverse_FT, DFT_COMPLEX_OUTPUT);

        //The initial data of the hologram are reset: hologram_iteration = optimal_hologram * inverse_FT / abs(inverse_FT)
        split(inverse_FT, mat_split);
        magnitude(mat_split[0], mat_split[1], temp); //Calcul of the module
        divide(optimal_hologram, temp, temp);
        my_multiply(temp, inverse_FT, hologram_iteration);

        e2 = getTickCount();
        seconds = (e2 - e1) / getTickFrequency();
        cout << "Iteration " << i << " : " << seconds << " seconds\n";
    }
    mat_end.copyTo(reconstituted_image);
}

#pragma endregion

#pragma region Regularised Inversion Algorithms

void RI_algorithme_ISTA(const Mat& optimal_hologram, Mat& reconstituted_image, Settings& setting, int object, double extremum[], int repetitions)
{
    int final_size[] = { optimal_hologram.rows, optimal_hologram.cols };

    OBJ_TYPE type_object; //Select the good object type
    if (object == 0) type_object = DEPHASING;
    else type_object = ABSORBING;

    Mat Hz, H_z;
    get_fresnel_propagator(setting, Hz, H_z, true, INTENSITY, true, type_object); //We recover the 2 kernel according to the above parameters

    Mat mat_split[2], Gz, temp;
    split(Hz, mat_split);
    mat_split[0].copyTo(Gz); //To have a real kernel with just the real part because imaginary part is set to 0 in this method
    //With the ISTA method, the propagation and back propagation kernel are the same kernel

    Mat hologram_iteration(optimal_hologram.size(), CV_64F, Scalar(0));

    double max_value;
    multiply(Gz, Gz, temp);
    minMaxIdx(temp, NULL, &max_value); //Take the max of Gz²

    long double c; //Intensity factor
    double t = 1 / (2 * max_value);
    double mu = 1;

    Mat FT, FT_product, inverse_FT;
    Mat m_tild; //Matrix with the direct model
    Mat r; //Matrix with the residues : the difference between the model and the data

    for (int i = 0; i < repetitions; i++)
    {
        cout << i << " ";

        //Calcul of the direct model with propagation
        dft(hologram_iteration, FT, DFT_COMPLEX_OUTPUT);
        my_multiply(Gz, FT, FT_product);
        idft(FT_product, inverse_FT, DFT_REAL_OUTPUT);

        inverse_FT = inverse_FT / (final_size[0] * final_size[1]);

        m_tild = 1 + inverse_FT;


        // Calcul of the optimal c
        multiply(m_tild, optimal_hologram, temp);
        c = sum(temp)[0];
        multiply(m_tild, m_tild, temp);
        c = c * 1.0 / sum(temp)[0];


        r = c * m_tild - optimal_hologram; //Calcul of the residues

        //Back propagation of the residues
        dft(r, FT, DFT_COMPLEX_OUTPUT);
        my_multiply(Gz, FT, FT_product);
        idft(FT_product, inverse_FT, DFT_REAL_OUTPUT);

        r = inverse_FT / (final_size[0] * final_size[1]);


        hologram_iteration = hologram_iteration - 2 * t * c * r; //Soft-thresholding


        //Apply the constraint
        hologram_iteration = max(hologram_iteration, extremum[0]); //min
        hologram_iteration = min(hologram_iteration, extremum[1]); //max
    }
    cout << endl;
    hologram_iteration.copyTo(reconstituted_image);
}

void RI_algorithme_FISTA(const Mat& optimal_hologram, Mat& reconstituted_image, Settings& setting, int object, double extremum[], int repetitions)
{
    int final_size[] = { optimal_hologram.rows, optimal_hologram.cols };

    OBJ_TYPE type_object; //Select the good object type
    if (object == 0) type_object = DEPHASING;
    else type_object = ABSORBING;

    Mat Hz, H_z;
    get_fresnel_propagator(setting, Hz, H_z, true, INTENSITY, true, type_object); //We recover the 2 kernel according to the above parameters

    Mat mat_split[2], Gz, temp;
    split(Hz, mat_split);
    mat_split[0].copyTo(Gz);//To have a real kernel with just the real part because imaginary part is set to 0 in this method
    //With the FISTA method, the propagation and back propagation kernel are the same kernel

    Mat hologram_iteration_prev(optimal_hologram.size(), CV_64F, Scalar(0)), hologram_iteration;
    Mat u; //Intermediate matrix
    hologram_iteration_prev.copyTo(u);
    double s_prev = 1, s;

    double max_value;
    multiply(Gz, Gz, temp);
    minMaxIdx(temp, NULL, &max_value); //Take the max of Gz²

    long double c; //Intensity factor
    double t = 1 / (2 * max_value);
    double mu = 1;

    Mat FT, FT_product, inverse_FT;
    Mat m_tild; //Matrix with the direct model
    Mat r; //Matrix with the residues : the difference between the model and the data

    for (int i = 0; i < repetitions; i++)
    {
        cout << i << " ";

        //Calcul of the direct model with propagation
        dft(u, FT, DFT_COMPLEX_OUTPUT);
        my_multiply(Gz, FT, FT_product);
        idft(FT_product, inverse_FT, DFT_REAL_OUTPUT);

        inverse_FT = inverse_FT / (final_size[0] * final_size[1]);

        m_tild = 1 + inverse_FT;


        // Calcul of the optimal c
        multiply(m_tild, optimal_hologram, temp);
        c = sum(temp)[0];
        multiply(m_tild, m_tild, temp);
        c = c * 1.0 / sum(temp)[0];


        r = c * m_tild - optimal_hologram; //Calcul of the residues

        //Back propagation of the residues
        dft(r, FT, DFT_COMPLEX_OUTPUT);
        my_multiply(Gz, FT, FT_product);
        idft(FT_product, inverse_FT, DFT_REAL_OUTPUT);

        r = inverse_FT / (final_size[0] * final_size[1]);


        hologram_iteration = u - 2 * t * c * r; //Soft-thresholding

        //Apply the constraint
        hologram_iteration = max(hologram_iteration, extremum[0]); //min
        hologram_iteration = min(hologram_iteration, extremum[1]); //max

        //Calcul of the new value and matrix for the next iteration
        s = 0.5 * (1 + sqrt(1 + 4 * s_prev * s_prev));
        u = hologram_iteration + (s_prev - 1) / s * (hologram_iteration - hologram_iteration_prev);

        s_prev = s;
        hologram_iteration.copyTo(hologram_iteration_prev);
    }
    cout << endl;
    hologram_iteration.copyTo(reconstituted_image);
}

#pragma endregion
