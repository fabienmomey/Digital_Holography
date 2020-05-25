
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

enum CalculMethod //Calcul method for complexes
{
    MODULE_CALCUL,  //For the module calcul
    PHASE_CALCUL    //For the phase calcul
};

enum HOLOGRAM_TYPE //The hologram type : indicates if complex wave or intensity data will be used
{
    //The Fresnel propagation kernel HZ is used in the context of complex wave measurements.It requires of full complex Fresnel kernel
    //U = | A0 | .(1 + HZ * O) where U is the total complex wave and O represents the targeted complex deviation from the unit transmittance plan
    COMPLEX,

    //The Fresnel propagation kernel is used in the context of intensity measurements.Hence, a "adapted" Fresnel kernel can be calculated depending on the TYPE_MODEL parameter
    //I = | U | ^ 2 = | A0 | ^ 2. | 1 + HZ * O | ^ 2 where I is the intensity of the total complex wave and O still represents the targeted complex deviation from the unit transmittance plane
    INTENSITY
};

enum OBJ_TYPE //The object type
{
    //Purely and weakly dephasing object. The linearized intensity measurement formation model writes :
    //I = 1 + GZ * O where GZ = -2 Im[HZ] and O represents the targeted phase - shift image
    DEPHASING,

    //Purely absorbing object. The linearized intensity measurement formation model writes :
    //I = 1 + GZ * O where GZ = 2 Re[HZ] and O represents the targeted opacity image(0 <= O <= 1)
    ABSORBING
};

enum KERNEL_TYPE
{
    HZ,
    GZ_DEPHASING,
    GZ_ABSORBING
};

/*
Compound type that fills in all the necessary parameters to create or reconstitute a hologram.
Initialization with random parameters to be modified
@param z: Distance of the sensor of the plane (m)
@param mag: Lens magnification
@param lambda:  Wavelength (m)
@param n0: Medium refractive index
@param pixel_size: Pixel size (m)
@param width: View width in pixels
@param height: View height in pixels
*/
struct Settings
{
    // INSTRUMENTAL

    double z = 10e-6;                   // Distance of the sensor of the plane (m)
    double mag = 50;                    // Lens magnification
    double lambda = 600e-9;             // Wavelength (m)
    double n0 = 1;                      // Medium refractive index
    // DIGITAL

    double pixel_size = 1e-6 / mag;     // Pixel size (m)
    int width = 1024;                   // View width in pixels
    int height = 1024;                  // View height in pixels
};

/*
Calculates the histogram of an image (color or grayscale)
@param initial_image: The matrix containing the image to be histographed (not modifiable)
@param image_hist: The matrix containing the histogram
*/
void histogram_calcul(const Mat& initial_image, Mat& image_hist);

/*
Multiplies 2 matrices, element by element, with one real matrix and the other in complex notation: M * (A + iB) = MA + iMB
@param simple_mat: The real matrix to be multiplied (noy modifiable)
@param complex_mat: The complex matrix to be multiplied (not modifiable)
@param res_mat: The matrix resulting from the multiplication
*/
void my_multiply(const Mat& simple_mat, const Mat& complex_mat, Mat& res_mat);

/*
Multiplies 2 matrices in complex notation, éléments par éléments: (A + iB) * (X + iY) = (AX - BY) + i(AY + BX)
@param mat1: The first matrix to be multiplied (not modifiable)
@param mat2: The second matrix to be multiplied (not modifiable)
@param res_mat: The matrix resulting from the multiplication
*/
void complex_multiply(const Mat& mat1, const Mat& mat2, Mat& res_mat);

/*
Multiplies one complex matrix by a complex number, éléments par éléments: (A + iB) * (X + iY) = (AX - BY) + i(AY + BX)
@param complex_scalar: The complex number to be multiplied (not modifiable)
@param complex_mat: The complex matrix to be multiplied (not modifiable)
@param res_mat: The matrix resulting from the multiplication
*/
void complex_multiply(const complex<double>& complex_scalar, const Mat& complex_mat, Mat& res_mat);

/*
Adds a border around the image so that it is placed in the center of the new image
@param initial_mat: The matrix containing the image to be enlarged in size (not modifiable)
@param x: The new number of rows (not modifiable)
@param y: The new number of columns (not modifiable)
@param pad: The border (int) to be added around the image (not modifiable)
@param resized_mat: The matrix resulting from this change
*/
void padding(const Mat& initial_mat, const int x, const int y, const int pad, Mat& resized_mat);

/*
Gets the optimal size for the Fourier transform and adds a margin if needed
@param initial_mat: The matrix containing the image to be optimized in size (not modifiable)
@param pad: The border (int) to be added around the image (not modifiable)
@param final_mat: The matrix resulting from this change
*/
void optimal_FT(const Mat& initial_mat, const int pad, Mat& final_mat);

/*
Allows to convert a complex image into a displayable image (module or phase)
@param initial_mat: The complex matrix to be displayed (not modifiable)
@param final_mat: The matrix resulting from this change
@param method: The method used to display the image: either CALCUL_MODULE for module calculation or CALCUL_PHASE for phase calculation
@param log_scale: Boolean which allows or not to pass the image in logarithmic scale (by default: false)
*/
void complex_display(const Mat& initial_mat, Mat& final_mat, CalculMethod method, bool log_scale = false);

/*
Allows to swap the 4 quadrants of the image (the 4 quarters of the image)
@param initial_mat: The matrix containing the image to be modified (not modifiable)
@param final_mat: The modified matrix
*/
void swap_quadrants(const Mat& initial_mat, Mat& final_mat);

/*
Displays an image
@param image: The matrix of the image to be displayed (not modifiable)
@param name: The name of the window in which the image will be displayed
@param norm: Boolean which allows or not to normalize the image (by default: false)
*/
void image_display(const Mat& image, string name, bool norm = false);

/*
Allows to calculate the complex exponential of a matrix: exp(i*M) = cos(M) + i*sin(M)
@param initial_mat: The matrix to calculate the complex exponential (not modifiable)
@param final_mat: The result matrix
*/
void complex_exp(const Mat& initial_mat, Mat& final_mat);

/*
Allows to create a propagation and backpropagation kernel according to different parameters
@param setting: The parameters to be used to create the kernel (not modifiable)
@param Hz: Le pointeur vers le kernel de propagation
@param H_z: Le pointeur vers le kernel de rétro-propagation
*/
void fresnel_propagator(const Settings& setting, Mat& Hz, Mat& H_z);

/*
Allows to create a propagation and backpropagation kernel according to different parameters
@param setting: The parameters to be used to create the kernel (not modifiable)
@param Hz: The propagation kernel
@param H_z: The backpropagation kernel
@param flag_phaseref: Indicates whether the phase origin is taken into account in the calculation of the propagation kernel,
@param |----> That is to say that the absolute phase term exp(2i pi Z / LAMBDA) induced by the propagation distance Z is calculated in the kernel (by default: false)
@param hologram_type: Indicates if complex wave or intensity data will be used (by default: COMPLEX)
@param flag_linearize: It indicates which kind of model will be applied to possibly adapt the calculation of the kernel (to set only if TYPE_HOLOGRAM = INTENSITY) (by default: false)
@param |----> false: The full propagation model has to be calculated. It requires the calculation of the full complex Fresnel kernel HZ
@param |----> true: Used in the context of TYPE_HOLOGRAM='intensity' only. A linearized model of intensity measurement can be applied. It yields a modified/simplified Fresnel kernel depending on the TYPE_OBJ parameter. In this case, FLAG_PHASEREF will be set to false
@param object_type: (to set only if TYPE_LINEARIZE = true) (by default: DEPHASING)
*/
void fresnel_propagation_kernel(const Settings& setting, Mat& Hz, Mat& H_z, bool flag_phaseref = false, HOLOGRAM_TYPE hologram_type = COMPLEX, bool flag_linearize = false, OBJ_TYPE object_type = DEPHASING);

/*
Allows to reconstruct an image from a hologram
@param hologram: The matrix containing the hologram to be reconstituted (not modifiable)
@param reconstituted_image: The matrix containing the reconstituted image
@param setting: The parameters to be used to reconstitute the hologram
@param pad: The border (int) to be added around the image (by default: 1)
*/
void reconstitution(const Mat& hologram, Mat& reconstituted_image, Settings& setting, int pad = 1);

/*
Allows to reconstruct an image from a complex hologram
@param real_hologram: The matrix containing the real part of the hologram to be reconstituted (not modifiable)
@param imag_hologram: The matrix containing the imaginary part of the hologram to be reconstituted (not modifiable)
@param reconstituted_image: The matrix containing the reconstituted image
@param setting: The parameters to be used to reconstitute the hologram
@param pad_Re: The real part of the border (int) to be added around the image (by default: 1)
@param pad_Im: The imaginary part of the border (int) to be added around the image (by default: 0)
@param |----> Typically, the image is filled with the complex number 1+0i to obtain a module of 1 and a phase of 0f
*/
void complex_reconstitution(const Mat& real_hologram, const Mat& imag_hologram, Mat& reconstituted_image, Settings& setting, int pad_Re = 1, int pad_Im = 0);

/*
Allows to reconstruct an image from a hologram using the "Fienup" method
@param hologram: The matrix containing the hologram to be reconstituted (not modifiable)
@param reconstituted_image: The matrix containing the reconstituted image
@param setting: The parameters to be used to reconstitute the hologram
@param repetitions: The number of repetitions to be done in the algorithm (by default: 10)
*/
void fienup_reconstitution(const Mat& hologram, Mat& reconstituted_image, Settings& setting, int repetitions = 10);
