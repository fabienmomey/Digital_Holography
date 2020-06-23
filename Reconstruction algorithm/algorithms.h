
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#pragma region Fienup Algorithm

/*
Allows to reconstruct an image from a hologram using the "Fienup" method
@param hologram: The matrix containing the hologram to be reconstituted with the good size (not modifiable)
@param reconstituted_image: The matrix containing the reconstituted image, but this image is not resize !
@param setting: The parameters to be used to reconstitute the hologram
@param object: The "type" of the object: 0 = phase object or 1 = absorbing object
@param complex_extremum: The table containing the 4 extremes for the complex image: the minimum and maximum of the real part then of the imaginary part (Infinite = DBL_MAX)
@param repetitions: The number of repetitions to be done in the algorithm
*/
void fienup_algorithm(const Mat& optimal_hologram, Mat& reconstituted_image, Settings& setting, int object, double complex_extremum[], int repetitions);

#pragma endregion

#pragma region Regularised Inversion Algorithms

/*
Allows to reconstruct an image from a hologram using the "Regularised Inversion" method in the "ISTA" algorithm
@param hologram: The matrix containing the hologram to be reconstituted with the good size (not modifiable)
@param reconstituted_image: The matrix containing the reconstituted image, but this image is not resize !
@param setting: The parameters to be used to reconstitute the hologram
@param object: The "type" of the object: 0 = phase object or 1 = absorbing object
@param extremum: The table containing the 2 extremes for the image (Infinite = DBL_MAX)
@param repetitions: The number of repetitions to be done in the algorithm
*/
void RI_algorithme_ISTA(const Mat& optimal_hologram, Mat& reconstituted_image, Settings& setting, int object, double extremum[], int repetitions);

/*
Allows to reconstruct an image from a hologram using the "Regularised Inversion" method in the "FISTA" algorithm
@param hologram: The matrix containing the hologram to be reconstituted with the good size (not modifiable)
@param reconstituted_image: The matrix containing the reconstituted image, but this image is not resize !
@param setting: The parameters to be used to reconstitute the hologram
@param object: The "type" of the object: 0 = phase object or 1 = absorbing object
@param extremum: The table containing the 2 extremes for the image (Infinite = DBL_MAX)
@param repetitions: The number of repetitions to be done in the algorithm
*/
void RI_algorithme_FISTA(const Mat& optimal_hologram, Mat& reconstituted_image, Settings& setting, int object, double extremum[], int repetitions);

#pragma endregion
