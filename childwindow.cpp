#include "imagewidget.h"
#include "childwindow.h"

ChildWindow::ChildWindow(void)
{
    imagewidget_ = new ImageWidget(this);
    setCentralWidget(imagewidget_);
}


ChildWindow::~ChildWindow(void)
{
}

bool ChildWindow::LoadFile(QString filename)
{
    imagewidget_->Open(filename);
    return true;
}
