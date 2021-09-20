#include "imagewidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets>
#include <iostream>
#include "childwindow.h"

using std::cout;
using std::endl;

ImageWidget::ImageWidget(ChildWindow* relatewindow)
{
    image_ = new QImage();
    image_backup_ = new QImage();

    draw_status_ = kNone;
    is_choosing_ = false;
    is_pasting_ = false;

    point_start_ = QPoint(0, 0);
    point_end_ = QPoint(0, 0);

    source_window_ = NULL;

    save_solver=NULL;
    cur_solver_red=NULL;
    cur_solver_green=NULL;
    cur_solver_blue=NULL;
}

ImageWidget::~ImageWidget(void)
{
    if(save_solver!=NULL){
        delete save_solver;
        save_solver=NULL;
    }
    if(cur_solver_red!=NULL){
        delete cur_solver_red;
        cur_solver_red=NULL;
    }
    if(cur_solver_green!=NULL){
        delete cur_solver_green;
        cur_solver_green=NULL;
    }
    if(cur_solver_blue!=NULL){
        delete cur_solver_blue;
        cur_solver_blue=NULL;
    }
}

int ImageWidget::ImageWidth()
{
    return image_->width();
}

int ImageWidget::ImageHeight()
{
    return image_->height();
}

void ImageWidget::set_draw_status_to_choose()
{
    draw_status_ = kChoose;
}

void ImageWidget::set_draw_status_to_paste()
{
    draw_status_ = kPaste;
    edit_mode_=kNormal;
}
void ImageWidget::set_draw_status_to_import(){
    draw_status_ = kPaste;
    edit_mode_=kSeamlessCloning;
}
void ImageWidget::set_draw_status_to_mixed(){
    draw_status_ = kPaste;
    edit_mode_=kMixingGradient;
}
QImage* ImageWidget::image()
{
    return image_;
}

void ImageWidget::set_source_window(ChildWindow* childwindow)
{
    source_window_ = childwindow;
}

void ImageWidget::paintEvent(QPaintEvent* paintevent)
{
    QPainter painter;
    painter.begin(this);

    // Draw background
    painter.setBrush(Qt::lightGray);
    QRect back_rect(0, 0, width(), height());
    painter.drawRect(back_rect);

    // Draw image
    QRect rect = QRect(0, 0, image_->width(), image_->height());
    painter.drawImage(rect, *image_);

    // Draw choose region
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::red);
    painter.drawRect(point_start_.x(), point_start_.y(),
                     point_end_.x() - point_start_.x(), point_end_.y() - point_start_.y());

    painter.end();
}

void ImageWidget::mousePressEvent(QMouseEvent* mouseevent)
{
    if (Qt::LeftButton == mouseevent->button())
    {
        switch (draw_status_)
        {
        case kChoose:
            is_choosing_ = true;
            point_start_ = point_end_ = mouseevent->pos();
            break;

        case kPaste:
        {
            is_pasting_ = true;

            // Start point in object image
            int xpos = mouseevent->pos().rx();
            int ypos = mouseevent->pos().ry();

            // Start point in source image
            int xsourcepos = source_window_->imagewidget_->point_start_.rx();
            int ysourcepos = source_window_->imagewidget_->point_start_.ry();

            // Width and Height of rectangle region
            int w = source_window_->imagewidget_->point_end_.rx()
                    - source_window_->imagewidget_->point_start_.rx() + 1;
            int h = source_window_->imagewidget_->point_end_.ry()
                    - source_window_->imagewidget_->point_start_.ry() + 1;

            // Paste
            if ((xpos + w < image_->width()) && (ypos + h < image_->height()))
            {
                // Restore image
                //	*(image_) = *(image_backup_);
                MatrixXd source_red(h, w);
                MatrixXd source_green(h, w);
                MatrixXd source_blue(h, w);
                MatrixXd target_red(h, w);
                MatrixXd target_green(h, w);
                MatrixXd target_blue(h, w);
                vector<Vector2i> vertices = vector<Vector2i>();

                vertices.push_back(Vector2i(0, 0));
                vertices.push_back(Vector2i(w - 1, 0));
                vertices.push_back(Vector2i(w - 1, h - 1));
                vertices.push_back(Vector2i(0, h - 1));

                for (int i = 0; i < w; i++)
                {
                    for (int j = 0; j < h; j++)
                    {
                        //h行w列
                        QRgb source_pixel = source_window_->imagewidget_->image()->pixel(xsourcepos + i, ysourcepos + j);
                        source_red(j, i) = qRed(source_pixel);
                        source_green(j, i) = qGreen(source_pixel);
                        source_blue(j, i) = qBlue(source_pixel);

                        QRgb target_pixel = image_->pixel(xpos + i, ypos + j);
                        target_red(j, i) = qRed(target_pixel);
                        target_green(j, i) = qGreen(target_pixel);
                        target_blue(j, i) = qBlue(target_pixel);
                    }

                }

                if(save_solver==NULL){
                    save_solver=new PoissonSolver();
                    save_solver->setImg(w,h,source_red,target_red);
                    cout<<"-----------one precompute";
                    save_solver->preCompute(vertices);
                    cout<<"2 precompute";
                }
                cout<<"||||||||||";
                if(cur_solver_red!=NULL){
                    delete cur_solver_red;
                    cur_solver_red=NULL;
                }
                if(cur_solver_green!=NULL){
                    delete cur_solver_green;
                    cur_solver_green=NULL;
                }
                if(cur_solver_blue!=NULL){
                    delete cur_solver_blue;
                    cur_solver_blue=NULL;
                }
                // Paste
                cout<<"------------start paste";
                if(edit_mode_==kNormal){    // Paste
                    for (int i = 0; i < w; i++)
                    {
                        for (int j = 0; j < h; j++)
                        {
                            image_->setPixel(xpos + i, ypos + j, source_window_->imagewidget_->image()->pixel(xsourcepos + i, ysourcepos + j));
                        }
                    }
                }
                else if(edit_mode_==kSeamlessCloning){
                    cur_solver_red=new ImportGrad(*save_solver);
                    cur_solver_red->setTarget(source_red,target_red);
                    cur_solver_red->Solve();
                    cur_solver_green=new ImportGrad(*save_solver);
                    cur_solver_green->setTarget(source_green,target_green);
                    cur_solver_green->Solve();
                    cur_solver_blue=new ImportGrad(*save_solver);
                    cur_solver_blue->setTarget(source_blue,target_blue);
                    cur_solver_blue->Solve();
                    for (int i = 0; i < w; i++)
                    {
                        for (int j = 0; j < h; j++)
                        {
                            int red = max(0, min((int)cur_solver_red->SolutionAt(i, j), 255));
                            int green = max(0, min((int)cur_solver_green->SolutionAt(i, j), 255));
                            int blue = max(0, min((int)cur_solver_blue->SolutionAt(i, j), 255));
                            image_->setPixel(xpos + i, ypos + j, qRgb(red, green, blue));
                        }
                    }
                }
                else if(edit_mode_==kMixingGradient){
                    cur_solver_red=new MixedGrad(*save_solver);
                    cur_solver_red->setTarget(source_red,target_red);
                    cur_solver_red->Solve();
                    cur_solver_green=new MixedGrad(*save_solver);
                    cur_solver_green->setTarget(source_green,target_green);
                    cur_solver_green->Solve();
                    cur_solver_blue=new MixedGrad(*save_solver);
                    cur_solver_blue->setTarget(source_blue,target_blue);
                    cur_solver_blue->Solve();
                    for (int i = 0; i < w; i++)
                    {
                        for (int j = 0; j < h; j++)
                        {
                            int red = max(0, min((int)cur_solver_red->SolutionAt(i, j), 255));
                            int green = max(0, min((int)cur_solver_green->SolutionAt(i, j), 255));
                            int blue = max(0, min((int)cur_solver_blue->SolutionAt(i, j), 255));
                            image_->setPixel(xpos + i, ypos + j, qRgb(red, green, blue));
                        }
                    }
                }
            }
        }

            update();
            break;

        default:
            break;
        }
    }
}

void ImageWidget::mouseMoveEvent(QMouseEvent* mouseevent)
{
    switch (draw_status_)
    {
    case kChoose:
        // Store point position for rectangle region
        if (is_choosing_)
        {
            point_end_ = mouseevent->pos();
        }
        break;

    case kPaste:
        // Paste rectangle region to object image
        if (is_pasting_)
        {
            // Start point in object image
            int xpos = mouseevent->pos().rx();
            int ypos = mouseevent->pos().ry();

            // Start point in source image
            int xsourcepos = source_window_->imagewidget_->point_start_.rx();
            int ysourcepos = source_window_->imagewidget_->point_start_.ry();

            // Width and Height of rectangle region
            int w = source_window_->imagewidget_->point_end_.rx()
                    - source_window_->imagewidget_->point_start_.rx() + 1;
            int h = source_window_->imagewidget_->point_end_.ry()
                    - source_window_->imagewidget_->point_start_.ry() + 1;

            // Paste
            if ((xpos > 0) && (ypos > 0) && (xpos + w < image_->width()) && (ypos + h < image_->height()))
            {
                // Restore image
                *(image_) = *(image_backup_);
                if(edit_mode_==kNormal){    // Paste
                    for (int i = 0; i < w; i++)
                    {
                        for (int j = 0; j < h; j++)
                        {
                            image_->setPixel(xpos + i, ypos + j, source_window_->imagewidget_->image()->pixel(xsourcepos + i, ysourcepos + j));
                        }
                    }
                }
                else {
                    MatrixXd source_red(h, w);
                    MatrixXd source_green(h, w);
                    MatrixXd source_blue(h, w);
                    MatrixXd target_red(h, w);
                    MatrixXd target_green(h, w);
                    MatrixXd target_blue(h, w);

                    for (int i = 0; i < w; i++)
                    {
                        for (int j = 0; j < h; j++)
                        {

                            QRgb source_pixel = source_window_->imagewidget_->image()->pixel(xsourcepos + i, ysourcepos + j);
                            source_red(j, i) = qRed(source_pixel);
                            source_green(j, i) = qGreen(source_pixel);
                            source_blue(j, i) = qBlue(source_pixel);

                            QRgb target_pixel = image_->pixel(xpos + i, ypos + j);
                            target_red(j, i) = qRed(target_pixel);
                            target_green(j, i) = qGreen(target_pixel);
                            target_blue(j, i) = qBlue(target_pixel);
                        }

                    }
                    cur_solver_red->setTarget(source_red,target_red);
                    cur_solver_red->Solve();
                    cur_solver_green->setTarget(source_green,target_green);
                    cur_solver_green->Solve();
                    cur_solver_blue->setTarget(source_blue,target_blue);
                    cur_solver_blue->Solve();
                    for (int i = 0; i < w; i++)
                    {
                        for (int j = 0; j < h; j++)
                        {
                            int red = max(0, min((int)cur_solver_red->SolutionAt(i, j), 255));
                            int green = max(0, min((int)cur_solver_green->SolutionAt(i, j), 255));
                            int blue = max(0, min((int)cur_solver_blue->SolutionAt(i, j), 255));
                            image_->setPixel(xpos + i, ypos + j, qRgb(red, green, blue));
                        }
                    }

                }

            }
        }

    default:
        break;
    }

    update();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* mouseevent)
{
    switch (draw_status_)
    {
    case kChoose:
        if (is_choosing_)
        {
            point_end_ = mouseevent->pos();
            is_choosing_ = false;
            draw_status_ = kNone;
        }

    case kPaste:
        if (is_pasting_)
        {
            is_pasting_ = false;
            draw_status_ = kNone;
        }

    default:
        break;
    }

    update();
}

void ImageWidget::Open(QString filename)
{
    // Load file
    if (!filename.isEmpty())
    {
        image_->load(filename);
        *(image_backup_) = *(image_);
    }

    //	setFixedSize(image_->width(), image_->height());
    //	relate_window_->setWindowFlags(Qt::Dialog);
    //	relate_window_->setFixedSize(QSize(image_->width(), image_->height()));
    //	relate_window_->setWindowFlags(Qt::SubWindow);

    //image_->invertPixels(QImage::InvertRgb);
    //*(image_) = image_->mirrored(true, true);
    //*(image_) = image_->rgbSwapped();
    cout << "image size: " << image_->width() << ' ' << image_->height() ;
    update();
}

void ImageWidget::Save()
{
    SaveAs();
}

void ImageWidget::SaveAs()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
    if (filename.isNull())
    {
        return;
    }

    image_->save(filename);
}

void ImageWidget::Invert()
{
    for (int i = 0; i < image_->width(); i++)
    {
        for (int j = 0; j < image_->height(); j++)
        {
            QRgb color = image_->pixel(i, j);
            image_->setPixel(i, j, qRgb(255 - qRed(color), 255 - qGreen(color), 255 - qBlue(color)));
        }
    }

    // equivalent member function of class QImage
    // image_->invertPixels(QImage::InvertRgb);
    update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
    QImage image_tmp(*(image_));
    int width = image_->width();
    int height = image_->height();

    if (ishorizontal)
    {
        if (isvertical)
        {
            for (int i = 0; i < width; i++)
            {
                for (int j = 0; j < height; j++)
                {
                    image_->setPixel(i, j, image_tmp.pixel(width - 1 - i, height - 1 - j));
                }
            }
        }
        else
        {
            for (int i = 0; i < width; i++)
            {
                for (int j = 0; j < height; j++)
                {
                    image_->setPixel(i, j, image_tmp.pixel(i, height - 1 - j));
                }
            }
        }

    }
    else
    {
        if (isvertical)
        {
            for (int i = 0; i < width; i++)
            {
                for (int j = 0; j < height; j++)
                {
                    image_->setPixel(i, j, image_tmp.pixel(width - 1 - i, j));
                }
            }
        }
    }

    // equivalent member function of class QImage
    //*(image_) = image_->mirrored(true, true);
    update();
}

void ImageWidget::TurnGray()
{
    for (int i = 0; i < image_->width(); i++)
    {
        for (int j = 0; j < image_->height(); j++)
        {
            QRgb color = image_->pixel(i, j);
            int gray_value = (qRed(color) + qGreen(color) + qBlue(color)) / 3;
            image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value));
        }
    }

    update();
}

void ImageWidget::Restore()
{
    *(image_) = *(image_backup_);
    point_start_ = point_end_ = QPoint(0, 0);
    update();
}
