#include "mainwindow.h"
#include "dialog.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("Digital Microfluidic Biochips Simulator"));
    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);

    // Chip
    bioChip = new chip(ui->widget);
    bioChip->show();
    connect(bioChip, SIGNAL(timeChanged(int)), this, SLOT(setTime(int)));

    // Menu & Toolbar
    initAction = new QAction(QIcon(":/icons/Icon/init.ico"), tr("New Simulation"), this);
    initAction->setShortcuts(QKeySequence::New);
    initAction->setStatusTip(tr("Start a simulation by entering parameters"));
    connect(initAction, &QAction::triggered, this, &MainWindow::init);
    stepForwardAction = new QAction(QIcon(":/icons/Icon/forward.ico"),tr("Step Forward"), this);
    stepForwardAction->setShortcuts(QKeySequence::MoveToPreviousChar);
    stepForwardAction->setStatusTip(tr("Move a step before the current status"));
    connect(stepForwardAction, &QAction::triggered, this, &MainWindow::stepForward);
    stepNextAction = new QAction(QIcon(":/icons/Icon/next.ico"),tr("Step Next"), this);
    stepNextAction->setShortcuts(QKeySequence::MoveToNextChar);
    stepNextAction->setStatusTip(tr("Move a step after the current status"));
    connect(stepNextAction, &QAction::triggered, this, &MainWindow::stepNext);
    playAllAction = new QAction(QIcon(":/icons/Icon/play.ico"),tr("Play All"), this);
    playAllAction->setShortcuts(QKeySequence::MoveToNextWord);
    playAllAction->setStatusTip(tr("Simulate the whole process"));
    connect(playAllAction, &QAction::triggered, this, &MainWindow::playAll);
    openAction = new QAction(QIcon(":/icons/Icon/file.ico"),tr("Open File"), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open a process file"));
    connect(openAction, &QAction::triggered, this, &MainWindow::open);
    resetAction = new QAction(QIcon(":/icons/Icon/reset.ico"),tr("Reset"), this);
    resetAction->setShortcuts(QKeySequence::MoveToPreviousWord);
    resetAction->setStatusTip(tr("Reset to the original state"));
    connect(resetAction, &QAction::triggered, this, &MainWindow::reset);

    QMenu *initMenu = menuBar()->addMenu(tr("File"));
    initMenu->addAction(initAction);
    initMenu->addAction(openAction);

    QMenu *operationMenu = menuBar()->addMenu(tr("Operation"));
    operationMenu->addAction(stepForwardAction);
    operationMenu->addAction(stepNextAction);
    operationMenu->addAction(playAllAction);
    operationMenu->addAction(resetAction);

    ui->toolBar->addAction(initAction);
    ui->toolBar->addAction(openAction);
    ui->toolBar->addAction(stepForwardAction);
    ui->toolBar->addAction(stepNextAction);
    ui->toolBar->addAction(playAllAction);
    ui->toolBar->addAction(resetAction);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Menu & Toolbar Implement

void MainWindow::init()
{
    bioChip->clearAllInput();
    initDialog = new Dialog(this);
    initDialog->setModal(false);
    initDialog->show();
    // Init
    connect(initDialog, SIGNAL(sizeChanged(int, int)), bioChip, SLOT(setSize(int, int)));
    connect(initDialog, SIGNAL(inputChanged(QString)), bioChip, SLOT(setInput(QString)));
    connect(initDialog, SIGNAL(outputChanged(int, int)), bioChip, SLOT(setOutput(int, int)));
    connect(initDialog, SIGNAL(validInput()), bioChip, SLOT(setReady()));
    connect(initDialog, SIGNAL(validInput()), this, SLOT(initTime()));
}

void MainWindow::initTime() {
    QFont font("Arial");
    font.setPixelSize(20);
    QString str = "CURRENT TIME    0";
    ui->timeDisplayer->setFont(font);
    ui->timeDisplayer->setText(str);
}

void MainWindow::setTime(int time) {
    QString str = "CURRENT TIME    " + QString::number(time);
    ui->timeDisplayer->setText(str);
}

void MainWindow::stepForward()
{
    if(!bioChip->ready)
        QMessageBox::critical(this, tr("Information"), tr("Please initialize first!"));
    else if(!fManager->fileLoaded())
        QMessageBox::critical(this, tr("Information"), tr("No file loaded!"));
    else
        bioChip->toPrev();
}

void MainWindow::stepNext()
{
    if(!bioChip->ready)
        QMessageBox::critical(this, tr("Information"), tr("Please initialize first!"));
    else if(!fManager->fileLoaded())
        QMessageBox::critical(this, tr("Information"), tr("No file loaded!"));
    else
        bioChip->toNext();
}

void MainWindow::playAll()
{
    if(!bioChip->ready)
        QMessageBox::critical(this, tr("Information"), tr("Please initialize first!"));
    else if(!fManager->fileLoaded())
        QMessageBox::critical(this, tr("Information"), tr("No file loaded!"));
    else
        bioChip->playAll();
}

void MainWindow::open()
{
    ui->textBrowser->clear();
    fManager = new fileManager;
    if(fManager->openFile()) {
        for(int i = 0; i < fManager->inputList.size(); i ++)
        {
            QString tmp = fManager->inputList.at(i);
//            if(i != fManager->inputList.size() - 1)
//                tmp.chop(1);
            ui->textBrowser->append(tmp);
        }
        fManager->parse();
        QMessageBox::information(this, tr("Information"), tr("File loaded!"));
    }
    //qDebug() << "qqqqqq" << fManager->getCommandList().size();
    bioChip->initCommandList(fManager->getCommandList());
}

void MainWindow::reset() {
    if(!bioChip->ready)
        QMessageBox::critical(this, tr("Information"), tr("Please initialize first!"));
    else if(!fManager->fileLoaded())
        QMessageBox::critical(this, tr("Information"), tr("No file loaded!"));
    else
        bioChip->reset();
}
