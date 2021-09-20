#ifndef POISSONSOLVER_H
#define POISSONSOLVER_H
#include <iostream>
#include <vector>
#include <Eigen\Dense>
#include <Eigen\Sparse>

using namespace std;
using namespace Eigen;

enum PointStatus
{
    kExterior,
    kBoundary,
    kInterior
};
typedef Matrix<PointStatus, Dynamic, Dynamic> sMatrix;  // 点的类型

class PoissonSolver
{
public:
    PoissonSolver();
    PoissonSolver(const PoissonSolver &pp);
    virtual ~PoissonSolver();
public:
    double SolutionAt(int x, int y);			// solution at given row-y and column-x
    void Solve();
    void preCompute(vector<Vector2i>& vertices);
    void setImg(int w,int h, MatrixXd source_value, MatrixXd target_value);
    void setTarget(MatrixXd source_value,MatrixXd target_value);
protected:
    void setRect(vector<Vector2i>& vertices);
    void setScanLine(vector<Vector2i>& vertices);										// the scan line algorithm for polygon rasterization
    virtual double NonlinearTerm(int p_row, int p_column, int q_row, int q_column);
protected:
    int			width_ = 0;						// the width of the envelope rectangle
    int			height_ = 0;					// the height of the envelope rectngle
    int			point_count_ = 0;				// the number of pixels in the region
    sMatrix     status_ = sMatrix();			// 记录点的状态
    MatrixXi	mIndex = MatrixXi();			// pixels对应的index
    MatrixXd	src_val = MatrixXd();		// the value in the source image
    MatrixXd	dst_val = MatrixXd();		// the value in the target image
    VectorXd	solution_ = VectorXd();			// the vector for solution列向量

    SparseMatrix<double, RowMajor> A;
    VectorXd b;
    SparseLU<SparseMatrix<double, RowMajor> > solver;
protected:
    struct Edge
    {
        double x = 0;
        double dx = 0;
        int ymax = 0;
        friend bool operator<(const Edge& e1, const Edge& e2)
        {
            if(e1.x!=e2.x) return e1.x<e2.x;
            return e1.dx<e2.dx;
        }
    };
    vector< vector<Edge> > edgeTable;
    vector<Edge> activeET;
};

#endif // POISSONSOLVER_H
