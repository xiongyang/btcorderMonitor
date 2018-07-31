#ifndef BITMEXQUERYDIALOG_H
#define BITMEXQUERYDIALOG_H

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

class BitmexQueryDialog : public QDialog
{
    Q_OBJECT

public:
    BitmexQueryDialog(QWidget *parent = 0);
    ~BitmexQueryDialog();

public slots:
    void onClickQueryTrade();
    void onClickQueryOrder();
    void onClickQueryAccount();


//    void onClientConnected();
//    void onClientDisconnect();
//    void onClientError(QAbstractSocket::SocketError);
private:
    QNetworkReply* getRequest(const QString &apimethod, QJsonObject params);
    void getOrderRequest(const std::map<QString, QString>& params);
    void getTradeRequest(const std::map<QString, QString>& params);
    void handleOrder(QNetworkReply* orderquery);
    void handleTrade(QNetworkReply* orderquery);
    void handleAccount(QNetworkReply* query);


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
    int tradeCountStart {0};
};

#endif // BITMEXQUERYDIALOG_H
