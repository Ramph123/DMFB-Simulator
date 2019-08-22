#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QFile>
#include <QFileDialog>
#include "command.h"
#include <set>

using std::multiset;

class fileManager
{
public:
    fileManager();
    bool openFile();
    void parse();
    bool fileLoaded() {return (file != nullptr) && (file->exists());}
    QStringList inputList;
    multiset<command>& getCommandList() {return commandList;}

private:
    QFile *file;
    multiset<command> commandList;
    int getMoveDir(int row, int col, int desRow, int desCol);
};

#endif // FILEMANAGER_H
