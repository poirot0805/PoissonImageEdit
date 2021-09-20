#ifndef MIXEDGRAD_H
#define MIXEDGRAD_H

#include "poissonsolver.h"
class MixedGrad : public PoissonSolver
{
public:
    MixedGrad();
    MixedGrad(const PoissonSolver &pp);
    ~MixedGrad();
    double NonlinearTerm(int p_row, int p_column, int q_row, int q_column);
};

#endif // MIXEDGRAD_H
