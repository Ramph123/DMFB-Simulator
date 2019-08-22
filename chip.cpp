#include "chip.h"
#include <iostream>
#include <QDebug>
#include <QMessageBox>

#define NEW_COLOR colorPalette[rand()%7]
#define NEW_ID waterDrop::idMax++

inline int max(int a, int b) {return (a>b)?a:b;}

QString chip::point2string(int col, int row) {
    QString str1 = QString::number(col);
    QString str2 = QString::number(row);
    QString str = str1 + " " + str2;
    return str;
}

//waterDrop::waterDrop(int row, int col, int id, QColor color) :
//    row(row), col(col), id(id), color(color) {}
waterDrop::waterDrop(int row, int col, int id, QColor color, int dir) :
    row(row), col(col), dir(dir), id(id), color(color) {}

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

void chip::drawChip() {
    _length = 400 / max(_rowNum, _colNum);
    QPainter p(this);
    QPen pen(Qt::black, 5);
    p.setPen(pen);
    for(int r = 0, curRow = startRow; r < _rowNum; r ++) {
        for(int c = 0, curCol = startCol; c < _colNum; c ++) {
            p.drawRect(curCol, curRow, _length, _length);
            curCol += _length;
        }
        curRow += _length;
    }
}

void chip::drawDrop(int row, int col, QColor color, int dir) {
    //qDebug() << "draw drop" << row << col << dir;
    int radius = _length/3;
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
        drawDrop(drop.row, drop.col, drop.color, drop.dir);
        iter ++;
    }
}

void chip::drawInput(int inputRowPos, int inputColPos) {
    int targetRow = calRowPos(inputRowPos);
    int targetCol = calColPos(inputColPos);
    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::red);
    if(inputColPos == 0) {
        p.drawRect(targetCol-20-2, targetRow, 20, _length);
        p.setPen(Qt::red);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol-50, targetRow+20, tr("IN"));
    }
    else if(inputColPos == _colNum - 1) {
        p.drawRect(targetCol+_length+2, targetRow, 20, _length);
        p.setPen(Qt::red);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol+_length+30, targetRow+_length/2, tr("IN"));
    }
    else if(inputRowPos == 0) {
        p.drawRect(targetCol, targetRow-22, _length, 20);
        p.setPen(Qt::red);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol + _length/4, targetRow-25, tr("IN"));
    }
    else if(inputRowPos == _rowNum-1) {
        p.drawRect(targetCol, targetRow+_length+2, _length, 20);
        p.setPen(Qt::red);
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
    p.setBrush(Qt::blue);
    if(_outputColPos == 1) {
        p.drawRect(targetCol-20-2, targetRow, 20, _length);
        p.setPen(Qt::blue);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol-70, targetRow+20, tr("OUT"));
    }
    else if(_outputColPos == _colNum) {
        p.drawRect(targetCol+_length+2, targetRow, 20, _length);
        p.setPen(Qt::blue);
        QFont font("Arial");
        font.setPixelSize(20);
        p.setFont(font);
        p.drawText(targetCol+_length+30, targetRow+_length/2, tr("OUT"));
    }
    else if(_outputRowPos == 1) {
        p.drawRect(targetCol, targetRow-22, _length, 20);
        p.setPen(Qt::blue);
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

void chip::paintEvent(QPaintEvent *) {
    if(ready) {
        drawChip();
        drawInput(2,1);
        multiset<QString>::iterator iter=inputPos.begin();
        while(iter!=inputPos.end())
        {
            QString str = *iter;
            QByteArray ba=str.toLatin1();
            char *c=ba.data();
            int inputRow, inputCol;
            sscanf(c, "%d %d", &inputCol, &inputRow);
//            qDebug() << inputRow;
//            qDebug() << inputCol;
            drawInput(inputRow-1, inputCol-1);
            iter ++;
        }
        drawOutput();
        drawWater();
    }
}

void chip::operateReverse(command op) {
    if(op.type == "Input") {
        command revCommand(op.time, "Output", op.tarRow, op.tarCol);
        operate(revCommand, true);
    }
    else if(op.type == "Output") {
        //command revCommand(op.time, "Input", op.tarRow, op.tarCol);
        waterDrop newDrop(op.tarRow, op.tarCol, NEW_ID, usedColor.top());
        usedColor.pop();
        water.insert(newDrop);
    }
    else if(op.type == "Move") {
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
        waterDrop newDrop(row, col, target->id, target->color);
        water.erase(target);
        water.insert(newDrop);
    }
    else if(op.type == "Split1") {
        command revCommand(op.time, "Merge2", op.tarRow, op.tarCol);
        operate(revCommand, true);
    }
    else if(op.type == "Split2") {
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
        waterDrop newDrop((row+newRow)/2, (col+newCol)/2, NEW_ID, usedColor.top(), op.dir);
        usedColor.pop();
        usedColor.push(target1->color);
        usedColor.push(target2->color);
        water.erase(target1);
        water.erase(target2);
        water.insert(newDrop);
    }
    else if(op.type == "Merge1") {
        multiset<waterDrop>::iterator target = getDrop(op.tarRow, op.tarCol);
        if(target == water.end()) {
            QMessageBox::critical(this, tr("Error"), tr("Error: water drop not exist!"));
            return;
        }
        if(target->dir == 1) {
            waterDrop newDrop1(target->row-1, target->col, NEW_ID, usedColor.top());
            usedColor.pop();
            waterDrop newDrop2(target->row+1, target->col, NEW_ID, usedColor.top());
            usedColor.pop();
            water.insert(newDrop1);
            water.insert(newDrop2);
        }
        else if(target->dir == 2) {
            waterDrop newDrop1(target->row, target->col-1, NEW_ID, usedColor.top());
            usedColor.pop();
            waterDrop newDrop2(target->row, target->col+1, NEW_ID, usedColor.top());
            usedColor.pop();
            water.insert(newDrop1);
            water.insert(newDrop2);
        }
        water.erase(target);
    }
    else if(op.type == "Merge2") {
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
            waterDrop newDrop(op.tarRow, op.tarCol, NEW_ID, NEW_COLOR);
            water.insert(newDrop);
        }
    }
    else if(op.type == "Move") {
        if(!mute)
            moveSound->play();
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
        waterDrop newDrop(newRow, newCol, target->id, target->color);
        water.erase(target);
        water.insert(newDrop);
    }
    else if(op.type == "Output") {
        if(!mute && (op.tarRow != _outputRowPos || op.tarCol != _outputRowPos)) {
            QMessageBox::critical(this, tr("Error"), tr("Error: not an output port!"));
        }
        else {
            multiset<waterDrop>::iterator target = getDrop(op.tarRow, op.tarCol);
            if(target == water.end()) {
                QMessageBox::critical(this, tr("Error"), tr("Error: water drop not exist!"));
                return;
            }
            usedColor.push(target->color);
            water.erase(target);
        }
    }
    else if(op.type == "Split1") {
        if(!mute)
            stretchSound->play();
        multiset<waterDrop>::iterator target = getDrop(op.tarRow, op.tarCol);
        if(target == water.end()) {
            QMessageBox::critical(this, tr("Error"), tr("Error: water drop not exist!"));
            return;
        }
        waterDrop newDrop(target->row, target->col, target->id, target->color, op.dir);
        water.erase(target);
        water.insert(newDrop);
    }
    else if(op.type == "Split2") {
        if(!mute)
            splitSound->play();
        multiset<waterDrop>::iterator target = getDrop(op.tarRow, op.tarCol);
        if(target == water.end()) {
            QMessageBox::critical(this, tr("Error"), tr("Error: water drop not exist!"));
            return;
        }
        if(target->dir == 1) {
            waterDrop newDrop1(target->row-1, target->col, NEW_ID, NEW_COLOR);
            waterDrop newDrop2(target->row+1, target->col, NEW_ID, NEW_COLOR);
            water.insert(newDrop1);
            water.insert(newDrop2);
        }
        else if(target->dir == 2) {
            waterDrop newDrop1(target->row, target->col-1, NEW_ID, NEW_COLOR);
            waterDrop newDrop2(target->row, target->col+1, NEW_ID, NEW_COLOR);
            water.insert(newDrop1);
            water.insert(newDrop2);
        }
        usedColor.push(target->color);
        water.erase(target);
    }
    else if(op.type == "Merge1") {
        if(!mute)
            mergeSound->play();
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
        waterDrop newDrop((row+newRow)/2, (col+newCol)/2, NEW_ID, NEW_COLOR, op.dir);
        usedColor.push(target1->color);
        usedColor.push(target2->color);
        water.erase(target1);
        water.erase(target2);
        water.insert(newDrop);
    }
    else if(op.type == "Merge2") {
        if(!mute)
            stretchSound->play();
        multiset<waterDrop>::iterator target = getDrop(op.tarRow, op.tarCol);
        if(target == water.end()) {
            QMessageBox::critical(this, tr("Error"), tr("Error: water drop not exist!"));
            return;
        }
        waterDrop newDrop(target->row, target->col, target->id, target->color);
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
    if(currentTime == 0) {
        QMessageBox::critical(this, tr("Information"), tr("Unable to simulate forward"));
        return;
    }
    currentTime --;
    emit(timeChanged(currentTime));
    //qDebug() << getPrev()->time << currentTime;
    while(getPrev()->time >= currentTime) {
        if(curCommand == commands.begin())
            break;
        //qDebug() << "---";
        operateReverse(*getPrev());
        curCommand --;
    }
    update();
}

void chip::toNext() {
    currentTime ++;
    while(curCommand != commands.end() && curCommand->time < currentTime) {
        operate(*curCommand);
        curCommand ++;
    }
    update();
    emit timeChanged(currentTime);
    if(timer->isActive() && curCommand == commands.end())
        timer->stop();
}

void chip::playAll() {
    timer->start(500);

}

void chip::reset() {
    currentTime = 0;
    emit timeChanged(currentTime);
    curCommand = commands.begin();
    water.clear();
    while(!usedColor.empty())
        usedColor.pop();
    update();
}

//signals

void chip::setReady() {
    ready = true;
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
