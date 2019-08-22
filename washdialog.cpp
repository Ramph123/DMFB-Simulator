#include "washdialog.h"
#include "ui_washdialog.h"
#include <QDebug>

washDialog::washDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::washDialog)
{
    ui->setupUi(this);

    connect(ui->confirmButton, SIGNAL(clicked()), this, SLOT(checkAndConfirm()));
    connect(ui->inputRow, SIGNAL(valueChanged(int)), this, SLOT(setInputRow(int)));
    connect(ui->inputCol, SIGNAL(valueChanged(int)), this, SLOT(setInputCol(int)));
    connect(ui->outputRow, SIGNAL(valueChanged(int)), this, SLOT(setOutputRow(int)));
    connect(ui->outputCol, SIGNAL(valueChanged(int)), this, SLOT(setOutputCol(int)));
    connect(ui->checkBox, SIGNAL(stateChanged(int)), this, SLOT(switchWash(int)));
}

washDialog::~washDialog() {
    delete ui;
}

void washDialog::checkAndConfirm() {
    //qDebug() << "!!!!!" << inputRow << inputCol << outputRow << outputCol;
    emit inputChanged(inputRow, inputCol);
    emit outputChanged(outputRow, outputCol);
    emit enableChanged(enable);
    emit validInput();
    accept();
}

void washDialog::setInputRow(int row) {
    inputRow = row;
}
void washDialog::setInputCol(int col) {
    inputCol = col;
}
void washDialog::setOutputRow(int row) {
    outputRow = row;
}
void washDialog::setOutputCol(int col) {
    outputCol = col;
}
void washDialog::switchWash(int state) {
    enable = state;
}
