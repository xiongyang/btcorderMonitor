#ifndef QUERYDIALOG_H
#define QUERYDIALOG_H

#include <QDialog>
#include <QAbstractSocket>
#include <QDateTimeEdit>
#include <QNetworkReply>
#include <map>
#include <set>

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
    QueryDialog(QWidget *parent = 0, QString key = "",  QString sce = "",QString symbol = "");
    ~QueryDialog();

public slots:
    void onClickQueryTrade();
    void onClickQueryOrder();
    void onClickQueryAccount();
    void onClickQueryBalance();
//    void onClientConnected();
//    void onClientDisconnect();
//    void onClientError(QAbstractSocket::SocketError);
private:
    QNetworkReply* getRequest(const QString &apimethod, const std::map<QString, QString>& params);
    void getOrderRequest(const std::map<QString, QString>& params);
    void getTradeRequest(const std::map<QString, QString>& params);
    void handleOrder(QNetworkReply* orderquery);
    void handleTrade(QNetworkReply* orderquery);

    void handleAccount(QNetworkReply* query);
    void handleBalance(QNetworkReply* query);



private:
    QLineEdit* querySymbol;
    QLineEdit* accessKey;
    QLineEdit* passKey;
    QLineEdit* accountId;
    QDateTimeEdit* dateStart;
    QDateTimeEdit* dateEnd;

    QLabel* statusLable;
    uint64_t lastTradeId {0};
    uint64_t preLastTradeId {0};
    bool isPrintHeader{false};


//    QTcpSocket* clientConnection;
    QNetworkAccessManager* qnam;
    std::set<int> accountIDs;
};

#endif // QUERYDIALOG_H
