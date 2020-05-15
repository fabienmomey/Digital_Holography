#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

enum MethodeCalcul {
    CALCUL_MODULE = 0,  //Pour le calcul du module
    CALCUL_PHASE = 1    //Pour le calcul de la phase
};

/*
Type compos� qui renseigne tous les param�tres n�cessaires pour cr�er ou reconstituer un hologramme
/!\ Initialisation avec des param�tres al�atoires � modifier
<champ : z> Distance du capteur du plan (m)
<champ : mag> Grossissement de l'objectif
<champ : lambda> Longueur d'onde (m)
<champ : n0> Indice de r�fraction moyen (non obligatoire)
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
<param�tre : image_initiale> Le pointeur vers la matrice contenant l'image dont on souhaite faire l'histogramme (non modifiable)
<param�tre : image_hist> Le pointeur vers la matrice contenant l'histogramme
*/
void calcul_histogramme(const Mat& image_initiale, Mat& image_hist);

//void transformee_fourier(const Mat& image_initiale, Mat& im_fourier, Mat& phase_img);
//void transformee_fourier(const Mat& image_initiale, const Size& new_taille, Mat& im_fourier, Mat& phase_img);
//void TF_inverse(const Mat& image_initiale, const Mat& phase_img, Mat& tf_inverse);
//void convolution(const Mat& image_initiale, const Mat& kernel, Mat& im_convolution);

/*
Multiplie 2 images en notation complexe : (A + iB) * (X + iY) = (AX - BY) + i(AY + BX)
<param�tre : mat1> Le pointeur vers la premi�re matrice � multiplier (non modifiable)
<param�tre : mat2> Le pointeur vers la seconde matrice � multiplier (non modifiable)
<param�tre : mat_res> Le pointeur vers la matrice r�sultat de la multiplication
*/
void multiplier_cmplx(const Mat& mat1, const Mat& mat2, Mat& mat_res);

/*
Place l'image au milieu et ajoute des bordures autour
<param�tre : image_initiale> Le pointeur vers la matrice contenant l'image dont on souhaite agrandir la taille (non modifiable)
<param�tre : x> Le nouveau nombre de lignes (non modifiable)
<param�tre : y> Le nouveau nombre de colonnes (non modifiable)
<param�tre : bordure> La bordure (entier) � ajouter autour de l'image (non modifiable)
<param�tre : image_resize> Le pointeur vers la matrice resultante de ce changement
*/
void padding(const Mat& image_initiale, const int x, const int y, const int bordure, Mat& image_resize);

/*
R�cup�re la taille optimale pour la TF et ajoute si besoin une marge avec des 0 � gauche et en bas
<param�tre : image_initiale> Le pointeur vers la matrice contenant l'image dont on souhaite optimiser la taille (non modifiable)
<param�tre : bordure> La bordure (entier) � ajouter autour de l'image (non modifiable)
<param�tre : image_finale> Le pointeur vers la matrice resultante de ce changement
*/
void optimal_TF(const Mat& image_initiale, const int bordure, Mat& image_finale);

/*
Permet de convertir une image complexe en une image affichable (module ou phase)
<param�tre : image_initiale> Le pointeur vers la matrice complexe que l'on souhaite afficher (non modifiable)
<param�tre : image_finale> Le pointeur vers la matrice que l'on peut maintenant afficher
<param�tre : method> La m�thode utilis�e pour afficher l'image : soit CALCUL_MODULE pour le calcule du module soit CALCUL_PHASE pour la phase
<param�tre : echelle_log> <par d�faut : false> Bool�en qui permet ou non de passer l'image en �chelle logarithmique : true = �chelle log
*/
void affiche_complex(const Mat& image_initiale, Mat& image_finale, int method, bool echelle_log = false);

/*
Permet d'�changer les 4 quadrants de l'image (les 4 quarts de l'image)
<param�tre : image_initiale> Le pointeur vers la matrice contenant l'image dont on souhaite changer les quadrants (non modifiable)
<param�tre : image_finale> Le pointeur vers la matrice modifi�e
*/
void change_quadrants(const Mat& image_initiale, Mat& image_finale);

/*
Permet d'afficher une image
<param�tre : image> Le pointeur vers la matrice de l'image � afficher (non modifiable)
<param�tre : nom> Le nom de la fen�tre dans laquelle l'image sera affich�e
<param�tre : norm> <par d�faut : false> Bool�en qui permet ou non de normaliser l'image : true = normalisation
*/
void affiche_image(const Mat& image, string nom, bool norm = false);

/*
Permet de calculer l'exponentiel complexe d'une matrice : exp(i*M) = cos(M) + i*sin(M)
<param�tre : mat_initiale> Le pointeur vers la matrice contenant l'image dont on souhaite calculer l'exp complexe (non modifiable)
<param�tre : mat_finale> Le pointeur vers la matrice r�sultat
*/
void exp_complex(const Mat& mat_initiale, Mat& mat_finale);

/*
Permet de cr�er un kernel de propagation et de r�tro-propagation en fonction des diff�rents param�tres
<param�tre : param> Les param�tres � utiliser pour cr�er le kernel (non modifiable)
<param�tre : Hz> Le pointeur vers le kernel de propagation
<param�tre : H_z> Le pointeur vers le kernel de r�tro-propagation
*/
void fresnel_propagator(const Parametres& param, Mat& Hz, Mat& H_z);

/*
Permet de reconstruire une image � partir d'un hologramme
<param�tre : hologramme> Le pointeur vers la matrice contenant l'hologramme que l'on souhaite reconstituer (non modifiable)
<param�tre : img_reconstituee> Le pointeur vers la matrice contenant l'image reconstitu�e
<param�tre : param> Les param�tres � utiliser pour reconstituer l'hologramme
<param�tre : bordure> <par d�faut : 1> Correspond � l'entier qui "remplira" l'image
*/
void reconstitution(const Mat& hologramme, Mat& img_reconstituee, Parametres& param, int bordure = 1);

/*
Permet de reconstruire une image � partir d'un hologramme complexe
<param�tre : hologramme_real> Le pointeur vers la matrice contenant la partie r�elle de l'hologramme que l'on souhaite reconstituer (non modifiable)
<param�tre : hologramme_imag> Le pointeur vers la matrice contenant la partie imaginaire de l'hologramme que l'on souhaite reconstituer (non modifiable)
<param�tre : img_reconstituee> Le pointeur vers la matrice contenant l'image reconstitu�e
<param�tre : param> Les param�tres � utiliser pour reconstituer l'hologramme
<param�tre : bordure_Re> <par d�faut : 1> Correspond � l'entier de la partie r�elle qui "remplira" l'image
<param�tre : bordure_Im> <par d�faut : 0> Correspond � l'entier de la partie imaginaire qui "remplira" l'image
    >>>>>   G�n�ralement, on remplie l'image avec le nombre complexe 1+0i pour obtenir un module de 1 et une phase de 0
*/
void reconstitution_cmplx(const Mat& hologramme_real, const Mat& hologramme_imag, Mat& img_reconstituee, Parametres& param, int bordure_Re = 1, int bordure_Im = 0);

/*
Permet de reconstruire une image � partir d'un hologramme � l'aide de la m�thode "�tat de l'art"
<param�tre : hologramme> Le pointeur vers la matrice contenant l'hologramme que l'on souhaite reconstituer (non modifiable)
<param�tre : img_reconstituee> Le pointeur vers la matrice contenant l'image reconstitu�e
<param�tre : param> Les param�tres � utiliser pour reconstituer l'hologramme
<param�tre : nb_repetition> <par d�faut : 10> Correspond au nombre de r�p�tition � faire dans l'algorithme
*/
void reconstitution_etat_art(const Mat& hologramme, Mat& img_reconstituee, Parametres& param, int nb_repetition = 10);