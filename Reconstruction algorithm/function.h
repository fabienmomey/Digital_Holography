#pragma once
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

using namespace std;
using namespace cv;

enum MethodeCalcul //Méthode de calcul pour des complexes
{
    CALCUL_MODULE,  //Pour le calcul du module
    CALCUL_PHASE    //Pour le calcul de la phase
};

enum TYPE_HOLOGRAM //Le type d'hologramme
{
    COMPLEXE,   //Pour un hologramme complexe
    INTENSITE   //Pour un hologramme intensité
};

enum TYPE_OBJ //Le type d'objet
{
    DEPHASING,  //Si l'objet est purement déphasant
    ABSORBING   //Si l'objet est purement absorbant
};

enum KERNEL_TYPE
{
    HZ,
    GZ_DEPHASING,
    GZ_ABSORBING
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

    double z = 10e-6;                   // Distance du capteur du plan (m)
    double mag = 50;                    // Grossissement de l'objectif
    double lambda = 600e-9;             // Longueur d'onde (m)
    double n0 = 1;                      // Indice de réfraction moyen (non obligatoire)

    // DIGITAL

    double pixel_size = 1e-6 / mag;     // Taille des pixels (m)
    int width = 1024;                   // Largeur de la vue en pixels
    int height = 1024;                  // Hauteur de la vue en pixels
};

/*
Calcul l'histogramme pour une image (couleur ou niveau de gris)
<paramètre : image_initiale> Le pointeur vers la matrice contenant l'image dont on souhaite faire l'histogramme (non modifiable)
<paramètre : image_hist> Le pointeur vers la matrice contenant l'histogramme
*/
void calcul_histogramme(const Mat& image_initiale, Mat& image_hist);

/*
Multiplie 2 matrices éléments par éléments dont une matrice est réelle et l'autre en notation complexe : M * (A + iB) = MA + iMB
<paramètre : mat_simple> Le pointeur vers la matrice réelle à multiplier (non modifiable)
<paramètre : mat_complexe> Le pointeur vers la matrice complexe à multiplier (non modifiable)
<paramètre : mat_res> Le pointeur vers la matrice résultat de la multiplication
*/
void my_multiply(const Mat& mat_simple, const Mat& mat_complexe, Mat& mat_res);

/*
Multiplie 2 matrices en notation complexe éléments par éléments : (A + iB) * (X + iY) = (AX - BY) + i(AY + BX)
<paramètre : mat1> Le pointeur vers la première matrice à multiplier (non modifiable)
<paramètre : mat2> Le pointeur vers la seconde matrice à multiplier (non modifiable)
<paramètre : mat_res> Le pointeur vers la matrice résultat de la multiplication
*/
void multiplier_cmplx(const Mat& mat1, const Mat& mat2, Mat& mat_res);

/*
SURCHARGE
Multiplie une matrice complexe par un nombre complexe éléments par éléments : (A + iB) * (X + iY) = (AX - BY) + i(AY + BX)
<paramètre : scalar_cmplx> Le nombre complexe à multiplier (non modifiable)
<paramètre : mat_complexe> Le pointeur vers la matrice complexe à multiplier (non modifiable)
<paramètre : mat_res> Le pointeur vers la matrice résultat de la multiplication
*/
void multiplier_cmplx(const complex<double>& scalar_cmplx, const Mat& mat_complexe, Mat& mat_res);

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
void affiche_complex(const Mat& image_initiale, Mat& image_finale, MethodeCalcul method, bool echelle_log = false);

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
Permet de créer un kernel de propagation et de rétro-propagation en fonction des différents paramètres
<paramètre : param> Les paramètres à utiliser pour créer le kernel (non modifiable)
<paramètre : Hz> Le pointeur vers le kernel de propagation
<paramètre : H_z> Le pointeur vers le kernel de rétro-propagation
<paramètre : flag_phaseref> <par défaut : false>
<paramètre : type_hologram> <par défaut : COMPLEXE>
<paramètre : flag_linearize> <par défaut : false>
<paramètre : type_obj> <par défaut : DEPHASING>
*/
void kernel_propagation_fresnel(const Parametres& param, Mat& Hz, Mat& H_z, bool flag_phaseref = false, TYPE_HOLOGRAM type_hologram = COMPLEXE, bool flag_linearize = false, TYPE_OBJ type_obj = DEPHASING);

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
Permet de reconstruire une image à partir d'un hologramme à l'aide de la méthode "fienup"
<paramètre : hologramme> Le pointeur vers la matrice contenant l'hologramme que l'on souhaite reconstituer (non modifiable)
<paramètre : img_reconstituee> Le pointeur vers la matrice contenant l'image reconstituée
<paramètre : param> Les paramètres à utiliser pour reconstituer l'hologramme
<paramètre : nb_repetition> <par défaut : 10> Correspond au nombre de répétition à faire dans l'algorithme
*/
void reconstitution_fienup(const Mat& hologramme, Mat& img_reconstituee, Parametres& param, int nb_repetition = 10);
