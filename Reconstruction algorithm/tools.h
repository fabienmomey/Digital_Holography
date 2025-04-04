
///
/// \file tools.h 
/// 
/// \author Yoann MASSET - Fabien MOMEY
/// \version Final version
/// \date June 20th 2020
/// 
/// \brief File grouping the declarations of tools useful for image reconstruction 
/// 

#include <opencv2/highgui.hpp> 
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#pragma region Reconstitution Parameters

///
/// \def EXTREMUMS
/// \brief Table constant 
///
/// The table with the 2 extremes for the complex image: the minimum and maximum of the constraint initialised by Infinite = DBL_MAX 
///
#define EXTREMUMS {-DBL_MAX, DBL_MAX}

/// <summary>
/// Calcul method for complexe
/// </summary>
enum CalculMethod
{
    MODULE_CALCUL, /*!< For the module calcul */
    PHASE_CALCUL   /*!< For the phase calcul */
};

/// <summary>
/// The hologram type : indicates if complex wave or intensity data will be used
/// </summary>
enum HOLOGRAM_TYPE
{
    COMPLEX,/*!< The Fresnel propagation kernel HZ is used in the context of complex wave measurements.It requires of full complex Fresnel kernel U = | A0 | .(1 + HZ * O) where U is the total complex wave and O represents the targeted complex deviation from the unit transmittance plan */

    INTENSITY /*!< The Fresnel propagation kernel is used in the context of intensity measurements.Hence, a "adapted" Fresnel kernel can be calculated depending on the TYPE_MODEL parameter I = | U | ^ 2 = | A0 | ^ 2. | 1 + HZ * O | ^ 2 where I is the intensity of the total complex wave and O still represents the targeted complex deviation from the unit transmittance plane */
};

/// <summary>
/// The object type
/// </summary>
enum OBJ_TYPE
{
    DEPHASING, /*!< Purely and weakly dephasing object. The linearized intensity measurement formation model writes : I = 1 + GZ * O where GZ = -2 Im[HZ] and O represents the targeted phase - shift image */

    ABSORBING /*!< Purely absorbing object. The linearized intensity measurement formation model writes : I = 1 + GZ * O where GZ = 2 Re[HZ] and O represents the targeted opacity image(0 <= O <= 1) */
};

/// <summary>
/// The kernel type
/// </summary>
enum KERNEL_TYPE
{
    HZ,
    GZ_DEPHASING,
    GZ_ABSORBING
};

/// <summary>
/// Compound type that fills in all the necessary parameters to create or reconstitute a hologram.
/// Initialization with random parameters to be modified
/// </summary>
/// <param name="z">Distance of the sensor of the plane (m)</param>
/// <param name="mag">Lens magnification</param>
/// <param name="lambda">Wavelength (m)</param>
/// <param name="n0">Medium refractive index</param>
/// <param name="pixel_size">Pixel size (m)</param>
/// <param name="width">View width in pixels</param>
/// <param name="height">View height in pixels</param>
struct Settings
{
    // INSTRUMENTAL

    double z = 10e-6;                   /*!< Distance of the sensor of the plane (m) */
    double mag = 50;                    /*!< Lens magnification */
    double lambda = 600e-9;             /*!< Wavelength (m) */
    double n0 = 1;                      /*!< Medium refractive index */
    // DIGITAL

    double pixel_size = 1e-6 / mag;     /*!< Pixel size (m) */
    int width = 1024;                   /*!< View width in pixels (initial image widht) */
    int height = 1024;                  /*!< View height in pixels (initial image height) */
};

#pragma endregion

#pragma region Matrix Operations

/// <summary>
/// Multiplies 2 matrices, element by element, with one real matrix and the other in complex notation: M * (A + iB) = MA + iMB
/// </summary>
/// <param name="simple_mat">The real matrix to be multiplied (not modifiable)</param>
/// <param name="complex_mat">The complex matrix to be multiplied (not modifiable)</param>
/// <param name="res_mat">The matrix resulting from the multiplication</param>
void my_multiply(const Mat& simple_mat, const Mat& complex_mat, Mat& res_mat);

/// <summary>
/// Multiplies 2 matrices in complex notation, element by element: (A + iB) * (X + iY) = (AX - BY) + i(AY + BX)
/// </summary>
/// <param name="mat1">The first matrix to be multiplied (not modifiable)</param>
/// <param name="mat2">The second matrix to be multiplied (not modifiable)</param>
/// <param name="res_mat">The matrix resulting from the multiplication</param>
void complex_multiply(const Mat& mat1, const Mat& mat2, Mat& res_mat);

/// <summary>
/// Multiplies one complex matrix by a complex number, element by element: (A + iB) * (X + iY) = (AX - BY) + i(AY + BX)
/// </summary>
/// <param name="complex_scalar">The complex number to be multiplied (not modifiable)</param>
/// <param name="complex_mat">The complex matrix to be multiplied (not modifiable)</param>
/// <param name="res_mat">The matrix resulting from the multiplication</param>
void complex_multiply(const complex<double>& complex_scalar, const Mat& complex_mat, Mat& res_mat);

/// <summary>
/// Adds a border around the image so that it is placed in the center of the new image
/// </summary>
/// <param name="initial_mat">The matrix containing the image to be enlarged in size (not modifiable)</param>
/// <param name="x">The new number of rows (not modifiable)</param>
/// <param name="y">The new number of columns (not modifiable)</param>
/// <param name="pad">The border (int) to be added around the image (not modifiable)</param>
/// <param name="resized_mat">The matrix resulting from this change</param>
void padding(const Mat& initial_mat, const int x, const int y, const int pad, Mat& resized_mat);

/// <summary>
/// Allows to convert a complex image into a displayable image (module or phase)
/// </summary>
/// <param name="initial_mat">The complex matrix to be displayed (not modifiable)</param>
/// <param name="final_mat">The matrix resulting from this change</param>
/// <param name="method">The method used to display the image: either CALCUL_MODULE for module calculation or CALCUL_PHASE for phase calculation</param>
/// <param name="log_scale">Boolean which allows or not to pass the image in logarithmic scale (by default: false)</param>
void complex_display(const Mat& initial_mat, Mat& final_mat, CalculMethod method, bool log_scale = false);

#pragma endregion

#pragma region Fourier Transform Tools

/// <summary>
/// Gets the optimal size for the Fourier transform and adds a margin if neededv
/// </summary>
/// <param name="initial_mat">The matrix containing the image to be optimized in size (not modifiable)</param>
/// <param name="pad">The border (int) to be added around the image (not modifiable)</param>
/// <param name="final_mat">The matrix resulting from this change</param>
void optimal_FT(const Mat& initial_mat, const int pad, Mat& final_mat);

/// <summary>
/// Allows to swap the 4 quadrants of the image (the 4 quarters of the image) to replace the origin in the image center (initially top left)
/// </summary>
/// <param name="initial_mat">The matrix containing the image to be modified (not modifiable)</param>
/// <param name="final_mat">The modified matrix</param>
void FT_shift(const Mat& initial_mat, Mat& final_mat);

#pragma endregion

#pragma region Reconstruction Operators

/// <summary>
/// Allows to create a propagation and backpropagation kernel according to different parameters
/// </summary>
/// <param name="setting">The parameters to be used to create the kernel (not modifiable)</param>
/// <param name="Hz">The propagation kernel</param>
/// <param name="H_z">The backpropagation kernel</param>
/// <param name="flag_phaseref">Indicates whether the phase origin is taken into account in the calculation of the propagation kernel, that is to say that the absolute phase term exp(2i pi Z / LAMBDA) induced by the propagation distance Z is calculated in the kernel (by default: false)</param>
/// <param name="hologram_type">Indicates if complex wave or intensity data will be used (by default: COMPLEX)</param>
/// <param name="flag_linearize">It indicates which kind of model will be applied to possibly adapt the calculation of the kernel (to set only if TYPE_HOLOGRAM = INTENSITY) (by default: false) \n false: The full propagation model has to be calculated.It requires the calculation of the full complex Fresnel kernel HZ \n true: Used in the context of TYPE_HOLOGRAM = 'intensity' only.A linearized model of intensity measurement can be applied.It yields a modified / simplified Fresnel kernel depending on the TYPE_OBJ parameter.In this case, FLAG_PHASEREF will be set to false</param>
/// <param name="object_type">(to set only if TYPE_LINEARIZE = true) (by default: DEPHASING)</param>
void get_fresnel_propagator(const Settings& setting, Mat& Hz, Mat& H_z, bool flag_phaseref = false, HOLOGRAM_TYPE hologram_type = COMPLEX, bool flag_linearize = false, OBJ_TYPE object_type = DEPHASING);

/// <summary>
/// Prepare the hologram image by doing the square root or not, normalizing the hologram (division by the mean value) and doing padding to have a good size
/// </summary>
/// <param name="initial_hologram">The matrix containing the initial hologram data (not modifiable)</param>
/// <param name="optimal_hologram">The matrix containing the hologram with the good size to be reconstitued</param>
/// <param name="do_padding">If equal to 1, we make padding by doubling the dimensions, otherwise we don't make padding</param>
/// <param name="do_sqrt">If equal to 1, we make the square root on the hologram data, otherwise we don't make the square root</param>
void image_calibration(const Mat& initial_hologram, Mat& optimal_hologram, int do_padding, int do_sqrt);

#pragma endregion
