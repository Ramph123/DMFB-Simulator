#include "chip.h"
#include <iostream>
#include <QDebug>
#include <QMessageBox>
#include <QTime>
#include <cmath>
#include <vector>
#include <QCoreApplication>

using std::vector;

#define NEW_COLOR colorPalette[rand()%7]
#define NEW_ID waterDrop::idMax++

inline int max(int a, int b) {return (a>b)?a:b;}

QString chip::point2string(int col, int row) {
    QString str1 = QString::number(col);
    QString str2 = QString::number(row);
    QString str = str1 + "  " + str2;
    return str;
}

//waterDrop::waterDrop(int row, int col, int id, QColor color) :
//    row(row), col(col), id(id), color(color) {}
waterDrop::waterDrop(int row, int col, int id, QColor color, int dir, double size) :
    row(row), col(col), dir(dir), id(id), color(color), size(size) {}

multiset<waterDrop>::iterator chip::getDrop(int row, int col) {
    multiset<waterDrop>::iterator iter=water.begin();
    while(iter!=water.end())
    {
        waterDrop drop = *iter;
        if(drop.row == row && drop.col == col)
            return iter;
        iter ++;
    }
    return water.end();
}

chip::chip(QWidget *parent) : QWidget(parent) {
    setMinimumSize(700,560);
    setMaximumSize(1677215,1677215);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPalette pal(palette());
    pal.setColor(QPalette::Background, Qt::white);
    setAutoFillBackground(true);
    setPalette(pal);
    connect(timer, SIGNAL(timeout()), this, SLOT(toNext()));
    connect(washTimer, SIGNAL(timeout()), this, SLOT(repaint()));

    moveSound = new QSoundEffect(this);
    moveSound->setSource(QUrl::fromLocalFile(":/sound/SoundEffect/Move.wav"));
    moveSound->setVolume(1.0);
    splitSound = new QSoundEffect(this);
    splitSound->setSource(QUrl::fromLocalFile(":/sound/SoundEffect/Split.wav"));
    splitSound->setVolume(1.0);
    mergeSound = new QSoundEffect(this);
    mergeSound->setSource(QUrl::fromLocalFile(":/sound/SoundEffect/Merge.wav"));
    mergeSound->setVolume(1.0);
    stretchSound = new QSoundEffect(this);
    stretchSound->setSource(QUrl::fromLocalFile(":/sound/SoundEffect/Stretch.wav"));
    stretchSound->setVolume(1.0);

    for(int i = 0; i < 15; i ++) {
        for(int j = 0; j < 15; j ++) {
            stainColor[i][j] = Qt::white;
            stainCnt[i][j] = 0;
            stainId[i][j] = -1;
            isClicked[i][j] = false;
        }
    }
    washState = false;
}

int chip::calRowPos(int rowPos) {
    return startRow + rowPos * _length;
}
int chip::calColPos(int colPos) {
    return startCol + colPos * _length;
}
void chip::clearAllInput() {
    inputPos.clear();
}
void chip::changePrintFlag() {
    printPollution = !printPollution;
    update();
}

void chip::setWashEnable(bool enable) {
    //qDebug() << "?" << enable;
    washEnable = enable;
}
void chip::setWashInput(int row, int col) {
    //qDebug() << "??" << row << col;
    washInputRow = row;
    washInputCol = col;
}
void chip::setWashOutput(int row, int col) {
    //qDebug() << "???" << row << col;
    washOutputRow = row;
    washOutputCol = col;
}

void chip::drawChip() {
    _length = 400 / max(_rowNum, _colNum);
    QPainter p(this);
    QPen pen(Qt::black, 5);
    p.setPen(pen);
    for(int r = 0, curRow = startRow; r < _rowNum; r ++) {
        for(int c = 0, curCol = startCol; c < _colNum; c ++) {
            p.setBrush((isClicked[r+1][c+1]) ? Qt::gray : Qt::white);
            p.drawRect(curCol, curRow, _length, _length);
            curCol += _length;
        }
        curRow += _length;
    }
}

void chip::drawDrop(int row, int col, QColor color, int dir, double size) {
    //qDebug() << "draw drop" << row << col << dir;
    int radius = _length/3 * size;
    int targetRow = calRowPos(row-1);
    int targetCol = calColPos(col-1);
    targetRow = targetRow + _length/2 - radius;
    targetCol = targetCol + _length/2 - radius;
    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(color);
    if(!dir)
        p.drawEllipse(targetCol, targetRow, radius*2, radius*2);
    else if(dir == 2)
        p.drawEllipse(targetCol-_length, targetRow, radius*8, radius*2);
    else
        p.drawEllipse(targetCol, targetRow-_length, radius*2, radius*8);
}

void chip::drawWater() {
    multiset<waterDrop>::iterator iter=water.begin();
    while(iter!=water.end())
    {
        waterDrop drop = *iter;
        drawDrop(drop.row, drop.col, drop.color, drop.dir, drop.size);
        iter ++;
    }
}

void chip::drawInput(int inputRowPos, int inputColPos) {
    int targetRow = calRowPos(inputRowPos);
    int targetCol = calColPos(inputColPos);
    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::green);
    if(inputColPos == 0) {
        p.drawRect(targetCol-20-2, targetRow, 20, _length);
        p.setPen(Qt::green);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol-50, targetRow+20, tr("IN"));
    }
    else if(inputColPos == _colNum - 1) {
        p.drawRect(targetCol+_length+2, targetRow, 20, _length);
        p.setPen(Qt::green);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol+_length+30, targetRow+_length/2, tr("IN"));
    }
    else if(inputRowPos == 0) {
        p.drawRect(targetCol, targetRow-22, _length, 20);
        p.setPen(Qt::green);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol + _length/4, targetRow-25, tr("IN"));
    }
    else if(inputRowPos == _rowNum-1) {
        p.drawRect(targetCol, targetRow+_length+2, _length, 20);
        p.setPen(Qt::green);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol+_length/4, targetRow+_length+40, tr("IN"));
    }
}

void chip::drawOutput() {
    int targetRow = calRowPos(_outputRowPos - 1);
    int targetCol = calColPos(_outputColPos - 1);
    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::red);
    if(_outputColPos == 1) {
        p.drawRect(targetCol-20-2, targetRow, 20, _length);
        p.setPen(Qt::red);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol-70, targetRow+20, tr("OUT"));
    }
    else if(_outputColPos == _colNum) {
        p.drawRect(targetCol+_length+2, targetRow, 20, _length);
        p.setPen(Qt::red);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol+_length+30, targetRow+_length/2, tr("OUT"));
    }
    else if(_outputRowPos == 1) {
        p.drawRect(targetCol, targetRow-22, _length, 20);
        p.setPen(Qt::red);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol + _length/4, targetRow-25, tr("OUT"));
    }
    else if(_outputRowPos == _rowNum) {
        p.drawRect(targetCol, targetRow+_length+2, _length, 20);
        p.setPen(Qt::blue);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol+_length/4, targetRow+_length+40, tr("OUT"));
    }
}

void chip::drawStain() {
    for(int i = 1; i <= _rowNum; i ++) {
        for(int j = 1; j <= _colNum; j ++) {
            int radius = _length/10;
            int targetRow = calRowPos(i-1);
            int targetCol = calColPos(j-1);
            targetRow = targetRow + _length/2 - radius;
            targetCol = targetCol + _length/2 - radius;
            QPainter p(this);
            p.setPen(Qt::NoPen);
            QColor color = stainColor[i][j];
            if(color == Qt::white && isClicked[i][j])
                color = Qt::gray;
            p.setBrush(color);
            p.drawEllipse(targetCol, targetRow, radius*2, radius*2);
        }
    }
}

void chip::drawPollution() {
    //qDebug() << _rowNum << _colNum;
    for(int i = 1; i <= _rowNum; i ++) {
        for(int j = 1; j <= _colNum; j ++) {
            int targetRow = calRowPos(i - 1) + _length/2 + 10;
            int targetCol = calColPos(j - 1) + _length/2 - 10;
            QPainter p(this);
            QFont font;
            font.setPixelSize(20);
            p.setFont(font);
            p.setPen(Qt::black);
            p.drawText(targetCol, targetRow, QString::number(stainCnt[i][j]));
        }
    }
}

void chip::drawWasher() {
    //qDebug() << washInputRow << washInputCol;
    //qDebug() << washOutputRow << washOutputCol;
    if(!washEnable)
        return;
    int targetRow = calRowPos(washInputRow - 1);
    int targetCol = calColPos(washInputCol - 1);
    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::blue);
    if(washInputCol == 1) {
        p.drawRect(targetCol-20-2, targetRow, 20, _length);
        p.setPen(Qt::blue);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol-110, targetRow+20, tr("WASHER"));
    }
    else if(washInputCol == _colNum) {
        p.drawRect(targetCol+_length+2, targetRow, 20, _length);
        p.setPen(Qt::blue);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol+_length+30, targetRow+_length/2, tr("WASHER"));
    }
    else if(washInputRow == 1) {
        p.drawRect(targetCol, targetRow-22, _length, 20);
        p.setPen(Qt::blue);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol + _length/4, targetRow-25, tr("WASHER"));
    }
    else if(washInputCol == _rowNum) {
        p.drawRect(targetCol, targetRow+_length+2, _length, 20);
        p.setPen(Qt::blue);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol+_length/4, targetRow+_length+40, tr("WASHER"));
    }
    targetRow = calRowPos(washOutputRow - 1);
    targetCol = calColPos(washOutputCol - 1);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::darkYellow);
    if(washOutputCol == 1) {
        p.drawRect(targetCol-20-2, targetRow, 20, _length);
        p.setPen(Qt::darkYellow);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol-100, targetRow+20, tr("WASTE"));
    }
    else if(washOutputCol == _colNum) {
        p.drawRect(targetCol+_length+2, targetRow, 20, _length);
        p.setPen(Qt::darkYellow);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol+_length+30, targetRow+_length/2, tr("WASTE"));
    }
    else if(washOutputRow == 1) {
        p.drawRect(targetCol, targetRow-22, _length, 20);
        p.setPen(Qt::darkYellow);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol + _length/4, targetRow-25, tr("WASTE"));
    }
    else if(washOutputCol == _rowNum) {
        p.drawRect(targetCol, targetRow+_length+2, _length, 20);
        p.setPen(Qt::darkYellow);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol+_length/4, targetRow+_length+40, tr("WASTE"));
    }
}

void chip::drawWasherDrop() {
    //qDebug() << "draw" << washerRow << washerCol;
    int radius = _length/3;
    int targetRow = calRowPos(washerRow-1);
    int targetCol = calColPos(washerCol-1);
    targetRow = targetRow + _length/2 - radius;
    targetCol = targetCol + _length/2 - radius;
    QPainter p1(this);
    p1.setPen(Qt::NoPen);
    p1.setBrush(Qt::blue);
    p1.drawEllipse(targetCol, targetRow, radius*2, radius*2);
}

void chip::paintEvent(QPaintEvent *) {
    if(ready) {
        drawChip();
        multiset<QString>::iterator iter=inputPos.begin();
        while(iter!=inputPos.end())
        {
            QString str = *iter;
            QByteArray ba=str.toLatin1();
            char *c=ba.data();
            int inputRow, inputCol;
            sscanf(c, "%d  %d", &inputCol, &inputRow);
//            qDebug() << inputRow;
//            qDebug() << inputCol;
            drawInput(inputRow-1, inputCol-1);
            iter ++;
        }
        drawOutput();
        drawWasher();
        if(printPollution) {
            drawPollution();
        }
        else {
            drawStain();
            if(washState)
                drawWasherDrop();
            drawWater();
        }
    }
}

void chip::operateReverse(command op) {
    if(op.type == "Input") {
        qDebug() << "Rev" << "Input";
        multiset<waterDrop>::iterator target = getDrop(op.tarRow, op.tarCol);
        water.erase(target);
    }
    else if(op.type == "Output") {
        qDebug() << "Rev" << "Output";
        //command revCommand(op.time, "Input", op.tarRow, op.tarCol);
        waterDrop newDrop(op.tarRow, op.tarCol, NEW_ID, usedColor.top(), 0, usedSize.top());
        usedColor.pop();
        usedSize.pop();
        water.insert(newDrop);
    }
    else if(op.type == "Move") {
        qDebug() << "Rev" << "Move";
        int row = op.tarRow, col = op.tarCol;
        int newRow = op.tarRow, newCol = op.tarCol;
        switch (op.dir) {
            case 1:
                newRow --;
                break;
            case 2:
                newRow ++;
                break;
            case 3:
                newCol --;
                break;
            case 4:
                newCol ++;
                break;
        }
        multiset<waterDrop>::iterator target = getDrop(newRow, newCol);
        if(target == water.end()) {
            QMessageBox::critical(this, tr("Error"), tr("Error: water drop not exist!"));
            return;
        }
        waterDrop newDrop(row, col, target->id, target->color, target->dir, target->size);
        water.erase(target);
        water.insert(newDrop);
    }
    else if(op.type == "Split1") {
        qDebug() << "Rev" << "Split1";
        command revCommand(op.time, "Merge2", op.tarRow, op.tarCol);
        operate(revCommand, true);
    }
    else if(op.type == "Split2") {
        qDebug() << "Rev" << "Split2";
        int row = op.tarRow, col = op.tarCol;
        int newRow = op.tarRow, newCol = op.tarCol;
        switch (op.dir) {
            case 1:
                row -= 1;
                newRow += 1;
                break;
            case 2:
                col -= 1;
                newCol += 1;
                break;
        }
        multiset<waterDrop>::iterator target1 = getDrop(row, col);
        multiset<waterDrop>::iterator target2 = getDrop(newRow, newCol);
        if(target1 == water.end() || target2 == water.end()) {
            QMessageBox::critical(this, tr("Error"), tr("Error: water drop not exist!"));
            return;
        }
        waterDrop newDrop((row+newRow)/2, (col+newCol)/2, NEW_ID, usedColor.top(), op.dir, 1);
        usedColor.pop();
        //usedColor.push(target1->color);
        //usedColor.push(target2->color);
        water.erase(target1);
        water.erase(target2);
        water.insert(newDrop);
    }
    else if(op.type == "Merge1") {
        qDebug() << "Rev" << "Merge1";
        multiset<waterDrop>::iterator target = getDrop(op.tarRow, op.tarCol);
        if(target == water.end()) {
            QMessageBox::critical(this, tr("Error"), tr("Error: water drop not exist!"));
            return;
        }
        if(target->dir == 1) {
            waterDrop newDrop1(target->row+1, target->col, NEW_ID, usedColor.top());
            usedColor.pop();
            waterDrop newDrop2(target->row-1, target->col, NEW_ID, usedColor.top());
            usedColor.pop();
            water.insert(newDrop1);
            water.insert(newDrop2);
        }
        else if(target->dir == 2) {
            waterDrop newDrop1(target->row, target->col+1, NEW_ID, usedColor.top());
            usedColor.pop();
            waterDrop newDrop2(target->row, target->col-1, NEW_ID, usedColor.top());
            usedColor.pop();
            water.insert(newDrop1);
            water.insert(newDrop2);
        }
        water.erase(target);
    }
    else if(op.type == "Merge2") {
        qDebug() << "Rev" << "Merge2";
        command revCommand(op.time, "Split1", op.tarRow, op.tarCol, op.dir);
        operate(revCommand, true);
    }
}

void chip::operate(command op, bool mute) {
    qDebug() << op.type << op.time << op.tarRow << op.tarCol << op.dir;

    if(op.type == "Input") {
        if(!mute && inputPos.find(point2string(op.tarCol, op.tarRow)) == inputPos.end()) {
            QMessageBox::critical(this, tr("Error"), tr("Error: not an input port!"));
        }
        else {
            if(washEnable && stainColor[op.tarRow][op.tarCol] != Qt::white) {
                washStain(op.tarRow, op.tarCol);
            }
            waterDrop newDrop(op.tarRow, op.tarCol, NEW_ID, NEW_COLOR);
            stainCommand newLog;
            newLog.time = op.time;
            newLog.row = op.tarRow;
            newLog.col = op.tarCol;
            newLog.prevColor = stainColor[op.tarRow][op.tarCol];
            newLog.prevId = stainId[op.tarRow][op.tarCol];
            stainLog.push(newLog);
            if(stainColor[op.tarRow][op.tarCol] != newDrop.color)
                stainCnt[op.tarRow][op.tarCol] ++;
            stainColor[op.tarRow][op.tarCol] = newDrop.color;
            stainId[op.tarRow][op.tarCol] = newDrop.getId();
            water.insert(newDrop);
        }
    }
    else if(op.type == "Move") {
        multiset<waterDrop>::iterator target = getDrop(op.tarRow, op.tarCol);
        if(target == water.end()) {
            QMessageBox::critical(this, tr("Error"), tr("Error: water drop not exist!"));
            return;
        }
        int newRow = op.tarRow, newCol = op.tarCol;
        switch (op.dir) {
            case 1:
                newRow --;
                break;
            case 2:
                newRow ++;
                break;
            case 3:
                newCol --;
                break;
            case 4:
                newCol ++;
                break;
        }
        if(washEnable && stainColor[newRow][newCol] != Qt::white && stainId[newRow][newCol] != target->id) {
            washStain(newRow, newCol);
        }
        if(washEnable && (newRow-1 != op.tarRow || newCol != op.tarCol) && stainColor[newRow-1][newCol] != Qt::white && stainId[newRow-1][newCol] != target->id) {
            washStain(newRow-1, newCol, false);
        }
        if(washEnable && (newRow+1 != op.tarRow || newCol != op.tarCol) && stainColor[newRow+1][newCol] != Qt::white && stainId[newRow+1][newCol] != target->id) {
            washStain(newRow+1, newCol, false);
        }
        if(washEnable && (newRow != op.tarRow || newCol-1 != op.tarCol) && stainColor[newRow][newCol-1] != Qt::white && stainId[newRow][newCol-1] != target->id) {
            washStain(newRow, newCol-1, false);
        }
        if(washEnable && (newRow != op.tarRow || newCol+1 != op.tarCol) && stainColor[newRow][newCol+1] != Qt::white && stainId[newRow][newCol+1] != target->id) {
            washStain(newRow, newCol+1, false);
        }
        if(!mute)
            moveSound->play();
        waterDrop newDrop(newRow, newCol, target->id, target->color, target->dir, target->size);
        stainCommand newLog;
        newLog.time = op.time;
        newLog.row = newRow;
        newLog.col = newCol;
        newLog.prevColor = stainColor[newRow][newCol];
        newLog.prevId = stainId[newRow][newCol];
        stainLog.push(newLog);
        if(stainColor[newRow][newCol] != newDrop.color)
            stainCnt[newRow][newCol] ++;
        stainColor[newRow][newCol] = newDrop.color;
        stainId[newRow][newCol] = newDrop.getId();
        water.erase(target);
        water.insert(newDrop);
    }
    else if(op.type == "Output") {
        qDebug() << op.tarRow << _outputRowPos << op.tarCol << _outputColPos;
        if(!mute && (op.tarRow != _outputRowPos || op.tarCol != _outputColPos)) {
            QMessageBox::critical(this, tr("Error"), tr("Error: not an output port!"));
        }
        else {
            multiset<waterDrop>::iterator target = getDrop(op.tarRow, op.tarCol);
            if(target == water.end()) {
                QMessageBox::critical(this, tr("Error"), tr("Error: water drop not exist!"));
                return;
            }
            usedColor.push(target->color);
            usedSize.push(target->size);
            water.erase(target);
        }
    }
    else if(op.type == "Split1") {
        multiset<waterDrop>::iterator target = getDrop(op.tarRow, op.tarCol);
        if(target == water.end()) {
            QMessageBox::critical(this, tr("Error"), tr("Error: water drop not exist!"));
            return;
        }
        if(op.dir == 1) {
            if(washEnable && stainColor[op.tarRow-1][op.tarCol] != Qt::white && stainId[op.tarRow-1][op.tarCol] != target->id) {
                washStain(op.tarRow-1, op.tarCol);
            }
            if(washEnable && stainColor[op.tarRow+1][op.tarCol] != Qt::white && stainId[op.tarRow+1][op.tarCol] != target->id) {
                washStain(op.tarRow+1, op.tarCol);
            }
        }
        else if(op.dir == 2) {
            if(washEnable && stainColor[op.tarRow][op.tarCol-1] != Qt::white && stainId[op.tarRow][op.tarCol-1] != target->id) {
                washStain(op.tarRow, op.tarCol-1);
            }
            if(washEnable && stainColor[op.tarRow][op.tarCol+1] != Qt::white && stainId[op.tarRow][op.tarCol+1] != target->id) {
                washStain(op.tarRow, op.tarCol+1);
            }
        }
        if(!mute)
            stretchSound->play();
        waterDrop newDrop(target->row, target->col, target->id, target->color, op.dir);
        water.erase(target);
        water.insert(newDrop);
    }
    else if(op.type == "Split2") {
        multiset<waterDrop>::iterator target = getDrop(op.tarRow, op.tarCol);
        if(target == water.end()) {
            QMessageBox::critical(this, tr("Error"), tr("Error: water drop not exist!"));
            return;
        }
        if(target->dir == 1) {
            waterDrop newDrop1(target->row-1, target->col, NEW_ID, NEW_COLOR);
            waterDrop newDrop2(target->row+1, target->col, NEW_ID, NEW_COLOR);
            stainCommand newLog;
            newLog.time = op.time;
            newLog.row = newDrop1.row;
            newLog.col = newDrop1.col;
            newLog.prevColor = stainColor[newDrop1.row][newDrop1.col];
            newLog.prevId = stainId[newDrop1.row][newDrop1.col];
            //qDebug() << newLog.row << newLog.col;
            stainLog.push(newLog);
            if(stainColor[newDrop1.row][newDrop1.col] != newDrop1.color)
                stainCnt[newDrop1.row][newDrop1.col] ++;
            stainColor[newDrop1.row][newDrop1.col] = newDrop1.color;
            stainId[newDrop1.row][newDrop1.col] = newDrop1.getId();
            stainCommand newLog2;
            newLog2.time = op.time;
            newLog2.row = newDrop2.row;
            newLog2.col = newDrop2.col;
            newLog2.prevColor = stainColor[newDrop2.row][newDrop2.col];
            newLog2.prevId = stainId[newDrop2.row][newDrop2.col];
            //qDebug() << newLog2.row << newLog2.col;
            stainLog.push(newLog2);
            if(stainColor[newDrop2.row][newDrop2.col] != newDrop2.color)
                stainCnt[newDrop2.row][newDrop2.col] ++;
            stainColor[newDrop2.row][newDrop2.col] = newDrop2.color;
            stainId[newDrop2.row][newDrop2.col] = newDrop2.getId();
            water.insert(newDrop1);
            water.insert(newDrop2);
        }
        else if(target->dir == 2) {
            waterDrop newDrop1(target->row, target->col-1, NEW_ID, NEW_COLOR);
            waterDrop newDrop2(target->row, target->col+1, NEW_ID, NEW_COLOR);
            stainCommand newLog;
            newLog.time = op.time;
            newLog.row = newDrop1.row;
            newLog.col = newDrop1.col;
            newLog.prevColor = stainColor[newDrop1.row][newDrop1.col];
            newLog.prevId = stainId[newDrop1.row][newDrop1.col];
            stainLog.push(newLog);
            if(stainColor[newDrop1.row][newDrop1.col] != newDrop1.color)
                stainCnt[newDrop1.row][newDrop1.col] ++;
            stainId[newDrop1.row][newDrop1.col] = newDrop1.getId();
            stainColor[newDrop1.row][newDrop1.col] = newDrop1.color;
            stainCommand newLog2;
            newLog2.time = op.time;
            newLog2.row = newDrop2.row;
            newLog2.col = newDrop2.col;
            newLog2.prevColor = stainColor[newDrop2.row][newDrop2.col];
            newLog2.prevId = stainId[newDrop2.row][newDrop2.col];
            stainLog.push(newLog2);
            if(stainColor[newDrop2.row][newDrop2.col] != newDrop2.color)
                stainCnt[newDrop2.row][newDrop2.col] ++;
            stainColor[newDrop2.row][newDrop2.col] = newDrop2.color;
            stainId[newDrop2.row][newDrop2.col] = newDrop2.getId();
            water.insert(newDrop1);
            water.insert(newDrop2);
        }
        if(!mute)
            splitSound->play();
        usedColor.push(target->color);
        water.erase(target);
    }
    else if(op.type == "Merge1") {
        int row = op.tarRow, col = op.tarCol;
        int newRow = op.tarRow, newCol = op.tarCol;
        switch (op.dir) {
            case 1:
                row -= 1;
                newRow += 1;
                break;
            case 2:
                col -= 1;
                newCol += 1;
                break;
        }
        multiset<waterDrop>::iterator target1 = getDrop(row, col);
        multiset<waterDrop>::iterator target2 = getDrop(newRow, newCol);
        if(target1 == water.end() || target2 == water.end()) {
            QMessageBox::critical(this, tr("Error"), tr("Error: water drop not exist!"));
            return;
        }
        if(washEnable && stainColor[(row+newRow)/2][(col+newCol)/2] != Qt::white) {
            washStain((row+newRow)/2, (col+newCol)/2);
        }
        if(!mute)
            mergeSound->play();
        waterDrop newDrop((row+newRow)/2, (col+newCol)/2, NEW_ID, NEW_COLOR, op.dir);
        usedColor.push(target1->color);
        usedColor.push(target2->color);
        water.erase(target1);
        water.erase(target2);
        water.insert(newDrop);
    }
    else if(op.type == "Merge2") {
        multiset<waterDrop>::iterator target = getDrop(op.tarRow, op.tarCol);
        if(target == water.end()) {
            QMessageBox::critical(this, tr("Error"), tr("Error: water drop not exist!"));
            return;
        }
        if(!mute)
            stretchSound->play();
        waterDrop newDrop(target->row, target->col, target->id, target->color, 0, 1.414);
        stainCommand newLog;
        newLog.time = op.time;
        newLog.row = newDrop.row;
        newLog.col = newDrop.col;
        newLog.prevColor = stainColor[newDrop.row][newDrop.col];
        newLog.prevId = stainId[newDrop.row][newDrop.col];
        stainLog.push(newLog);
        if(stainColor[newDrop.row][newDrop.col] != newDrop.color)
            stainCnt[newDrop.row][newDrop.col] ++;
        stainId[newDrop.row][newDrop.col] = newDrop.getId();
        stainColor[newDrop.row][newDrop.col] = newDrop.color;
        water.erase(target);
        water.insert(newDrop);
    }
}

void chip::initCommandList(multiset<command>& commandList) {
    commands = commandList;
    //qDebug() << "num" << commandList.size();
    curCommand = commands.begin();
}

multiset<command>::iterator chip::getPrev() {
    if(curCommand == commands.begin())
        return curCommand;
    else {
        multiset<command>::iterator ret = -- curCommand;
        curCommand ++;
        return ret;
    }
}

void chip::toPrev() {
    if(washStopSignal || washState)
        return;
    //qDebug() << "!!!" << water.size();
    if(currentTime == 0) {
        QMessageBox::critical(this, tr("Information"), tr("Unable to simulate forward"));
        return;
    }
    currentTime --;
    emit(timeChanged(currentTime));
    while(!stainLog.empty() && stainLog.top().time >= currentTime) {
        stainCommand c = stainLog.top();
        qDebug() << "stain" << c.row << c.col << c.prevColor;
        stainLog.pop();
        stainId[c.row][c.col] = c.prevId;
        stainColor[c.row][c.col] = c.prevColor;
        stainCnt[c.row][c.col] --;
    }
    //qDebug() << getPrev()->time << currentTime;
    while(getPrev()->time >= currentTime) {
        if(curCommand == commands.begin())
            break;
        //qDebug() << "---";
        operateReverse(*getPrev());
        curCommand --;
    }
    conatraint = checkConstraint();
    update();
}

void chip::toNext() {
    if(washStopSignal || washState)
        return;
    //qDebug() << "!!!" << water.size();
    if(!conatraint && washStopSignal)
        return;
    while(curCommand != commands.end() && curCommand->time <= currentTime) {
        operate(*curCommand);
        curCommand ++;
    }
    conatraint = checkConstraint();
    if(!conatraint) {
        //operateReverse(*(-- curCommand));
        //curCommand ++;
        while(getPrev()->time >= currentTime) {
            if(curCommand == commands.begin())
                break;
            //qDebug() << "---";
            while(!stainLog.empty() && stainLog.top().time >= currentTime) {
                stainCommand c = stainLog.top();
                //qDebug() << "stain" << c.row << c.col << c.prevColor;
                stainLog.pop();
                stainColor[c.row][c.col] = c.prevColor;
                stainCnt[c.row][c.col] --;
            }
            operateReverse(*getPrev());
            curCommand --;
        }
        QMessageBox::critical(this, tr("Error"), tr("Constraint violation!"));
        update();
    }
    else {
        currentTime ++;
        update();
        emit timeChanged(currentTime);
    }
    if(timer->isActive() && (curCommand == commands.end() || !conatraint))
        timer->stop();
}

void chip::playAll() {
    if(washState)
        return;
    timer->start(500);
}

void chip::reset() {
    if(washState)
        return;
    timer->stop();
    currentTime = 0;
    washStopSignal = false;
    emit timeChanged(currentTime);
//    washEnable = false;
//    washInputRow = 0; washInputCol = 0;
//    washOutputRow = 0; washOutputCol = 0;
    curCommand = commands.begin();
    for(int i = 0; i < 15; i ++) {
        for(int j = 0; j < 15; j ++) {
            stainColor[i][j] = Qt::white;
            stainCnt[i][j] = 0;
        }
    }
    while(!stainLog.empty())
        stainLog.pop();
    water.clear();
    conatraint = true;
    while(!usedColor.empty())
        usedColor.pop();
    update();
}

bool chip::checkConstraint() {
    //qDebug() << "Size" << water.size();
    if(water.size() < 2)
        return true;
    vector<int> rows;
    vector<int> cols;
    multiset<waterDrop>::iterator drop1;
    for(drop1 = water.begin(); drop1 != water.end(); drop1 ++) {
        if(drop1->dir == 0) {
            rows.push_back(drop1->row);
            cols.push_back(drop1->col);
        }
        else if(drop1->dir == 1) {
            rows.push_back(drop1->row-1);
            cols.push_back(drop1->col);
            rows.push_back(drop1->row+1);
            cols.push_back(drop1->col);
        }
        else if(drop1->dir == 2) {
            rows.push_back(drop1->row);
            cols.push_back(drop1->col-1);
            rows.push_back(drop1->row);
            cols.push_back(drop1->col+1);
        }
        for(int i = 0; i < rows.size(); i ++) {
            for(int j = i+1; j < rows.size(); j ++) {
                if(abs(rows[i]-rows[j]) <= 1 && abs(cols[i]-cols[j]) <= 1) {
                    //qDebug() << i << rows[i] << cols[i] << j << rows[j] << cols[j];
                    return false;
                }
            }
        }
    }
    return true;
}

void chip::mousePressEvent ( QMouseEvent * e ) {
    //QString str="("+QString::number(e->x())+","+QString::number(e->y())+")";
    if(ready && e->button()==Qt::RightButton)
    {
        int col = (e->x() - startCol)/_length + 1;
        int row = (e->y() - startRow)/_length + 1;
        if(1 <= row && row <= _rowNum && 1 <= col && col <= _colNum) {
            isClicked[row][col] = !isClicked[row][col];
            update();
        }
    }
}

struct BFSunit {
    int row, col;
    string dir;
    bool flag;
};

string chip::findRoute1(int row, int col) {
    queue<BFSunit> step;
    bool visited[15][15];
    for(int i = 1; i <= _rowNum; i ++) {
        for(int j = 1; j <= _colNum; j ++) {
            visited[i][j] = isClicked[i][j];
        }
    }
    for(multiset<waterDrop>::iterator it = water.begin(); it != water.end(); it ++) {
        qDebug() << it->row << it->col;
        visited[it->row-1][it->col-1] = 1;
        visited[it->row-1][it->col] = 1;
        visited[it->row-1][it->col+1] = 1;
        visited[it->row][it->col-1] = 1;
        visited[it->row][it->col] = 1;
        visited[it->row][it->col+1] = 1;
        visited[it->row+1][it->col-1] = 1;
        visited[it->row+1][it->col] = 1;
        visited[it->row+1][it->col+1] = 1;
        if(it->dir == 1) {
            qDebug() << "?";
            visited[it->row-2][it->col-1] = 1;
            visited[it->row-2][it->col] = 1;
            visited[it->row-2][it->col+1] = 1;
            visited[it->row+2][it->col-1] = 1;
            visited[it->row+2][it->col] = 1;
            visited[it->row+2][it->col+1] = 1;
        }
        else if(it->dir == 2) {
            qDebug() << "??";
            visited[it->row-1][it->col-2] = 1;
            visited[it->row][it->col-2] = 1;
            visited[it->row+1][it->col-2] = 1;
            visited[it->row-1][it->col+2] = 1;
            visited[it->row][it->col+2] = 1;
            visited[it->row+1][it->col+2] = 1;
        }
    }
    std::cout << "clicked" << std::endl;
    for(int i = 1; i <= _rowNum; i ++) {
        for(int j = 1; j <= _colNum; j ++) {
            std::cout << isClicked[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "visited" << std::endl;
    for(int i = 1; i <= _rowNum; i ++) {
        for(int j = 1; j <= _colNum; j ++) {
            std::cout << visited[i][j] << " ";
        }
        std::cout << std::endl;
    }
    BFSunit init;
    init.row = washInputRow;
    init.col = washInputCol;
    init.dir = "";
    init.flag = false;
    step.push(init);
    visited[washInputRow][washInputCol] = true;
    while(!step.empty()) {
        BFSunit cur = step.front();
        step.pop();
        visited[cur.row][cur.col] = true;
        if(cur.row == row && cur.col == col) {
            return cur.dir;
        }
        if(!visited[cur.row-1][cur.col] && cur.row > 1) {
            BFSunit next;
            next.row = cur.row-1;
            next.col = cur.col;
            next.dir = cur.dir + "w";
            next.flag = cur.flag;
            step.push(next);
        }
        if(!visited[cur.row+1][cur.col] && cur.row < _rowNum) {
            BFSunit next;
            next.row = cur.row+1;
            next.col = cur.col;
            next.dir = cur.dir + "s";
            next.flag = cur.flag;
            step.push(next);
        }
        if(!visited[cur.row][cur.col-1] && cur.col > 1) {
            BFSunit next;
            next.row = cur.row;
            next.col = cur.col-1;
            next.dir = cur.dir + "a";
            next.flag = cur.flag;
            step.push(next);
        }
        if(!visited[cur.row][cur.col+1] && cur.col < _colNum) {
            BFSunit next;
            next.row = cur.row;
            next.col = cur.col+1;
            next.dir = cur.dir + "d";
            next.flag = cur.flag;
            step.push(next);
        }
    }
    return "";
}
string chip::findRoute2(int row, int col) {
    queue<BFSunit> step;
    bool visited[15][15];
    for(int i = 1; i <= _rowNum; i ++) {
        for(int j = 1; j <= _colNum; j ++) {
            visited[i][j] = 0;
        }
    }
    for(int i = 1; i <= _rowNum; i ++) {
        for(int j = 1; j <= _colNum; j ++) {
            visited[i][j] = isClicked[i][j];
        }
    }
    for(multiset<waterDrop>::iterator it = water.begin(); it != water.end(); it ++) {
        visited[it->row][it->col] = 1;
        if(it->dir == 1) {
            visited[it->row-1][it->col] = 1;
            visited[it->row+1][it->col] = 1;
        }
        else if(it->dir == 2) {
            visited[it->row][it->col-1] = 1;
            visited[it->row][it->col+1] = 1;
        }
    }
    BFSunit init;
    init.row = washOutputRow;
    init.col = washOutputCol;
    init.dir = "";
    step.push(init);
    visited[washInputRow][washInputCol] = true;
    while(!step.empty()) {
        BFSunit cur = step.front();
        step.pop();
        visited[cur.row][cur.col] = true;
        if(cur.row == row && cur.col == col) {
            return cur.dir;
        }
        if(!visited[cur.row-1][cur.col] && cur.row > 1) {
            BFSunit next;
            next.row = cur.row-1;
            next.col = cur.col;
            next.dir = "s" + cur.dir;
            next.flag = cur.flag;
            step.push(next);
        }
        if(!visited[cur.row+1][cur.col] && cur.row < _rowNum) {
            BFSunit next;
            next.row = cur.row+1;
            next.col = cur.col;
            next.dir = "w" + cur.dir;
            next.flag = cur.flag;
            step.push(next);
        }
        if(!visited[cur.row][cur.col-1] && cur.col > 1) {
            BFSunit next;
            next.row = cur.row;
            next.col = cur.col-1;
            next.dir = "d" + cur.dir;
            next.flag = cur.flag;
            step.push(next);
        }
        if(!visited[cur.row][cur.col+1] && cur.col < _colNum) {
            BFSunit next;
            next.row = cur.row;
            next.col = cur.col+1;
            next.dir = "a" + cur.dir;
            next.flag = cur.flag;
            step.push(next);
        }
    }
    return "";
}

void chip::washStain(int row, int col, bool flag) {
    if(washStopSignal)
        return;
    //qDebug() << "entering...";
    string directions;
    string find1 = findRoute1(row, col);
    string find2 = findRoute2(row, col);
    if(find1 == "" || find2 == "") {
        if(flag) {
            QMessageBox::critical(this, tr("Information"), tr("No washing solution!"));
            washStopSignal = true;
        }
        return;
    }
    directions = find1+find2;
    qDebug() << "directions" << QString::fromStdString(find1) << QString::fromStdString(find2);
    bool flag1 = timer->isActive();
    if(flag1)    timer->stop();
    washState = true;
    //qDebug() << "in function" << washState;
    washerRow = washInputRow;
    washerCol = washInputCol;
    //qDebug() << washerRow << washerCol;
    repaint();
    for(int cur = 0; cur < directions.size(); cur ++) {
        //qDebug() << "iiiii";
        if(directions[cur] == 'w')
            washerRow --;
        else if(directions[cur] == 's')
            washerRow ++;
        else if(directions[cur] == 'a')
            washerCol --;
        else if(directions[cur] == 'd')
            washerCol ++;
        //qDebug() << washerRow << washerCol;
        QEventLoop eventloop;
        QTimer::singleShot(150, &eventloop, SLOT(quit()));
        eventloop.exec();
        repaint();
//        if(stainColor[washerRow][washerCol] != Qt::white) {
//            stainCnt[washerRow][washerCol] --;
//        }
        stainCnt[washerRow][washerCol] = 0;
        stainId[washerRow][washerCol] = -1;
        stainColor[washerRow][washerCol] = Qt::white;
    }
    washState = false;
    if(flag1)    timer->start(500);
}

//signals

void chip::setReady() {
    ready = true;
    reset();
}
void chip::setSize(int row, int col) {
    if(_rowNum != row)
        _rowNum = row;
    if(_colNum != col)
        _colNum = col;
}
void chip::setInput(QString input) {
    inputPos.insert(input);
}
void chip::setOutput(int row, int col) {
    if(_outputRowPos != row)
        _outputRowPos = row;
    if(_outputColPos != col)
        _outputColPos = col;
}
