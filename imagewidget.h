#pragma once
#include <QWidget>
#include "poissonsolver.h"
#include "importgrad.h"
#include "mixedgrad.h"
#include <iostream>
class ChildWindow;
QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
QT_END_NAMESPACE
using namespace std;
using namespace Eigen;
enum DrawStatus
{
    kChoose,
    kPaste,
    kNone
};

enum Mode
{
    kNormal,
    kSeamlessCloning,
    kMixingGradient
};

class ImageWidget :
    public QWidget
{
    Q_OBJECT

public:
    ImageWidget(ChildWindow *relatewindow);
    ~ImageWidget(void);

    int ImageWidth();											// Width of image
    int ImageHeight();											// Height of image
    void set_draw_status_to_choose();
    void set_draw_status_to_paste();
    void set_draw_status_to_import();
    void set_draw_status_to_mixed();
    QImage* image();
    void set_source_window(ChildWindow* childwindow);

protected:
    void paintEvent(QPaintEvent *paintevent);
    void mousePressEvent(QMouseEvent *mouseevent);
    void mouseMoveEvent(QMouseEvent *mouseevent);
    void mouseReleaseEvent(QMouseEvent *mouseevent);

public slots:
    // File IO
    void Open(QString filename);								// Open an image file, support ".bmp, .png, .jpg" format
    void Save();												// Save image to current file
    void SaveAs();												// Save image to another file

    // Image processing
    void Invert();												// Invert pixel value in image
    void Mirror(bool horizontal=false, bool vertical=true);		// Mirror image vertically or horizontally
    void TurnGray();											// Turn image to gray-scale map
    void Restore();												// Restore image to origin

public:
    QPoint						point_start_;					// Left top point of rectangle region
    QPoint						point_end_;						// Right bottom point of rectangle region

private:
    QImage						*image_;						// image
    QImage						*image_backup_;

    // Pointer of child window
    ChildWindow					*source_window_;				// Source child window

    // Signs
    DrawStatus					draw_status_;					// Enum type of draw status
    Mode                        edit_mode_;                     // normalcopy/import/mixed
    bool						is_choosing_;
    bool						is_pasting_;

    PoissonSolver* save_solver; //存储数据
    PoissonSolver* cur_solver_red;  //根据方法变换
    PoissonSolver* cur_solver_green;
    PoissonSolver* cur_solver_blue;

};

