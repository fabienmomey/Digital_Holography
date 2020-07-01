
///
/// \file algorithms.h 
/// 
/// \author Yoann MASSET - Fabien MOMEY
/// \version Final version
/// \date June 20th 2020
/// 
/// \brief File grouping the declarations of reconstruction algorithm functions
/// 

#include <opencv2/highgui.hpp> 
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#pragma region Simple Backpropagation

/// <summary>
/// Allows to reconstruct an image applying a simple backpropagation
/// </summary>
/// <param name="optimal_hologram">The matrix containing the hologram to be reconstituted with the good size (not modifiable)</param>
/// <param name="reconstituted_image">The matrix containing the reconstituted image, but this image is not resize !</param>
/// <param name="setting">The parameters to be used to reconstitute the hologram</param>
void simple_backpropagation(const Mat& optimal_hologram, Mat& reconstituted_image, Settings& setting);

#pragma endregion


#pragma region Fienup Algorithm

/// <summary>
/// Allows to reconstruct an image from a hologram using the "Fienup" method
/// </summary>
/// <param name="optimal_hologram">The matrix containing the hologram to be reconstituted with the good size (not modifiable)</param>
/// <param name="reconstituted_image">The matrix containing the reconstituted image, but this image is not resize !</param>
/// <param name="setting">The parameters to be used to reconstitute the hologram</param>
/// <param name="object">The "type" of the object: 0 = phase object or 1 = absorbing object</param>
/// <param name="complex_extremum">The table containing the 4 extremes for the complex image: the minimum and maximum of the real part then of the imaginary part (Infinite = DBL_MAX)</param>
/// <param name="repetitions">The number of repetitions to be done in the algorithm</param>
void fienup_algorithm(const Mat& optimal_hologram, Mat& reconstituted_image, Settings& setting, int object, double complex_extremum[], int repetitions);

#pragma endregion

#pragma region Regularised Inversion Algorithms

/// <summary>
/// Allows to reconstruct an image from a hologram using the "Regularised Inversion" method in the "ISTA" algorithm
/// </summary>
/// <param name="optimal_hologram">The matrix containing the hologram to be reconstituted with the good size (not modifiable)</param>
/// <param name="reconstituted_image">The matrix containing the reconstituted image, but this image is not resize !</param>
/// <param name="setting">The parameters to be used to reconstitute the hologram</param>
/// <param name="object">The "type" of the object: 0 = phase object or 1 = absorbing object</param>
/// <param name="extremum">The table containing the 2 extremes for the image (Infinite = DBL_MAX)</param>
/// <param name="repetitions">The number of repetitions to be done in the algorithm</param>
void RI_algorithme_ISTA(const Mat& optimal_hologram, Mat& reconstituted_image, Settings& setting, int object, double extremum[], int repetitions);

/// <summary>
/// Allows to reconstruct an image from a hologram using the "Regularised Inversion" method in the "FISTA" algorithm
/// </summary>
/// <param name="optimal_hologram">The matrix containing the hologram to be reconstituted with the good size (not modifiable)</param>
/// <param name="reconstituted_image">The matrix containing the reconstituted image, but this image is not resize !</param>
/// <param name="setting">The parameters to be used to reconstitute the hologram</param>
/// <param name="object">The "type" of the object: 0 = phase object or 1 = absorbing object</param>
/// <param name="extremum">The table containing the 2 extremes for the image (Infinite = DBL_MAX)</param>
/// <param name="repetitions">The number of repetitions to be done in the algorithm</param>
void RI_algorithme_FISTA(const Mat& optimal_hologram, Mat& reconstituted_image, Settings& setting, int object, double extremum[], int repetitions);

#pragma endregion
