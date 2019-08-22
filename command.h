#ifndef COMMAND_H
#define COMMAND_H

#include <QString>

class command
{
public:
    command();
    command(int time, QString type, int tarRow, int tarCol);
    command(int time, QString type, int tarRow, int tarCol, int dir);
    bool operator< (const command &c) const {
        if(time < c.time)
            return true;
        return false;
    }

    int time;
    QString type;  // input, output, move, split1, split2, merge1, merge2;
    int dir = 0; // Move - 1:up 2:down 3:left 4:right
                 // Split & Merge - 1:up&down 2:left&right
    int tarRow, tarCol;
};

#endif // COMMAND_H
