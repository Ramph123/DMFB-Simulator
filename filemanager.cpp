#include "filemanager.h"
#include <QDebug>

fileManager::fileManager() {}

bool fileManager::openFile() {
    QString path = QFileDialog::getOpenFileName(nullptr,"Open",QDir::homePath(),"Text File(*.txt)");
    file = new QFile(path);
    if(file->exists()) {
        file->open(QIODevice::ReadOnly | QIODevice::Text);
        while (!file->atEnd()) {
            QByteArray line = file->readLine();
            QString str(line);
            inputList.append(str);
            //qDebug() << str;
        }
        file->close();
        return true;
    }
    return false;
}

int fileManager::getMoveDir(int row, int col, int desRow, int desCol) {
    qDebug() << "cal" << row << col << desRow << desCol;
    if(row - 1 == desRow && col == desCol) //up
        return 1;
    if(row + 1 == desRow && col == desCol) // down
        return 2;
    if(row == desRow && col - 1 == desCol) // left
        return 3;
    if(row == desRow && col + 1 == desCol) // right
        return 4;
    return 0;
}

void fileManager::parse() {
    for(int i = 0; i < inputList.size(); i ++)
    {
        QString tmp = inputList.at(i);
        if(i == inputList.size() - 1)
            tmp.chop(1);
        else
            tmp.chop(2);
        QStringList l = tmp.split(' ');
        QString type = l.at(0);
        l = l.at(1).split(',');
        qDebug() << l;
        int time = l.at(0).toInt();
        if(type == "Input" || type == "Output") {
            command inputCommand(time, type, l.at(2).toInt(), l.at(1).toInt());
            //qDebug() << "!!!!!!!" << inputCommand.tarRow << inputCommand.tarCol;
            commandList.insert(inputCommand);
            //qDebug() << "IO inserted";
        }
        else if(type == "Move") {
            int col = l.at(1).toInt(), row = l.at(2).toInt();
            int desCol = l.at(3).toInt(), desRow = l.at(4).toInt();
            command inputCommand(time, type, row, col, getMoveDir(row, col, desRow, desCol));
            commandList.insert(inputCommand);
            //qDebug() << "Move inserted";
        }
        else if (type == "Mix") {
            int row, col, desRow, desCol;
            int curStep = 0;
            while(2 * curStep + 4 <= l.size()) {
                col = l.at(2*curStep+1).toInt();
                row = l.at(2*curStep+2).toInt();
                desCol = l.at(2*curStep+3).toInt();
                desRow = l.at(2*curStep+4).toInt();
                command inputCommand(time + curStep, "Move", row, col, getMoveDir(row, col, desRow, desCol));
                commandList.insert(inputCommand);
                curStep ++;
            }
        }
        else if (type == "Split") {
            int col = l.at(1).toInt(), row = l.at(2).toInt();
            int desCol = l.at(3).toInt(), desRow = l.at(4).toInt();
            int dir = (int)(getMoveDir(row, col, desRow, desCol)+1)/2;
            command inputCommand1(time, "Split1", row, col, dir);
            command inputCommand2(time+1, "Split2", row, col, dir);
            commandList.insert(inputCommand1);
            commandList.insert(inputCommand2);
        }
        else if (type == "Merge") {
            int col = l.at(1).toInt(), row = l.at(2).toInt();
            int desCol = l.at(3).toInt(), desRow = l.at(4).toInt();
            int midRow = (row+desRow)/2, midCol = (col+desCol)/2;
            int dir = (getMoveDir(row, col, midRow, midCol)+1)/2;
            command inputCommand1(time, "Merge1", midRow, midCol, dir);
            command inputCommand2(time+1, "Merge2", midRow, midCol, dir);
            commandList.insert(inputCommand1);
            commandList.insert(inputCommand2);
        }
    }
}
