#include <QtGui/QApplication>
#include "roundabout.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Roundabout w;
    w.show();

    return a.exec();
}
