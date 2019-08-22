#ifndef WASHDIALOG_H
#define WASHDIALOG_H

#include <QDialog>

namespace Ui {
class washDialog;
}

class washDialog : public QDialog
{
    Q_OBJECT
public:
    explicit washDialog(QWidget *parent = nullptr);
    ~washDialog();

signals:
    void inputChanged(int, int);
    void outputChanged(int, int);
    void enableChanged(int);
    void validInput();

public slots:
    void checkAndConfirm();
    void setInputRow(int);
    void setInputCol(int);
    void setOutputRow(int);
    void setOutputCol(int);
    void switchWash(int);

private:
    Ui::washDialog *ui;
    bool enable = false;
    int inputRow = 1, inputCol = 1, outputRow = 1, outputCol = 1;
};

#endif // WASHDIALOG_H
