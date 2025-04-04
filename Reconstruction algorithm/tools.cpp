
///
/// \file tools.cpp
/// 
/// \author Yoann MASSET - Fabien MOMEY
/// \version Final version
/// \date June 20th 2020
/// 
/// \brief File grouping the definitions of tools useful for image reconstruction 
/// 

#include "tools.h"

#include <iostream>
#include <opencv2/opencv.hpp> 
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

#pragma region Matrix Operations

void my_multiply(const Mat& simple_mat, const Mat& complex_mat, Mat& res_mat)
{
    Mat mat_split[2];
    split(complex_mat, mat_split); //We separate the real from the imaginary 
    //M * (A+iB) = MA + iMB 
    multiply(simple_mat, mat_split[0], mat_split[0]);
    multiply(simple_mat, mat_split[1], mat_split[1]);
    merge(mat_split, 2, res_mat);
}

void complex_multiply(const Mat& mat1, const Mat& mat2, Mat& res_mat)
{
    assert(mat1.size() == mat2.size()); //We check that both images are the same size

    Mat complex1[2], complex2[2], res[2];
    Mat temp1, temp2;

    //We separate each factor matrix into a double matrix (real and imaginary part)
    split(mat1, complex1);
    split(mat2, complex2);

    //(A + iB) * (X + iY) = (AX - BY) + i(AY + BX)

    //For the calcul of the real part: Re = (AX - BY)
    multiply(complex1[0], complex2[0], temp1);
    multiply(complex1[1], complex2[1], temp2);
    res[0] = temp1 - temp2;

    //For the calcul of the imaginary part : Im = (AY + BX)
    multiply(complex1[1], complex2[0], temp1);
    multiply(complex1[0], complex2[1], temp2);
    res[1] = temp1 + temp2;

    merge(res, 2, res_mat);
}

void complex_multiply(const complex<double>& complex_scalar, const Mat& complex_mat, Mat& res_mat)
{
    Mat complex[2], res[2];
    Mat temp1, temp2;

    double real = complex_scalar.real(), imag = complex_scalar.imag();

    //We separate the complex matrix into a double matrix (real and imaginary part)
    split(complex_mat, complex);

    //(A + iB) * (X + iY) = (AX - BY) + i(AY + BX)

    //For the calcul of the real part: Re = (AX - BY)
    multiply(real, complex[0], temp1);
    multiply(imag, complex[1], temp2);
    res[0] = temp1 - temp2;

    //For the calcul of the imaginary part : Im = (AY + BX)
    multiply(imag, complex[0], temp1);
    multiply(real, complex[1], temp2);
    res[1] = temp1 + temp2;

    merge(res, 2, res_mat);
}

void padding(const Mat& initial_mat, const int x, const int y, const int pad, Mat& resized_mat)
{
    int top_margin, bottom_margin, left_margin, right_margin;

    //We wish to have the same margin all around (to the nearest 1)

    top_margin = floor(1.0 * (x - initial_mat.rows) / 2);
    left_margin = floor(1.0 * (y - initial_mat.cols) / 2);

    if ((x - initial_mat.rows) % 2 == 1) bottom_margin = top_margin + 1;
    else bottom_margin = top_margin;

    if ((y - initial_mat.cols) % 2 == 1) right_margin = left_margin + 1;
    else right_margin = left_margin;

    //We place the initial image in the middle of the final image with the right dimensions and fill the "empty" space with pixels of the chosen value
    copyMakeBorder(initial_mat, resized_mat, top_margin, bottom_margin, left_margin, right_margin, BORDER_CONSTANT, Scalar::all(pad));
}

void complex_display(const Mat& initial_mat, Mat& final_mat, CalculMethod method, bool log_scale)
{
    Mat plane[2];
    split(initial_mat, plane); //We thus obtain planes [0] = real part, planes [1] = imaginary part

    Mat res_image;
    if (method == MODULE_CALCUL) magnitude(plane[0], plane[1], res_image); //We calculate the module of the complex number
    else if (method == PHASE_CALCUL) phase(plane[0], plane[1], res_image); //We calculate the phase of the complex number

    if (log_scale) log(res_image + Scalar::all(1), final_mat); //We switch to logarithmic scale: log(1 + module)
    else res_image.copyTo(final_mat);

    normalize(final_mat, final_mat, 0, 1, NORM_MINMAX);
}

#pragma endregion

#pragma region Fourier Transform Tools

void optimal_FT(const Mat& initial_mat, const int pad, Mat& final_mat)
{
    //We get the optimal size for the Fourier transform
    int row_number = getOptimalDFTSize(initial_mat.rows);
    int col_number = getOptimalDFTSize(initial_mat.cols);

    //The padding function is used to resize the image
    padding(initial_mat, row_number, col_number, pad, final_mat);
}

void FT_shift(const Mat& initial_mat, Mat& final_mat)
{
    //The origin of the marker is placed in the center of the image
    int origin_x = initial_mat.cols / 2;
    int origin_y = initial_mat.rows / 2;

    initial_mat.copyTo(final_mat);

    //We create the 4 quadrants of the image to use them more easily
    Mat q0(final_mat, Rect(0, 0, origin_x, origin_y));   //Top left
    Mat q1(final_mat, Rect(origin_x, 0, origin_x, origin_y));  //Top right
    Mat q2(final_mat, Rect(0, origin_y, origin_x, origin_y));  //Bottom left
    Mat q3(final_mat, Rect(origin_x, origin_y, origin_x, origin_y)); //Bottom right

    //Since the origin has been moved, the quadrants must be repositioned in relation to the new origin
    //We have to make a "symmetry" in relation to the new origin
    Mat temp;
    //Swap the top left quadrant with the bottom right quadrant
    q0.copyTo(temp);
    q3.copyTo(q0);
    temp.copyTo(q3);
    //Swap the top right quadrant with the bottom left quadrant
    q1.copyTo(temp);
    q2.copyTo(q1);
    temp.copyTo(q2);
}

#pragma endregion

#pragma region Reconstruction Operators

void get_fresnel_propagator(const Settings& setting, Mat& Hz, Mat& H_z, bool flag_phaseref, HOLOGRAM_TYPE hologram_type, bool flag_linearize, OBJ_TYPE object_type)
{
    //Define the kernel type

    KERNEL_TYPE kernel_type = HZ;
    if (hologram_type == INTENSITY && flag_linearize)
    {
        flag_phaseref = false;
        kernel_type = GZ_DEPHASING;

        if (object_type == ABSORBING) kernel_type = GZ_ABSORBING;
    }

    complex<double> temp_calcul;
    double k0 = 2 * setting.n0 * CV_PI / setting.lambda; //Constant k0
    Vec2d* temp; //Pointer to the pixels of a matrix
    Hz = Mat(setting.width, setting.height, CV_64FC2);
    Mat mat_split[2];

    double z_shannon = MAX(setting.width, setting.height) * pow(setting.pixel_size, 2) / setting.lambda; //Shannon's criterion for using the most suitable method is defined as follows

    if (setting.z >= z_shannon)
    {
        //Analytical impulse response in the space domain + FFT
        cout << "Impulse response\n";

        double x, y;
        complex<double> n0_on_i_lambda_z = setting.n0 / 1i / setting.lambda / setting.z;

        for (int i = 0; i < setting.width; i++)
        {
            x = (i - (floor(0.5 * ((double)setting.width - 1)) + 1)) * setting.pixel_size;
            for (int j = 0; j < setting.height; j++)
            {
                y = (j - (floor(0.5 * ((double)setting.height - 1)) + 1)) * setting.pixel_size;

                temp_calcul = exp(setting.n0 * 1i * CV_PI / setting.lambda / setting.z * (pow(x, 2) + pow(y, 2)));

                temp = &Hz.at<Vec2d>(i, j);
                (*temp)[0] = temp_calcul.real();
                (*temp)[1] = temp_calcul.imag();
            }
        }
        complex_multiply(n0_on_i_lambda_z, Hz, Hz);

        if (flag_phaseref)
        {
            temp_calcul = exp(1i * k0 * setting.z);
            complex_multiply(temp_calcul, Hz, Hz);
        }

        if (kernel_type != HZ)
        {
            split(Hz, mat_split);
            if (kernel_type == GZ_DEPHASING)
            {
                mat_split[0] = -2 * mat_split[1];
                mat_split[1] = 0;
            }
            if (kernel_type == GZ_ABSORBING)
            {
                mat_split[0] = 2 * mat_split[0];
                mat_split[1] = 0;
            }
            merge(mat_split, 2, Hz);
        }

        FT_shift(Hz, Hz);
        dft(Hz, Hz, DFT_COMPLEX_OUTPUT);
        multiply(pow(setting.pixel_size, 2), Hz, Hz);

        //We do the conjugate of Hz to find H_z
        split(Hz, mat_split);
        mat_split[1] = -mat_split[1];
        merge(mat_split, 2, H_z);
    }
    else
    {
        //Fourier-domain analytical transfer function
        cout << "Transfer function\n";

        double u, v;
        complex<double> i_pi_lambda_z = -1i * CV_PI * setting.lambda * setting.z / setting.n0;

        for (int i = 0; i < setting.width; i++)
        {
            u = (i - (floor(0.5 * ((double)setting.width - 1)) + 1)) / setting.width / setting.pixel_size;
            for (int j = 0; j < setting.height; j++)
            {
                v = (j - (floor(0.5 * ((double)setting.height - 1)) + 1)) / setting.height / setting.pixel_size;

                temp_calcul = exp(i_pi_lambda_z * (pow(u, 2) + pow(v, 2)));

                temp = &Hz.at<Vec2d>(i, j);
                (*temp)[0] = temp_calcul.real();
                (*temp)[1] = temp_calcul.imag();
            }
        }
        FT_shift(Hz, Hz);

        if (flag_phaseref)
        {
            temp_calcul = exp(1i * k0 * setting.z);
            complex_multiply(temp_calcul, Hz, Hz);
        }

        if (kernel_type != HZ)
        {
            idft(Hz, Hz, DFT_COMPLEX_OUTPUT);
            split(Hz, mat_split);
            if (kernel_type == GZ_DEPHASING)
            {
                mat_split[0] = -2 * mat_split[1];
                mat_split[1] = 0;
            }
            if (kernel_type == GZ_ABSORBING)
            {
                mat_split[0] = 2 * mat_split[0];
                mat_split[1] = 0;
            }
            merge(mat_split, 2, Hz);
            dft(Hz, Hz, DFT_COMPLEX_OUTPUT);
        }

        //We do the conjugate of Hz to find H_z
        split(Hz, mat_split);
        mat_split[1] = -mat_split[1];
        merge(mat_split, 2, H_z);
    }
}

void image_calibration(const Mat& initial_hologram, Mat& optimal_hologram, int do_padding, int do_sqrt)
{
    initial_hologram.convertTo(optimal_hologram, CV_64F);
    if (do_sqrt == 1) sqrt(optimal_hologram, optimal_hologram); //We do the square root of the hologram, because the recovered data are the square of the module

    int initial_size[] = { initial_hologram.rows, initial_hologram.cols };

    Scalar mean_value = mean(optimal_hologram); //We calculate the average of the image
    optimal_hologram = optimal_hologram / mean_value; //We normalize the image by dividing it by its average
    int pad = 1; //We put a border of 1 because we normalized by dividing by the mean

    if (do_padding == 1) padding(optimal_hologram, initial_size[0] * 2, initial_size[1] * 2, pad, optimal_hologram); //Double the size of the image to avoid aliasing
    optimal_FT(optimal_hologram, pad, optimal_hologram);
}

#pragma endregion
