#include "optilocale.h"

Eigen::MatrixXd corr_w_dc(Eigen::MatrixXd w, Eigen::MatrixXd g, Eigen::MatrixXd d)
{
	Eigen::MatrixXd res(2,1);

	double ws = w.sum();
	auto wxg = w.array()*g.array();
	double wg = wxg.matrix().sum();
	double wg2 = (wxg*g.array()).matrix().sum();
	double wrg = (wrg*d.array()).matrix().sum();

	res(0, 0) = wrg / (wrg - wg * wg / ws);
	res(1, 0) = res(0, 0) * wg / ws;

	return res;
}
