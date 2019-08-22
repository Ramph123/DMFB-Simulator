#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QMessageBox>
#include <QListWidgetItem>
#include <set>

using std::multiset;

namespace Ui {
class Dialog;
}

struct PointString {
    int row, col;
};

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();
    int getRowNum() {return _rowNum;}
    int getColNum() {return _colNum;}
    int getOutputRow() {return _outputRow;}
    int getOutputCol() {return _outputCol;}

public slots:
    void setRowNum(int);
    void setColNum(int);
    void setOutputRow(int);
    void setOutputCol(int);
    void checkAndConfirm();
    void addbtn();
    void deletebtn();

signals:
    void sizeChanged(int, int);
    void inputChanged(QString);
    void outputChanged(int, int);
    void validInput();


private:
    Ui::Dialog *ui;
    multiset<QString> content;

    int checkInput();
    void transferInput();

    int _rowNum = 5, _colNum = 5, _outputRow = 1, _outputCol = 1;
};

#endif // DIALOG_H
