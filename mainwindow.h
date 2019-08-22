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
#include "washdialog.h"

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

    void setWashEnable(int);
    void setWashInput(int, int);
    void setWashOutput(int, int);
    void checkWashValid();

private:
    Ui::MainWindow *ui;
    Dialog *initDialog;
    chip *bioChip;
    fileManager *fManager;
    washDialog *washConfig;

    // Menu & Toolbar - Actions & Implements
    QAction *initAction;
    QAction *washAction;
    QAction *stepForwardAction;
    QAction *stepNextAction;
    QAction *playAllAction;
    QAction *openAction;
    QAction *resetAction;
    QAction *inspectPollutionAction;
    void init();
    void stepForward();
    void stepNext();
    void playAll();
    void open();
    void reset();
    void checkPollution();
    void washConfigure();
    bool checkWashConfig();

    bool washEnable;
    int washInputRow, washInputCol;
    int washOutputRow, washOutputCol;
};

#endif // MAINWINDOW_H
