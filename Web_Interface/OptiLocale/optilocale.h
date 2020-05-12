#ifndef OPTI_LOCALE_H
#define OPTI_LOCALE_H

#include <Eigen/Dense>

/*
Fonction permettant de calculer le coefficient de correlation entre un modele et des donn�es en prenant en compte
un masque BINAIRE. Calcul �galement de l'offset des donn�es par rapport
au modele.

Parametres d'entr�e :
w : masque de m�me taille que d (2D)
g : modeles de m�me taille que d (2D)
d : data ( 2D) --> centr�
Param�tres de sortie :
Vecteur colonne res.
res(0, 0) = alpha : coefficient de correlation d~I_p+alpha*g;
res(1, 0) = I_p
*/
Eigen::MatrixXd corr_w_dc(Eigen::MatrixXd w, Eigen::MatrixXd g, Eigen::MatrixXd d);

#endif