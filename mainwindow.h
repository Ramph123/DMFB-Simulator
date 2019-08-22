#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QMessageBox>
#include <QIcon>
#include "dialog.h"
#include "chip.h"
#include "filemanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void initTime();
    void setTime(int);

private:
    Ui::MainWindow *ui;
    Dialog *initDialog;
    chip *bioChip;
    fileManager *fManager;

    // Menu & Toolbar - Actions & Implements
    QAction *initAction;
    QAction *stepForwardAction;
    QAction *stepNextAction;
    QAction *playAllAction;
    QAction *openAction;
    QAction *resetAction;
    void init();
    void stepForward();
    void stepNext();
    void playAll();
    void open();
    void reset();
};

#endif // MAINWINDOW_H
