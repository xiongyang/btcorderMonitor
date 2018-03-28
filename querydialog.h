#ifndef QUERYDIALOG_H
#define QUERYDIALOG_H

#include <QDialog>
#include <QAbstractSocket>
#include <QDateTimeEdit>
#include <QNetworkReply>
#include <map>

QT_BEGIN_NAMESPACE
class QTcpSocket;
class QLineEdit;
class QLabel;
class QNetworkAccessManager;

QT_END_NAMESPACE

class QueryDialog : public QDialog
{
    Q_OBJECT

public:
    QueryDialog(QWidget *parent = 0);
    ~QueryDialog();

public slots:
    void onClickQueryTrade();
    void onClickQueryOrder();


//    void onClientConnected();
//    void onClientDisconnect();
//    void onClientError(QAbstractSocket::SocketError);
private:
    QNetworkReply* getRequest(const QString &apimethod, const std::map<QString, QString>& params);
    void getOrderRequest(const std::map<QString, QString>& params);
    void getTradeRequest(const std::map<QString, QString>& params);
    void handleOrder(QNetworkReply* orderquery);
    void handleTrade(QNetworkReply* orderquery);


private:
    QLineEdit* querySymbol;
    QLineEdit* accessKey;
    QLineEdit* passKey;
    QDateTimeEdit* dateStart;
    QDateTimeEdit* dateEnd;

    QLabel* statusLable;
    int lastTradeId {0};
    int preLastTradeId {0};
    bool isPrintHeader{false};


//    QTcpSocket* clientConnection;
    QNetworkAccessManager* qnam;
};

#endif // QUERYDIALOG_H
