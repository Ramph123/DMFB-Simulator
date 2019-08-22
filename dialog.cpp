#include "dialog.h"
#include "ui_dialog.h"
#include <QGridLayout>
#include <iostream>
#include <QDebug>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    connect(ui->confrimButton, SIGNAL(clicked()), this, SLOT(checkAndConfirm()));
    connect(ui->rowNum, SIGNAL(valueChanged(int)), this, SLOT(setRowNum(int)));
    connect(ui->columnNum, SIGNAL(valueChanged(int)), this, SLOT(setColNum(int)));
    connect(ui->output_row, SIGNAL(valueChanged(int)), this, SLOT(setOutputRow(int)));
    connect(ui->output_column, SIGNAL(valueChanged(int)), this, SLOT(setOutputCol(int)));
    ui->listWidget->setViewMode(QListView::ListMode);
    connect(ui->AddButton,SIGNAL(clicked()),this,SLOT(addbtn()));
    connect(ui->DeleteButton,SIGNAL(clicked()),this,SLOT(deletebtn()));
}

Dialog::~Dialog()
{
//    std::cout << _rowNum << " " << _colNum << std::endl;
//    std::cout << _inputRow << " " << _inputCol << std::endl;
//    std::cout << _outputRow << " " << _outputCol << std::endl;
    delete ui;
}

void Dialog::addbtn()
{
    PointString input;
    input.row = ui->input_row->value();
    input.col = ui->input_column->value();
    QString str1 = QString::number(input.col);
    QString str2 = QString::number(input.row);
    QString str = str1 + " " + str2;
    QListWidgetItem *item = new QListWidgetItem;
    content.insert(str);
    item->setText(str);
    ui->listWidget->addItem(item);
}

void Dialog::deletebtn()
{
    if(ui->listWidget->currentRow() == -1) {
        QMessageBox::critical(this, tr("Error"), tr("No item selected!"));
    }
    else {
        QListWidgetItem *item = ui->listWidget->takeItem(ui->listWidget->currentRow());
        content.erase(item->text());
    }
}

int Dialog::checkInput() {
    std::set<QString>::iterator iter=content.begin();
    while(iter!=content.end())
    {
        QString str = *iter;
        QByteArray ba=str.toLatin1();
        char *c=ba.data();
        PointString item;
        sscanf(c, "%d %d", &item.row, &item.col);
        if(!(item.row == 1) && !(item.row == _rowNum) && !(item.col == 1) && !(item.col == _colNum))
            return 1;
        if(item.row == _outputRow && item.col == _outputCol)
            return 2;
        iter++;
    }
    return 0;
}

void Dialog::transferInput() {
    std::set<QString>::iterator iter=content.begin();
    while(iter!=content.end())
    {
        QString str = *iter;
        emit inputChanged(str);
        iter++;
    }
}


void Dialog::checkAndConfirm() {
    int res = checkInput();
    if(_rowNum <= 3 && _colNum <= 3) {
        QMessageBox::warning(this, tr("Error"), tr("Error: row number and column number cannot be both under 4!"));
    }
    else if(content.empty()) {
        QMessageBox::warning(this, tr("Error"), tr("Error: No input location!"));
    }
    else if(res == 1) {
        QMessageBox::warning(this, tr("Error"), tr("Error: invalid input position!"));
    }
    else if(res == 2) {
        QMessageBox::warning(this, tr("Error"), tr("Error: input and output in same position!"));
    }
    else if(!(_outputRow == 1) && !(_outputRow == _rowNum) && !(_outputCol == 1) && !(_outputCol == _colNum)) {
        QMessageBox::warning(this, tr("Error"), tr("Error: invalid output position!"));
    }
    else {
        emit validInput();
        emit sizeChanged(_rowNum, _colNum);
        transferInput();
        emit outputChanged(_outputRow, _outputCol);
        accept();
    }
}

void Dialog::setRowNum(int rowNum)
{
//    if(_rowNum == rowNum)
//        return;
    _rowNum = rowNum;
}

void Dialog::setColNum(int colNum)
{
//    if(_colNum == colNum)
//        return;
    _colNum = colNum;
}

void Dialog::setOutputRow(int outputRow)
{
//    if(_outputRow == outputRow)
//        return;
    _outputRow = outputRow;
}

void Dialog::setOutputCol(int outputCol)
{
//    if(_outputCol == outputCol)
//        return;
    _outputCol = outputCol;
}
