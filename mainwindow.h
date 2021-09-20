#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include <QAction>
#include <QMenu>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QSignalMapper>

class ChildWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    // File IO
//	void New();
    void Open();								// Open an existing file
    void Save();								// Save image to file
    void SaveAs();
    ChildWindow *CreateChildWindow();
    void SetActiveSubWindow(QWidget* window);

    // Image Processing
    void Invert();								// Invert each pixel's rgb value
    void Mirror();								// Mirror image vertically or horizontally
    void GrayScale();							// Turn image to gray-scale map
    void Restore();								// Restore image to origin

    // Poisson Image Editing
    void ChooseRect();							// Choose rectangle region
    void Paste();								// Paste rect region to object image
    void SetModeImport();
    void SetModeMixed();

private:
    void CreateActions();
    void CreateMenus();
    void CreateToolBars();
    void CreateStatusBar();

    QMdiSubWindow *FindChild(const QString &filename);
    ChildWindow* GetChildWindow();

private:
    Ui::MainWindow ui;

    QMenu						*menu_file_;
    QMenu						*menu_edit_;
    QMenu						*menu_help_;
    QToolBar					*toolbar_file_;
    QToolBar					*toolbar_edit_;
//	QAction						*action_new_;
    QAction						*action_open_;
    QAction						*action_save_;
    QAction						*action_saveas_;

    QAction						*action_invert_;
    QAction						*action_mirror_;
    QAction						*action_gray_;
    QAction						*action_restore_;

    QAction						*action_choose_polygon_;
    QAction						*action_copy_;
    QAction						*action_paste_;
    QAction                     *action_mixed;
    QAction                     *action_import;


    QMdiArea					*mdi_area_;
    QSignalMapper				*window_mapper_;

    ChildWindow					*child_source_;
};

#endif // MAINWINDOW_H
