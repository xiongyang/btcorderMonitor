#include "util.h"

#include <time.h>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QDebug>

const std::string currentDateTime()
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *gmtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format

    //    2018-01-06T14%3A52%3A59
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H%%3A%M%%3A%S", &tstruct);

    return buf;
}

QJsonObject getJson(QByteArray  reply)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply, &jsonError);
    if (jsonError.error != QJsonParseError::NoError)
    {
        qDebug() << "Error Parse Reply from " << reply
                 << " Error " << jsonError.errorString();
        return QJsonObject();
    }
    else
    {
        return std::move(jsonDoc.object());
    }
}
