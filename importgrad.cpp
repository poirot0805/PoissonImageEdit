#include "importgrad.h"

ImportGrad::ImportGrad()
{

}

ImportGrad::ImportGrad(const PoissonSolver& pp):PoissonSolver(pp)
{

}
ImportGrad::~ImportGrad(){

}

double	ImportGrad::NonlinearTerm(int p_row, int p_column, int q_row, int q_column)
{
    return src_val(p_row, p_column) - src_val(q_row, q_column);
}
