#include "monitordialog.h"
#include "querydialog.h"
#include <QApplication>

#include "logger.h"
#include <QLabel>

int main(int argc, char *argv[])
{
    initLogger();
   // Logger << "Start BtcOrderMonitor";
    QApplication a(argc, argv);

    if(argc == 1)
    {
        MonitorDialog w;
        w.show();
        return a.exec();
    }

    std::string type = argv[1];
    if(type == "c")
    {
         std::string address ;
         std::string config;
        if(argc >= 3)
        {
            address = argv[2];
        }
        if(argc >= 4)
        {
            config = argv[3];
        }

        MonitorDialog w(nullptr,address.c_str(),  config.c_str());
        w.show();
        return a.exec();
    }
    else if(type == "q")
    {
        std::string key ;
        std::string sec ;
        std::string symbol;
       if(argc >= 3)
       {
           key = argv[2];
       }
       if(argc >= 4)
       {
           sec = argv[3];
       }

       if(argc >= 5)
       {
           symbol = argv[4];
       }


        QueryDialog q(nullptr,key.c_str(), sec.c_str(), symbol.c_str());
        q.show();
        return a.exec();
    }
}
