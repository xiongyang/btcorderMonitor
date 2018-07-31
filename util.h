#ifndef UTIL_H
#define UTIL_H

#include <QJsonObject>
#include <QByteArray>
#include <string>

const std::string currentDateTime();
QJsonObject getJson(QByteArray  reply);


#endif // UTIL_H
