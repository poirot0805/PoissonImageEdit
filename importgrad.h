#ifndef IMPORTGRAD_H
#define IMPORTGRAD_H
#include "poissonsolver.h"

class ImportGrad : public PoissonSolver
{
public:
    ImportGrad();
    ImportGrad(const PoissonSolver &pp);
    ~ImportGrad();
    double NonlinearTerm(int p_row, int p_column, int q_row, int q_column);
};

#endif // IMPORTGRAD_H
