#include "poissonsolver.h"

PoissonSolver::PoissonSolver()
{

}
PoissonSolver::PoissonSolver(const PoissonSolver &pp){
    width_ = pp.width_;						// the width of the envelope rectangle
    height_ = pp.height_;					// the height of the envelope rectngle
    point_count_ = pp.point_count_;				// the number of pixels in the region
    status_ = pp.status_;			// 记录点的状态
    mIndex = pp.mIndex;			// pixels对应的index
    src_val = pp.src_val;		// the value in the source image
    dst_val = pp.dst_val;		// the value in the target image
    solution_ = pp.solution_;			// the vector for solution列向量

    A=pp.A;
    b=pp.b;
    solver.compute(A);
}
PoissonSolver::~PoissonSolver()
{

}
void PoissonSolver::setImg(int w,int h,MatrixXd source_value, MatrixXd target_value){
    src_val=source_value;
    dst_val=target_value;
    status_.resize(h,w);
    mIndex.resize(h,w);
    width_=w;
    height_=h;
    // initialize status_, index_ matrix
    for (int i = 0; i < height_; i++)
    {
        for (int j = 0; j < width_; j++)
        {
            status_(i, j) = kExterior;
            mIndex(i, j) = -1;
        }
    }
    point_count_ = 0;
}
void PoissonSolver::setTarget(MatrixXd source_value,MatrixXd target_value){
    src_val=source_value;
    dst_val=target_value;
}
void PoissonSolver::preCompute(vector<Vector2i>& vertices){
    setRect(vertices);
    //setScanLine(vertices);
    cout <<"------------------ScanLine finished!"<<endl;
    cout <<"heigh:"<<height_<<", width:"<<width_<<endl;

    int ttc = A.cols();
    for (int i = 0; i < height_; i++)
    {
        for (int j = 0; j < width_; j++)
        {
            // for point-β
            if(status_(i, j) == kBoundary){
                A.insert(mIndex(i, j), mIndex(i, j)) =1;
            }
            if (status_(i, j) == kInterior)
            {
                A.insert(mIndex(i, j), mIndex(i, j)) = 4;   //A(β,β)=4;
                if ((i == 0) || (i == height_ - 1))
                {
                    A.coeffRef(mIndex(i, j), mIndex(i, j))--;   //邻居
                }
                if ((j == 0) || (j == width_ - 1))
                {
                    A.coeffRef(mIndex(i, j), mIndex(i, j))--;
                }
                b(mIndex(i, j)) = 0;
                int test1=A.coeffRef(mIndex(i, j), mIndex(i, j));
                int test2=0;
                // neighborhood
                int row_array[4] = { 0, 0, -1, 1 };
                int column_array[4] = { -1, 1, 0, 0 };
                for (int n = 0; n < 4; n++)
                {
                    int row = i + row_array[n];
                    int column = j + column_array[n];
                    if ((row < 0) || (row > height_ - 1))
                    {
                        continue;
                    }
                    if ((column < 0) || (column > width_ - 1))
                    {
                        continue;
                    }

                    b(mIndex(i, j)) += NonlinearTerm(i, j, row, column);    // Vpq
                    if (status_(row, column) == kInterior)
                    {
                        A.insert(mIndex(i, j), mIndex(row, column)) = -1;
                        test2++;
                    }
                    else
                    {
                        b(mIndex(i, j)) += dst_val(row, column);    //边界邻居
                    }
                }
                if(test1==test2) cout<<"YES"<<endl;
                else cout<<"NO"<<endl;

            }
        }
    }

    solver.compute(A);
    if (solver.info() != Success)
    {
        cout << "Compute Matrix is error" << endl;
        return;
    }
    else cout<<"final!"<<endl;
}
void PoissonSolver::Solve(){
    for(int i=0;i<point_count_;i++){
        b(i)=0;
    }
    // cout<<A;
    for (int i = 0; i < height_; i++)
    {
        for (int j = 0; j < width_; j++)
        {
            // for point-β
            if(status_(i, j) == kBoundary){
                b(mIndex(i, j))=dst_val(i,j);
            }
            if (status_(i, j) == kInterior)
            {
                b(mIndex(i, j)) = 0;

                // neighborhood
                int row_array[4] = { 0, 0, -1, 1 };
                int column_array[4] = { -1, 1, 0, 0 };
                for (int n = 0; n < 4; n++)
                {
                    int row = i + row_array[n];
                    int column = j + column_array[n];
                    if ((row < 0) || (row > height_ - 1))
                    {
                        continue;
                    }
                    if ((column < 0) || (column > width_ - 1))
                    {
                        continue;
                    }

                    b(mIndex(i, j)) += NonlinearTerm(i, j, row, column);    // Vpq
                    //cout<< NonlinearTerm(i, j, row, column)<<endl;
                    if (status_(row, column) == kInterior)
                    {
                        //A.insert(mIndex(i, j), mIndex(row, column)) = -1;
                    }
                    else if(status_(row, column) == kBoundary)
                    {
                        b(mIndex(i, j)) += dst_val(row, column);    //边界邻居
                    }
                }
            }
        }
    }
    solver.compute(A);
    solution_ = solver.solve(b);
}
double PoissonSolver::SolutionAt(int x,int y){
    int index = mIndex(y, x);
    if ((index >= 0) && (index < point_count_))
    {
        return solution_(index);
    }
    else
    {
        return dst_val(y, x);
    }
}

double	PoissonSolver::NonlinearTerm(int p_row, int p_column, int q_row, int q_column)
{
    // cout << "poisson" << endl;
    return 0;
}

void PoissonSolver::setRect(vector<Vector2i>& vertices){
    for (int i = 0; i < height_; i++)
    {
        for (int j = 0; j < width_; j++)
        {
            status_(i, j) = kInterior;
            mIndex(i, j) = point_count_++;
        }
    }
    for(int i=0;i<height_;i++){
        status_(i,0)=kBoundary;
        status_(i,width_-1)=kBoundary;
    }
    for (int j = 0; j < width_; j++)
    {
        status_(0, j) = kBoundary;
        status_(height_-1,j)=kBoundary;
    }
    cout<<"rect-before"<<endl;
    A.resize(point_count_,point_count_);
    b.resize(point_count_);
}
void PoissonSolver::setScanLine(vector<Vector2i>& vertices)
{
    //0-max_y直接按照vertice给的坐标赋值的！
    //初始化ET
    int max_y=0;

    vector<Vector2i> flat_vertice;
    for(int i=vertices.size()-1, j=0;j< vertices.size();i=j,j++){
        int temp=min(vertices[i](1),vertices[j](1));
                if(temp>max_y) max_y=temp;
    }
    edgeTable.resize(max_y+1);
    for(int i=vertices.size()-1, j=0;j< vertices.size();i=j,j++){
        int tid=i;//下端点
        int tid2=j;//上端点
        if(vertices[i](1)>vertices[j](1)) {tid=j;tid2=i;}
        int temp_y=vertices[tid](1);
        int temp_x=vertices[tid](0);
        int my=vertices[tid2](1);
        int mx=vertices[tid2](0);
        Edge edge;
        edge.x=temp_x;
        edge.ymax=my;
        edge.dx=(temp_x-mx)*1.0/(temp_y-my);
        if(temp_y!=my) edgeTable[temp_y].push_back(edge);
        else {
            flat_vertice.push_back(vertices[i]);
            flat_vertice.push_back(vertices[j]);
        }
    }
    for(int i=0;i<max_y+1;i++){
        //更新AET
        vector<Edge>::iterator it=edgeTable[i].begin();
        while(it!=edgeTable[i].end()){
            activeET.push_back(*it);
        }
        sort(activeET.begin(),activeET.end());
        // 设置内外点
        it=activeET.begin();
        vector<Edge>::iterator it2=it;    it2++;
        int pivot_x1,pivot_x2;
        while(it!=activeET.end() && it2!=activeET.end()){
            pivot_x1=it->x; pivot_x2=it2->x;
            for(int j=pivot_x1+1;j<pivot_x2;j++){
                status_(i,j)=kInterior;
            }
            status_(i,pivot_x1)=status_(i,pivot_x2)=kBoundary;
            it++;it2++;it++;it2++;
        }
        it=activeET.begin();
        while(it!=activeET.end()){
            if(it->ymax>=i){
                it=activeET.erase(it);
            }
            else{
                it->x=it->x+it->dx;
                it++;
            }
        }
    }
    for(int i=0,j=1;i<flat_vertice.size() && j<flat_vertice.size();i+=2,j+=2){
        int pivot_x1,pivot_x2,pivot_y;
        pivot_x1=min(flat_vertice[i](0),flat_vertice[j](0));
                pivot_x2=max(flat_vertice[i](0),flat_vertice[j](0));
                pivot_y=flat_vertice[i](1);
        for(int k=pivot_x1;k<=pivot_x2;k++){
            status_(pivot_y,k)=kBoundary;
        }
    }
    for (int i = 0; i < height_; i++)
    {
        for (int j = 0; j < width_; j++)
        {
            if(status_(i,j)!=kExterior){
                mIndex(i,j)=point_count_++;
            }
        }
    }
    A.resize(point_count_,point_count_);
    b.resize(point_count_);
    /*
    for (int i = 0; i < height_; i++)
    {
        int y = i;
        // update and delete edges
        list<Edge>::iterator it = edge_list.begin();
        while (it != edge_list.end())
        {
            if (y > it->ymax)
            {
                it = edge_list.erase(it);
            }
            else
            {
                it->x += it->dx;
                it++;
            }
        }

        // insert new edges
        for (int j = vertices.size() - 1, k = 0; k < vertices.size(); j = k, k++)
        {
            if (((vertices[j](1) == y) && (vertices[k](1) > y))
                || ((vertices[j](1) > y) && (vertices[k](1) == y)))
            {
                Edge edge;
                edge.dx = ((double)(vertices[j](0) - vertices[k](0))) / (vertices[j](1) - vertices[k](1));
                if (vertices[j](1) == y)
                {
                    edge.x = vertices[j](0);
                    edge.ymax = vertices[k][1];

                    // left or right vertex
                    int l = ((k == vertices.size() - 1) ? 0 : k + 1);
                    if (vertices[l](1) > vertices[k](1))
                    {
                        edge.ymax--;
                    }
                }
                else
                {
                    edge.x = vertices[k](0);
                    edge.ymax = vertices[j][1];

                    // left or right vertex
                    int l = ((j == 0) ? vertices.size() - 1 : j - 1);
                    if (vertices[l](1) > vertices[j](1))
                    {
                        edge.ymax--;
                    }
                }

                list<Edge>::iterator itr = edge_list.begin();
                while (itr != edge_list.end())
                {
                    if (itr->x < edge.x)
                    {
                        itr++;
                    }
                    else
                    {
                        break;
                    }
                }
                edge_list.insert(itr, edge);
            }
        }

        list<Edge>::iterator itr1 = edge_list.begin();
        for (int k = 0; k < edge_list.size() / 2; k++)
        {
            list<Edge>::iterator itr2 = itr1;
            itr2++;
            for (int j = (int)itr1->x; j <= (int)itr2->x; j++)
            {
                mIndex(i, j) = point_count_;
                point_count_++;
                if ((j == (int)itr1->x) || (j == (int)itr2->x))
                {
                    status_(i, j) = kBoundary;
                }
                else
                {
                    status_(i, j) = kInterior;
                }
            }
            itr1++;
            itr1++;
        }
    }

    // flat edge correction
    for (int i = vertices.size() - 1, j = 0; j < vertices.size(); i = j, j++)
    {
        if (vertices[i](1) == vertices[j](1))
        {
            for (int k = min(vertices[i](0), vertices[j](0)); k <= max(vertices[i](0), vertices[j](0)); k++)
            {
                if (mIndex(vertices[i](1), k) != -1)
                {
                    status_(vertices[i](1), k) = kBoundary;
                }
            }
        }
    }
    */
}
