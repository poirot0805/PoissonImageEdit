#include "mixedgrad.h"

MixedGrad::MixedGrad()
{

}

MixedGrad::MixedGrad(const PoissonSolver &pp):PoissonSolver(pp)
{

}

MixedGrad::~MixedGrad(){

}

double	MixedGrad::NonlinearTerm(int p_row, int p_column, int q_row, int q_column)
{
    //cout<<"Mix"<<endl;
    double x = src_val(p_row, p_column) - src_val(q_row, q_column);
    double y = dst_val(p_row, p_column) - dst_val(q_row, q_column);
    return abs(x) > abs(y) ? x : y;
}

