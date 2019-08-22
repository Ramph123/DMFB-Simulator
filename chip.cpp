#include "chip.h"
#include <iostream>
#include <QDebug>
#include <QMessageBox>
#include <cmath>
#include <vector>

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
        }
    }
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
            p.setBrush(stainColor[i][j]);
            p.drawEllipse(targetCol, targetRow, radius*2, radius*2);
        }
    }
}

void chip::drawPollution() {
    for(int i = 1; i <= _rowNum; i ++) {
        for(int j = 1; j <= _colNum; j ++) {
            int targetRow = calRowPos(i);
            int targetCol = calColPos(j - 1);
            QPainter p(this);
            QFont font;
            font.setPixelSize(20);
            p.setFont(font);
            p.setPen(Qt::black);
            p.drawText(targetRow, targetCol, QString::number(stainCnt[j][i]));
        }
    }
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
        if(printPollution) {
            drawPollution();
        }
        else {
            drawStain();
            drawWater();
        }
    }
}

void chip::operateReverse(command op) {
    if(op.type == "Input") {
        qDebug() << "Rev" << "Input";
        command revCommand(op.time, "Output", op.tarRow, op.tarCol);
        operate(revCommand, true);
    }
    else if(op.type == "Output") {
        qDebug() << "Rev" << "Output";
        //command revCommand(op.time, "Input", op.tarRow, op.tarCol);
        waterDrop newDrop(op.tarRow, op.tarCol, NEW_ID, usedColor.top());
        usedColor.pop();
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
        usedColor.push(target1->color);
        usedColor.push(target2->color);
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
            waterDrop newDrop(op.tarRow, op.tarCol, NEW_ID, NEW_COLOR);
            stainCommand newLog;
            newLog.time = op.time;
            newLog.row = op.tarRow;
            newLog.col = op.tarCol;
            newLog.prevColor = stainColor[op.tarRow][op.tarCol];
            stainLog.push(newLog);
            stainColor[op.tarRow][op.tarCol] = newDrop.color;
            stainCnt[op.tarRow][op.tarCol] ++;


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
        waterDrop newDrop(newRow, newCol, target->id, target->color, target->dir, target->size);
        stainCommand newLog;
        newLog.time = op.time;
        newLog.row = newRow;
        newLog.col = newCol;
        newLog.prevColor = stainColor[newRow][newCol];
        stainLog.push(newLog);
        stainColor[newRow][newCol] = newDrop.color;
        stainCnt[newRow][newCol] ++;
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
            stainCommand newLog;
            newLog.time = op.time;
            newLog.row = newDrop1.row;
            newLog.col = newDrop1.col;
            newLog.prevColor = stainColor[newDrop1.row][newDrop1.col];
            stainLog.push(newLog);
            stainColor[newDrop1.row][newDrop1.col] = newDrop1.color;
            stainCnt[newDrop1.row][newDrop1.col] ++;
            stainCommand newLog2;
            newLog2.time = op.time;
            newLog2.row = newDrop2.row;
            newLog2.col = newDrop2.col;
            newLog2.prevColor = stainColor[newDrop2.row][newDrop2.col];
            stainLog.push(newLog);
            stainColor[newDrop2.row][newDrop2.col] = newDrop2.color;
            stainCnt[newDrop2.row][newDrop2.col] ++;
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
            stainLog.push(newLog);
            stainColor[newDrop1.row][newDrop1.col] = newDrop1.color;
            stainCnt[newDrop1.row][newDrop1.col] ++;
            stainCommand newLog2;
            newLog2.time = op.time;
            newLog2.row = newDrop2.row;
            newLog2.col = newDrop2.col;
            newLog2.prevColor = stainColor[newDrop2.row][newDrop2.col];
            stainLog.push(newLog);
            stainColor[newDrop2.row][newDrop2.col] = newDrop2.color;
            stainCnt[newDrop2.row][newDrop2.col] ++;
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
        waterDrop newDrop(target->row, target->col, target->id, target->color, 0, 1.414);
        stainCommand newLog;
        newLog.time = op.time;
        newLog.row = newDrop.row;
        newLog.col = newDrop.col;
        newLog.prevColor = stainColor[newDrop.row][newDrop.col];
        stainLog.push(newLog);
        stainColor[newDrop.row][newDrop.col] = newDrop.color;
        stainCnt[newDrop.row][newDrop.col] ++;
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
    qDebug() << "!!!" << water.size();
    if(currentTime == 0) {
        QMessageBox::critical(this, tr("Information"), tr("Unable to simulate forward"));
        return;
    }
    currentTime --;
    emit(timeChanged(currentTime));
    while(!stainLog.empty() && stainLog.top().time >= currentTime) {
        stainCommand c = stainLog.top();
        stainLog.pop();
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
    //qDebug() << "!!!" << water.size();
    if(!conatraint)
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
    timer->start(500);

}

void chip::reset() {
    currentTime = 0;
    emit timeChanged(currentTime);
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
    if(water.size() < 2)
        return true;
    vector<int> rows;
    vector<int> cols;
    multiset<waterDrop>::iterator drop1;
    multiset<waterDrop>::iterator drop2;
    //qDebug() << (int)(drop1==water.begin());
    for(drop1 = water.begin(); drop1 != water.end(); drop1 ++) {
        for(drop2 = drop1; drop2 != water.end(); drop2 ++) {
            //qDebug() << "!!!!";
            if(drop1 == drop2)
                continue;
            //int row1 = drop1->row, col1 = drop1->col;
            //int row2 = drop2->row, col2 = drop2->col;
            //if(abs(row1-row2) <= 1 && abs(col1-col2) <= 1) {
                //qDebug() << row1 << col1;
                //qDebug() << row2 << col2;
                //return false;
            //}
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
            if(drop2->dir == 0) {
                rows.push_back(drop2->row);
                cols.push_back(drop2->col);
            }
            else if(drop2->dir == 1) {
                rows.push_back(drop2->row);
                cols.push_back(drop2->col-1);
                rows.push_back(drop2->row);
                cols.push_back(drop2->col+1);
            }
            else if(drop2->dir == 2) {
                rows.push_back(drop2->row-1);
                cols.push_back(drop2->col);
                rows.push_back(drop2->row+1);
                cols.push_back(drop2->col);
            }
            for(int i = 0; i < rows.size(); i ++) {
                for(int j = i+1; j < rows.size(); j ++) {
                    if(abs(rows[i]-rows[j]) <= 1 && abs(cols[i]-cols[j]) <= 1)
                        return false;
                }
            }
        }
    }
    return true;
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
