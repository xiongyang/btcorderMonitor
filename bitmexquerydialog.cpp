#include "bitmexquerydialog.h"

#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QHostAddress>
#include <QDate>
#include <QLabel>
#include  <QDateTimeEdit>
#include <QMessageAuthenticationCode>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QJsonArray>

#include "logger.h"
#include "util.h"

QByteArray makeBitMexsignature(const char* s, const char* method, const char* apiAccess, const char* data, int expire)
{
    std::string message(method);
    message = message + apiAccess  + std::to_string(expire) + data;
    QByteArray sig = QMessageAuthenticationCode::hash(message.c_str(), s, QCryptographicHash::Sha256).toHex();
    return sig;
}



BitmexQueryDialog::BitmexQueryDialog(QWidget *parent)
    : QDialog(parent)
{
    dateStart = new QDateTimeEdit(QDate::currentDate());
    dateEnd = new QDateTimeEdit(QDate::currentDate());
    qnam = new QNetworkAccessManager;
    this->setToolTip("BitMexQuery");

    //    dateStart->setCalendarPopup(true);
    //    dateEnd->setCalendarPopup(true);

    this->setWindowTitle("BitMex");

    querySymbol = new QLineEdit("ethusdt",this);


    accessKey = new QLineEdit("bitmexKey", this);
    passKey = new QLineEdit("2d5afaab-51a4d408-8a10ed5c-16879", this);

    statusLable = new QLabel(this);
    statusLable->setWordWrap(true);
    statusLable->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QFormLayout *formLayout = new QFormLayout;



    //    connect(clientConnection, &QTcpSocket::connected, this, &MonitorDialog::onClientConnected);
    //    connect(clientConnection, &QTcpSocket::disconnected, this, &MonitorDialog::onClientDisconnect);
    //    connect(clientConnection, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &MonitorDialog::onClientError);


    formLayout->addRow("accessKey",accessKey);
    formLayout->addRow("passKey",passKey);

    formLayout->addRow("symbol",querySymbol);
    formLayout->addRow("start",dateStart);
    formLayout->addRow("end",dateEnd);
    formLayout->addRow("status",statusLable);


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    QPushButton* orderButton = new QPushButton("Order");
    //connect(connectButton, &QPushButton::clicked, this, &MonitorDialog::onClickConnect);

    connect(orderButton, &QAbstractButton::clicked, this, &BitmexQueryDialog::onClickQueryOrder);

    QPushButton* quitButton = new QPushButton("Quit");
    connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);

    QPushButton* tradeButton = new QPushButton("Trade");
    connect(tradeButton, &QAbstractButton::clicked, this, &BitmexQueryDialog::onClickQueryTrade);


    QPushButton* accountButton = new QPushButton("AccountID");
    connect(accountButton, &QAbstractButton::clicked, this, &BitmexQueryDialog::onClickQueryAccount);

    buttonBox->addButton(orderButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(tradeButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(accountButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    mainLayout->addWidget(buttonBox);
    this->setLayout(mainLayout);
}


BitmexQueryDialog::~BitmexQueryDialog()
{

}

void BitmexQueryDialog::onClickQueryTrade()
{
    tradeCountStart = 0;
    QJsonObject para;
    para["symbol"] = querySymbol->text();
    para["startTime"] = dateStart->date().toString("yyyy-MM-dd");
    para["endTime"] = dateEnd->date().toString("yyyy-MM-dd");
    para["count"] = 200;
    para["start"] = tradeCountStart;

    QString query("/trade");
    auto reply = getRequest(query,para);
    connect(reply, &QNetworkReply::finished, [=](){handleTrade(reply);});
}

void BitmexQueryDialog::onClickQueryOrder()
{
    std::map<QString, QString> para;
    para["symbol"] = querySymbol->text();
    para["start-date"] = dateStart->date().toString("yyyy-MM-dd");
    para["end-date"] = dateEnd->date().toString("yyyy-MM-dd");
    para["states"] = "filled";
    para["size"] = "10";

//    QString query("/v1/order/orders/");
//    auto reply = getRequest(query,para);
//    connect(reply, &QNetworkReply::finished, [=](){handleOrder(reply);});
}

void BitmexQueryDialog::onClickQueryAccount()
{
//   auto reply = getRequest("/v1/account/accounts", std::map<QString,QString>());
//   connect(reply,  &QNetworkReply::finished, [=](){handleAccount(reply);} );
}

QNetworkReply* BitmexQueryDialog::getRequest(const QString &apimethod, QJsonObject params)
{
    QByteArray ba  =  QJsonDocument(params).toJson(QJsonDocument::Compact);

    int expire = time(NULL)+ 60;
    QByteArray sig = makeBitMexsignature(passKey->text().trimmed().toLatin1(), "GET", apimethod.toLatin1().data(), ba.data(), expire);
    QNetworkRequest nr;
    nr.setUrl(QUrl("https://www.bitmex.com" + apimethod));
    nr.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    nr.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
    nr.setRawHeader("Accept", "application/json");
    //nr.setRawHeader("Connection", "keep-alive");
    // nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
    nr.setRawHeader("api-expires", std::to_string(expire).c_str());
    nr.setRawHeader("api-key",accessKey->text().trimmed().toLatin1());
    nr.setRawHeader("api-signature",  sig);

    QNetworkReply* r =  qnam->post(nr, ba);
    connect(r, &QNetworkReply::finished, [=](){this->handleTrade(r);});
    return r;
}


void BitmexQueryDialog::handleOrder(QNetworkReply *r)
{
    qDebug() << r->readAll();
}

void BitmexQueryDialog::handleTrade(QNetworkReply *r)
{
    QJsonObject json =  getJson(r->readAll());
    // if(json[""])
    qDebug() <<json;
    if(json["status"].toString() == "ok")
    {

        QJsonArray a = json["data"].toArray();
        for(int i = 0; i != a.size(); ++ i)
        {
              QJsonObject trade = a[i].toObject();
               int id  = trade["id"].toInt();
               if(id != preLastTradeId)
               {
                  uint64_t time =  trade["created-at"].toVariant().toLongLong();
                  uint64_t time_s = time / 1000;
                  uint64_t millsec = time % 1000;

               QDateTime  t=  QDateTime::fromTime_t(time_s);


               if(!isPrintHeader)
               {
                   isPrintHeader = true;
                   Logger << "Symbol,Dir,Price,Qty,Commison,Commison_Point,tradeID,date,time";
               }

                  Logger << trade["symbol"].toString().toStdString()
                           << "," << trade["type"].toString().toStdString()
                           << ","<<trade["price"].toString().toDouble()
                           << ","  <<  trade["filled-amount"].toString().toDouble()
                           << ","   << trade["filled-fees"].toString().toDouble()
                           << ","   << trade["filled-points"].toString().toDouble()
                           << ","   << trade["id"].toInt()
                        //   << "," << trade["created-at"].toVariant().toLongLong()
                          << "," <<  t.toString("yyMMdd,hh:mm:ss").toStdString()
                          << "." << millsec

                              ;
                   lastTradeId = id;
               }
        }

        if(lastTradeId != 0 && preLastTradeId !=  lastTradeId)
        {
            QJsonObject para;
            para["symbol"] = querySymbol->text();
            para["startTime"] = dateStart->date().toString("yyyy-MM-dd");
            para["endTime"] = dateEnd->date().toString("yyyy-MM-dd");
            para["count"] = 200;
            para["start"] = tradeCountStart;

            QString query("/trade");
            auto reply = getRequest(query,para);
            connect(reply, &QNetworkReply::finished, [=](){handleTrade(reply);});


        }
        else
        {
             statusLable->setText("Finish Trade Query" );
        }

        preLastTradeId = lastTradeId;
    }

}

void BitmexQueryDialog::handleAccount(QNetworkReply *query)
{
    statusLable->setText(query->readAll());
}
