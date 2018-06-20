#include "monitordialog.h"
#include "querydialog.h"
#include <QApplication>

#include "logger.h"
#include <QLabel>

int main(int argc, char *argv[])
{
    initLogger();
    QApplication a(argc, argv);
    MonitorDialog w(nullptr, argv[1], argv[2]);
    QueryDialog q;
    w.show();
    if(argc ==  1) q.show();
    return a.exec();
}
