#include <QtGui>
#include "earcomm.h"
#include "data.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EARComm w;

    w.show();
    return a.exec();
}
