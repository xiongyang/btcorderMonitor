#include "monitordialog.h"
#include "querydialog.h"
#include <QApplication>

#include "logger.h"


int main(int argc, char *argv[])
{
     initLogger();
    QApplication a(argc, argv);
    MonitorDialog w;
    QueryDialog q;
    w.show();
    q.show();
    return a.exec();
}
