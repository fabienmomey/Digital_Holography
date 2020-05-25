
#include "fonction.h"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

void histogram_calcul(const Mat& initial_image, Mat& image_hist)
{
    int source_number = 1;
    int dims = 1;
    int hist_size = 256; //The 256 values that a pixel can take
    float data_range[] = { 0, 255 }; //Each pixel can take a value between 0 and 256
    const float* hist_range = { data_range };
    bool uniform = true; //The value range is uniform, from 1 to 1

    int scale = 5; //Scale size
    int hist_width = scale * hist_size;
    int hist_height = 1000;
    image_hist = Mat(hist_height, hist_width, CV_8UC3, Scalar(0, 0, 0));

    if (initial_image.channels() == 1) //If we get a grayscale image
    {
        Mat grey_hist;

        calcHist(&initial_image, source_number, 0, Mat(), grey_hist, dims, &hist_size, &hist_range, uniform, false);

        float maxi = 0;
        for (int i = 0; i < hist_size; i++) //We look for the highest value in all the histograms to adapt the height of the final image
        {
            if (grey_hist.at<float>(i) > maxi) maxi = grey_hist.at<float>(i);
        }
        assert(maxi != 0); //We check that the max is not 0, if it is the case, the program stops here

        for (int i = 1; i < hist_size; i++) //Each line is drawn between the previous point and the current point to obtain the histogram
        {
            line(image_hist, Point(scale * (i - 1), hist_height * (1 - grey_hist.at<float>(i - 1) / maxi)), Point(scale * (i), hist_height * (1 - grey_hist.at<float>(i) / maxi)), Scalar(255, 255, 255), 2, 8, 0);
        }
    }

    else if (initial_image.channels() == 3) //If we get a color image
    {
        //We create a matrix for each color channel
        Mat r(initial_image.rows, initial_image.cols, CV_8UC1);
        Mat g(initial_image.rows, initial_image.cols, CV_8UC1);
        Mat b(initial_image.rows, initial_image.cols, CV_8UC1);

        Mat bgr[] = { b, g, r };

        split(initial_image, bgr); //We separate the image into its different channels

        //The same method as for the grayscale image is used for each channel
        Mat hist_blue, hist_green, hist_red;

        calcHist(&b, source_number, 0, Mat(), hist_blue, dims, &hist_size, &hist_range, uniform, false);
        calcHist(&g, source_number, 0, Mat(), hist_green, dims, &hist_size, &hist_range, uniform, false);
        calcHist(&r, source_number, 0, Mat(), hist_red, dims, &hist_size, &hist_range, uniform, false);

        float maxi = 0;
        for (int i = 0; i < hist_size; i++) //We're looking for the highest value in all the histograms
        {
            if (hist_blue.at<float>(i) > maxi) maxi = hist_blue.at<float>(i);
            if (hist_green.at<float>(i) > maxi) maxi = hist_green.at<float>(i);
            if (hist_red.at<float>(i) > maxi) maxi = hist_red.at<float>(i);
        }
        assert(maxi != 0);

        for (int i = 1; i < hist_size; i++)
        {
            line(image_hist, Point(scale * (i - 1), hist_height * (1 - hist_blue.at<float>(i - 1) / maxi)), Point(scale * (i), hist_height * (1 - hist_blue.at<float>(i) / maxi)), Scalar(255, 0, 0), 2, 8, 0);
            line(image_hist, Point(scale * (i - 1), hist_height * (1 - hist_green.at<float>(i - 1) / maxi)), Point(scale * (i), hist_height * (1 - hist_green.at<float>(i) / maxi)), Scalar(0, 255, 0), 2, 8, 0);
            line(image_hist, Point(scale * (i - 1), hist_height * (1 - hist_red.at<float>(i - 1) / maxi)), Point(scale * (i), hist_height * (1 - hist_red.at<float>(i) / maxi)), Scalar(0, 0, 255), 2, 8, 0);
        }
    }

    else
    {
        cout << "Error: The histogram cannot be plotted.\n";
    }
}

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

    //We wish to have the same margin all around(to the nearest 1)

    top_margin = floor(1.0 * (x - initial_mat.rows) / 2);
    left_margin = floor(1.0 * (y - initial_mat.cols) / 2);

    if ((x - initial_mat.rows) % 2 == 1) bottom_margin = top_margin + 1;
    else bottom_margin = top_margin;

    if ((y - initial_mat.cols) % 2 == 1) right_margin = left_margin + 1;
    else right_margin = left_margin;

    //We place the initial image in the middle of the final image with the right dimensions and fill the "empty" space with pixels of the chosen value
    copyMakeBorder(initial_mat, resized_mat, top_margin, bottom_margin, left_margin, right_margin, BORDER_CONSTANT, Scalar::all(pad));
}

void optimal_FT(const Mat& initial_mat, const int pad, Mat& final_mat)
{
    //We get the optimal size for the Fourier transform
    int row_number = getOptimalDFTSize(initial_mat.rows);
    int col_number = getOptimalDFTSize(initial_mat.cols);

    //The padding function is used to resize the image
    padding(initial_mat, row_number, col_number, pad, final_mat);
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

    //This results in high values in white and low values in black

    normalize(final_mat, final_mat, 0, 1, NORM_MINMAX);
}

void swap_quadrants(const Mat& initial_mat, Mat& final_mat)
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

void image_display(const Mat& image, string name, bool norm)
{
    namedWindow(name, WINDOW_AUTOSIZE); //We create the window
    if (norm)
    {
        Mat temp;
        normalize(image, temp, 0, 255, NORM_MINMAX);
        imshow(name, temp); //We display the image in this window
    }
    else imshow(name, image); //We display the image in this window
}

void complex_exp(const Mat& initial_mat, Mat& final_mat)
{
    final_mat = Mat(initial_mat.size(), CV_64FC2);

    //exp(i*M) = cos(M) + i*sin(M)
    for (int i = 0; i < initial_mat.rows; i++)
    {
        for (int j = 0; j < initial_mat.cols; j++)
        {
            final_mat.at<Vec2d>(i, j)[0] = cos(initial_mat.at<double>(i, j));
            final_mat.at<Vec2d>(i, j)[1] = sin(initial_mat.at<double>(i, j));
        }
    }
}

void fresnel_propagator(const Settings& setting, Mat& Hz, Mat& H_z)
{
    complex<double> temp_calcul;
    double k0 = 2 * setting.n0 * CV_PI / setting.lambda; //Constant k0

    Vec2d* Hz_temp, * H_z_temp;

    Hz = Mat(setting.width, setting.height, CV_64FC2);
    H_z = Mat(setting.width, setting.height, CV_64FC2);

    double z_shannon = MAX(setting.width, setting.height) * pow(setting.pixel_size, 2) / setting.lambda; //Shannon's criterion for using the most suitable method is defined as follows

    if (setting.z >= z_shannon)
    {
        //Analytical impulse response in the space domain + FFT
        cout << "Impulse response\n";
        double x, y;

        complex<double> n0_on_i_pi_lambda_z = setting.n0 / 1i / setting.lambda / setting.z;

        for (int i = 0; i < setting.width; i++)
        {
            x = (i - (floor(0.5 * ((double)setting.width - 1)) + 1)) * setting.pixel_size;
            for (int j = 0; j < setting.height; j++)
            {
                y = (j - (floor(0.5 * ((double)setting.height - 1)) + 1)) * setting.pixel_size;

                temp_calcul = exp(setting.n0 * 1i * CV_PI / setting.lambda / setting.z * (pow(x, 2) + pow(y, 2))) * exp(1i * k0 * setting.z);
                //hz.at<Vec2d>(i, j)[0] = calc_inter.real();
                //hz.at<Vec2d>(i, j)[1] = calc_inter.imag();

                //Calcul of Hz (propagation)
                Hz_temp = &Hz.at<Vec2d>(i, j);
                (*Hz_temp)[0] = temp_calcul.real() * n0_on_i_pi_lambda_z.real() - temp_calcul.imag() * n0_on_i_pi_lambda_z.imag();
                (*Hz_temp)[1] = temp_calcul.real() * n0_on_i_pi_lambda_z.imag() + temp_calcul.imag() * n0_on_i_pi_lambda_z.real();

                //Calcul of H_z (back-propagation): same method as for Hz but replacing z by -z
                H_z_temp = &H_z.at<Vec2d>(i, j);
                (*H_z_temp)[0] = temp_calcul.real() * (-n0_on_i_pi_lambda_z.real()) - temp_calcul.imag() * n0_on_i_pi_lambda_z.imag();
                (*H_z_temp)[1] = temp_calcul.real() * (-n0_on_i_pi_lambda_z.imag()) + temp_calcul.imag() * n0_on_i_pi_lambda_z.real();
            }
        }
        double pixel_size_square = pow(setting.pixel_size, 2);

        swap_quadrants(Hz, Hz);
        dft(Hz, Hz, DFT_COMPLEX_OUTPUT);
        multiply(pixel_size_square, Hz, Hz);

        swap_quadrants(H_z, H_z);
        dft(H_z, H_z, DFT_COMPLEX_OUTPUT);
        multiply(pixel_size_square, H_z, H_z);
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

                //Calcul of Hz (propagation)
                temp_calcul = exp(i_pi_lambda_z * (pow(u, 2) + pow(v, 2))) * exp(1i * k0 * setting.z);

                Hz_temp = &Hz.at<Vec2d>(i, j);
                (*Hz_temp)[0] = temp_calcul.real();
                (*Hz_temp)[1] = temp_calcul.imag();

                //Calcul of H_z (back-propagation): same method as for Hz but replacing z by -z
                H_z_temp = &H_z.at<Vec2d>(i, j);
                (*H_z_temp)[0] = temp_calcul.real();
                (*H_z_temp)[1] = -temp_calcul.imag();
            }
        }
        swap_quadrants(Hz, Hz);
        swap_quadrants(H_z, H_z);
    }
}

void fresnel_propagation_kernel(const Settings& setting, Mat& Hz, Mat& H_z, bool flag_phaseref, HOLOGRAM_TYPE hologram_type, bool flag_linearize, OBJ_TYPE object_type)
{
    KERNEL_TYPE kernel_type = HZ;

    if (hologram_type == INTENSITY && flag_linearize)
    {
        flag_phaseref = false;
        kernel_type = GZ_DEPHASING;

        if (object_type == ABSORBING) kernel_type = GZ_ABSORBING;
    }

    complex<double> temp_calcul;
    double k0 = 2 * setting.n0 * CV_PI / setting.lambda; //Constant k0
    Vec2d* temp;
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
                mat_split[0] = 0;
                mat_split[1] = -2 * mat_split[1];
            }
            if (kernel_type == GZ_ABSORBING)
            {
                mat_split[0] = 2 * mat_split[0];
                mat_split[1] = 0;
            }
            merge(mat_split, 2, Hz);
        }

        swap_quadrants(Hz, Hz);
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
        swap_quadrants(Hz, Hz);

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
                mat_split[0] = 0;
                mat_split[1] = -2 * mat_split[1];
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

void reconstitution(const Mat& hologram, Mat& reconstituted_image, Settings& setting, int pad)
{
    int initial_size[] = { hologram.rows, hologram.cols };

    hologram.convertTo(hologram, CV_64F);
    sqrt(hologram, hologram); //We do the square root of the hologram, because the recovered data are the square of the module

    Mat optimal_hologram;
    padding(hologram, initial_size[0] * 2, initial_size[1] * 2, pad, optimal_hologram); //Double the size of the image to avoid aliasing

    optimal_FT(optimal_hologram, pad, optimal_hologram);

    int final_size[] = { optimal_hologram.rows, optimal_hologram.cols };

    //We change the size parameters
    setting.width = final_size[0];
    setting.height = final_size[1];

    optimal_hologram.convertTo(optimal_hologram, CV_64FC1);

    Mat Hz, H_z;
    fresnel_propagator(setting, Hz, H_z); //We recover the 2 kernel according to the above parameters

    Mat fourier_hologram;
    dft(optimal_hologram, fourier_hologram, DFT_COMPLEX_OUTPUT); //We do the Fourier transform of the hologram

    Mat product;
    complex_multiply(fourier_hologram, H_z, product); //We multiply the TF with the kernel (complex input and output)

    Mat inverse_FT;
    idft(product, inverse_FT, DFT_COMPLEX_OUTPUT); //We perform the inverse Fourier transform of the product

    //We reduce the reconstructed image to keep only the central part that corresponds to the result
    int row_margin = floor(1.0 * (final_size[0] - initial_size[0]) / 2), col_margin = floor(1.0 * (final_size[1] - initial_size[1]) / 2);
    Mat(inverse_FT, Rect(row_margin, col_margin, initial_size[0], initial_size[1])).copyTo(reconstituted_image);

}

void complex_reconstitution(const Mat& real_hologram, const Mat& imag_hologram, Mat& reconstituted_image, Settings& setting, int pad_Re, int pad_Im)
{
    //For the reconstruction of a complex hologram, it's exactly the same method as a simple hologram

    Mat complex_hologram[] = { real_hologram, imag_hologram }; //We create the complex hologram in the form: Re + i*Im

    Mat hologram;
    merge(complex_hologram, 2, hologram);

    int initial_size[] = { hologram.rows, hologram.cols };

    Mat optimal_complex_hologram[2];

    //Double the size of the image to avoid aliasing
    padding(complex_hologram[0], initial_size[0] * 2, initial_size[1] * 2, pad_Re, optimal_complex_hologram[0]);
    optimal_FT(optimal_complex_hologram[0], pad_Re, optimal_complex_hologram[0]);

    padding(complex_hologram[1], initial_size[0] * 2, initial_size[1] * 2, pad_Im, optimal_complex_hologram[1]);
    optimal_FT(optimal_complex_hologram[1], pad_Im, optimal_complex_hologram[1]);

    Mat optimal_hologram;
    merge(optimal_complex_hologram, 2, optimal_hologram);

    int final_size[] = { optimal_hologram.rows, optimal_hologram.cols };

    //We change the size parameters
    setting.width = final_size[0];
    setting.height = final_size[1];

    optimal_hologram.convertTo(optimal_hologram, CV_64FC2);

    Mat Hz, H_z;
    fresnel_propagator(setting, Hz, H_z); //We recover the 2 kernel according to the above parameters

    Mat fourier_hologram;
    dft(optimal_hologram, fourier_hologram, DFT_COMPLEX_INPUT | DFT_COMPLEX_OUTPUT); //The Fourier transform of the hologram is performed (in this case, the input and output are complex)

    Mat product;
    complex_multiply(fourier_hologram, H_z, product); //We multiply the TF with the kernel (complex input and output)

    Mat inverse_FT;
    idft(product, inverse_FT, DFT_COMPLEX_OUTPUT); //We perform the inverse Fourier transform of the product

    //We reduce the reconstructed image to keep only the central part that corresponds to the result
    int marge_rows = floor(1.0 * (final_size[0] - initial_size[0]) / 2), marge_cols = floor(1.0 * (final_size[1] - initial_size[1]) / 2);
    Mat(inverse_FT, Rect(marge_rows, marge_cols, initial_size[0], initial_size[1])).copyTo(reconstituted_image);
}

void fienup_reconstitution(const Mat& hologram, Mat& reconstituted_image, Settings& setting, int repetitions)
{
    double e1, e2, seconds;

    Mat temp;
    hologram.convertTo(temp, CV_64F);
    sqrt(temp, temp); //We do the square root of the hologram, because the recovered data are the square of the module

    int initial_size[] = { hologram.rows, hologram.cols };

    Scalar mean_value = mean(temp); //We calculate the average of the image
    temp /= mean_value; //We normalize the image by dividing it by its average
    int pad = 1; //We put a border of 1 because we normalized by dividing by the mean

    Mat optimal_hologram;
    padding(temp, initial_size[0] * 2, initial_size[1] * 2, pad, optimal_hologram); //Double the size of the image to avoid aliasing
    optimal_FT(optimal_hologram, pad, optimal_hologram);

    int final_size[] = { optimal_hologram.rows, optimal_hologram.cols };

    //We change the size parameters
    setting.width = final_size[0];
    setting.height = final_size[1];

    Mat Hz, H_z;
    e1 = getTickCount();

    //fresnel_propagator(param, Hz, H_z); //Old function
    fresnel_propagation_kernel(setting, Hz, H_z); //We recover the 2 kernel according to the above parameters

    e2 = getTickCount();
    seconds = (e2 - e1) / getTickFrequency();
    cout << "Fresnel : " << seconds << " seconds\n";

    Mat hologram_iteration; //The matrix that will be retrieved and modified at each iteration
    optimal_hologram.copyTo(hologram_iteration);

    Mat FT, FT_product, inverse_FT; //Fourier matrix
    Mat mat_split[2], module1;

    for (int i = 0; i < repetitions; i++)
    {
        e1 = getTickCount();

        //Backpropagation
        dft(hologram_iteration, FT, DFT_COMPLEX_OUTPUT);
        complex_multiply(FT, H_z, FT_product);
        idft(FT_product, inverse_FT, DFT_COMPLEX_OUTPUT);

        //We recreate an image by changing the module to 1: module1 = inverse_FT / abs(inverse_FT) with positive real part and positive imaginary part
        split(inverse_FT, mat_split);
        max(0, mat_split[0], mat_split[0]); //We force the real positive part
        max(0, mat_split[1], mat_split[1]); //We force the positive imaginary part
        magnitude(mat_split[0], mat_split[1], temp); //Calcul of the module
        divide(1, temp, temp);
        merge(mat_split, 2, inverse_FT);
        my_multiply(temp, inverse_FT, module1);

        //Propagation
        dft(module1, FT, DFT_COMPLEX_OUTPUT);
        complex_multiply(FT, Hz, FT_product);
        idft(FT_product, inverse_FT, DFT_COMPLEX_OUTPUT);

        //The initial data of the hologram are reset: hologram_iteration = optimal_hologram * inverse_FT / abs(inverse_FT)
        split(inverse_FT, mat_split);
        magnitude(mat_split[0], mat_split[1], temp); //Calcul of the module
        divide(optimal_hologram, temp, temp);
        my_multiply(temp, inverse_FT, hologram_iteration);

        e2 = getTickCount();
        seconds = (e2 - e1) / getTickFrequency();
        cout << "iteration " << i << " : " << seconds << " seconds\n";
    }

    //We reduce the reconstructed image to keep only the central part that corresponds to the result
    int row_margin = floor(1.0 * (final_size[0] - initial_size[0]) / 2), col_margin = floor(1.0 * (final_size[1] - initial_size[1]) / 2);
    Mat(module1, Rect(row_margin, col_margin, initial_size[0], initial_size[1])).copyTo(reconstituted_image);
}

