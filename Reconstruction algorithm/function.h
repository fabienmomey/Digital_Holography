#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

enum MethodeCalcul {
    CALCUL_MODULE = 0,  //Pour le calcul du module
    CALCUL_PHASE = 1    //Pour le calcul de la phase
};

/*
Type composé qui renseigne tous les paramètres nécessaires pour créer ou reconstituer un hologramme
/!\ Initialisation avec des paramètres aléatoires à modifier
<champ : z> Distance du capteur du plan (m)
<champ : mag> Grossissement de l'objectif
<champ : lambda> Longueur d'onde (m)
<champ : n0> Indice de réfraction moyen (non obligatoire)
<champ : pixel_size> Taille des pixels (m)
<champ : width> Largeur de la vue en pixels
<champ : height> Hauteur de la vue en pixels
*/
struct Parametres
{
    // INSTRUMENTAL

    double z = 10e-6;
    double mag = 50;
    double lambda = 600e-9;
    double n0 = 1;

    // DIGITAL

    double pixel_size = 1e-6 / mag;
    int width = 1024;
    int height = 1024;
};

/*
Calcul l'histogramme pour une image (couleur ou niveau de gris)
<paramètre : image_initiale> Le pointeur vers la matrice contenant l'image dont on souhaite faire l'histogramme (non modifiable)
<paramètre : image_hist> Le pointeur vers la matrice contenant l'histogramme
*/
void calcul_histogramme(const Mat& image_initiale, Mat& image_hist);

//void transformee_fourier(const Mat& image_initiale, Mat& im_fourier, Mat& phase_img);
//void transformee_fourier(const Mat& image_initiale, const Size& new_taille, Mat& im_fourier, Mat& phase_img);
//void TF_inverse(const Mat& image_initiale, const Mat& phase_img, Mat& tf_inverse);
//void convolution(const Mat& image_initiale, const Mat& kernel, Mat& im_convolution);

/*
Multiplie 2 images en notation complexe : (A + iB) * (X + iY) = (AX - BY) + i(AY + BX)
<paramètre : mat1> Le pointeur vers la première matrice à multiplier (non modifiable)
<paramètre : mat2> Le pointeur vers la seconde matrice à multiplier (non modifiable)
<paramètre : mat_res> Le pointeur vers la matrice résultat de la multiplication
*/
void multiplier_cmplx(const Mat& mat1, const Mat& mat2, Mat& mat_res);

/*
Place l'image au milieu et ajoute des bordures autour
<paramètre : image_initiale> Le pointeur vers la matrice contenant l'image dont on souhaite agrandir la taille (non modifiable)
<paramètre : x> Le nouveau nombre de lignes (non modifiable)
<paramètre : y> Le nouveau nombre de colonnes (non modifiable)
<paramètre : bordure> La bordure (entier) à ajouter autour de l'image (non modifiable)
<paramètre : image_resize> Le pointeur vers la matrice resultante de ce changement
*/
void padding(const Mat& image_initiale, const int x, const int y, const int bordure, Mat& image_resize);

/*
Récupère la taille optimale pour la TF et ajoute si besoin une marge avec des 0 à gauche et en bas
<paramètre : image_initiale> Le pointeur vers la matrice contenant l'image dont on souhaite optimiser la taille (non modifiable)
<paramètre : bordure> La bordure (entier) à ajouter autour de l'image (non modifiable)
<paramètre : image_finale> Le pointeur vers la matrice resultante de ce changement
*/
void optimal_TF(const Mat& image_initiale, const int bordure, Mat& image_finale);

/*
Permet de convertir une image complexe en une image affichable (module ou phase)
<paramètre : image_initiale> Le pointeur vers la matrice complexe que l'on souhaite afficher (non modifiable)
<paramètre : image_finale> Le pointeur vers la matrice que l'on peut maintenant afficher
<paramètre : method> La méthode utilisée pour afficher l'image : soit CALCUL_MODULE pour le calcule du module soit CALCUL_PHASE pour la phase
<paramètre : echelle_log> <par défaut : false> Booléen qui permet ou non de passer l'image en échelle logarithmique : true = échelle log
*/
void affiche_complex(const Mat& image_initiale, Mat& image_finale, int method, bool echelle_log = false);

/*
Permet d'échanger les 4 quadrants de l'image (les 4 quarts de l'image)
<paramètre : image_initiale> Le pointeur vers la matrice contenant l'image dont on souhaite changer les quadrants (non modifiable)
<paramètre : image_finale> Le pointeur vers la matrice modifiée
*/
void change_quadrants(const Mat& image_initiale, Mat& image_finale);

/*
Permet d'afficher une image
<paramètre : image> Le pointeur vers la matrice de l'image à afficher (non modifiable)
<paramètre : nom> Le nom de la fenêtre dans laquelle l'image sera affichée
<paramètre : norm> <par défaut : false> Booléen qui permet ou non de normaliser l'image : true = normalisation
*/
void affiche_image(const Mat& image, string nom, bool norm = false);

/*
Permet de calculer l'exponentiel complexe d'une matrice : exp(i*M) = cos(M) + i*sin(M)
<paramètre : mat_initiale> Le pointeur vers la matrice contenant l'image dont on souhaite calculer l'exp complexe (non modifiable)
<paramètre : mat_finale> Le pointeur vers la matrice résultat
*/
void exp_complex(const Mat& mat_initiale, Mat& mat_finale);

/*
Permet de créer un kernel de propagation et de rétro-propagation en fonction des différents paramètres
<paramètre : param> Les paramètres à utiliser pour créer le kernel (non modifiable)
<paramètre : Hz> Le pointeur vers le kernel de propagation
<paramètre : H_z> Le pointeur vers le kernel de rétro-propagation
*/
void fresnel_propagator(const Parametres& param, Mat& Hz, Mat& H_z);

/*
Permet de reconstruire une image à partir d'un hologramme
<paramètre : hologramme> Le pointeur vers la matrice contenant l'hologramme que l'on souhaite reconstituer (non modifiable)
<paramètre : img_reconstituee> Le pointeur vers la matrice contenant l'image reconstituée
<paramètre : param> Les paramètres à utiliser pour reconstituer l'hologramme
<paramètre : bordure> <par défaut : 1> Correspond à l'entier qui "remplira" l'image
*/
void reconstitution(const Mat& hologramme, Mat& img_reconstituee, Parametres& param, int bordure = 1);

/*
Permet de reconstruire une image à partir d'un hologramme complexe
<paramètre : hologramme_real> Le pointeur vers la matrice contenant la partie réelle de l'hologramme que l'on souhaite reconstituer (non modifiable)
<paramètre : hologramme_imag> Le pointeur vers la matrice contenant la partie imaginaire de l'hologramme que l'on souhaite reconstituer (non modifiable)
<paramètre : img_reconstituee> Le pointeur vers la matrice contenant l'image reconstituée
<paramètre : param> Les paramètres à utiliser pour reconstituer l'hologramme
<paramètre : bordure_Re> <par défaut : 1> Correspond à l'entier de la partie réelle qui "remplira" l'image
<paramètre : bordure_Im> <par défaut : 0> Correspond à l'entier de la partie imaginaire qui "remplira" l'image
    >>>>>   Généralement, on remplie l'image avec le nombre complexe 1+0i pour obtenir un module de 1 et une phase de 0
*/
void reconstitution_cmplx(const Mat& hologramme_real, const Mat& hologramme_imag, Mat& img_reconstituee, Parametres& param, int bordure_Re = 1, int bordure_Im = 0);

/*
Permet de reconstruire une image à partir d'un hologramme à l'aide de la méthode "état de l'art"
<paramètre : hologramme> Le pointeur vers la matrice contenant l'hologramme que l'on souhaite reconstituer (non modifiable)
<paramètre : img_reconstituee> Le pointeur vers la matrice contenant l'image reconstituée
<paramètre : param> Les paramètres à utiliser pour reconstituer l'hologramme
<paramètre : nb_repetition> <par défaut : 10> Correspond au nombre de répétition à faire dans l'algorithme
*/
void reconstitution_etat_art(const Mat& hologramme, Mat& img_reconstituee, Parametres& param, int nb_repetition = 10);
