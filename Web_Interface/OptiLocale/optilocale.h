#ifndef OPTI_LOCALE_H
#define OPTI_LOCALE_H

#include <Eigen/Dense>

/*
Fonction permettant de calculer le coefficient de correlation entre un modele et des données en prenant en compte
un masque BINAIRE. Calcul également de l'offset des données par rapport
au modele.

Parametres d'entrée :
w : masque de même taille que d (2D)
g : modeles de même taille que d (2D)
d : data ( 2D) --> centré
Paramètres de sortie :
Vecteur colonne res.
res(0, 0) = alpha : coefficient de correlation d~I_p+alpha*g;
res(1, 0) = I_p
*/
Eigen::MatrixXd corr_w_dc(Eigen::MatrixXd w, Eigen::MatrixXd g, Eigen::MatrixXd d);

#endif