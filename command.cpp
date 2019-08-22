#include "command.h"

command::command() {}
command::command(int time, QString type, int tarRow, int tarCol) :
    time(time), type(type), tarRow(tarRow), tarCol(tarCol) {}
command::command(int time, QString type, int tarRow, int tarCol, int dir) :
    time(time), type(type), dir(dir), tarRow(tarRow), tarCol(tarCol) {}
