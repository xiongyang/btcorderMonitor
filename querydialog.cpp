#include "querydialog.h"

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

QueryDialog::QueryDialog(QWidget *parent)
    : QDialog(parent)
{
    dateStart = new QDateTimeEdit(QDate::currentDate());
    dateEnd = new QDateTimeEdit(QDate::currentDate());
    qnam = new QNetworkAccessManager;

    //    dateStart->setCalendarPopup(true);
    //    dateEnd->setCalendarPopup(true);

    querySymbol = new QLineEdit("ethusdt",this);


    accessKey = new QLineEdit("8fed21c0-7bd73df5-247decad-ab3e4", this);
    passKey = new QLineEdit("2d5afaab-51a4d408-8a10ed5c-16879", this);

    statusLable = new QLabel(this);
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

    connect(orderButton, &QAbstractButton::clicked, this, &QueryDialog::onClickQueryOrder);

    QPushButton* quitButton = new QPushButton("Quit");
    connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);

    QPushButton* tradeButton = new QPushButton("Trade");
    connect(tradeButton, &QAbstractButton::clicked, this, &QueryDialog::onClickQueryTrade);



    buttonBox->addButton(orderButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(tradeButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    mainLayout->addWidget(buttonBox);
    this->setLayout(mainLayout);
}


QueryDialog::~QueryDialog()
{

}

void QueryDialog::onClickQueryTrade()
{
    std::map<QString, QString> para;
    para["symbol"] = querySymbol->text();
    para["start-date"] = dateStart->date().toString("yyyy-MM-dd");
    para["end-date"] = dateEnd->date().toString("yyyy-MM-dd");
    para["states"] = "filled";
    para["size"] = "10";

    QString query("/v1/order/matchresults");
    auto reply = getRequest(query,para);
    connect(reply, &QNetworkReply::finished, [=](){handleTrade(reply);});
}

void QueryDialog::onClickQueryOrder()
{
    std::map<QString, QString> para;
    para["symbol"] = querySymbol->text();
    para["start-date"] = dateStart->date().toString("yyyy-MM-dd");
    para["end-date"] = dateEnd->date().toString("yyyy-MM-dd");
    para["states"] = "filled";
    para["size"] = "10";

    QString query("/v1/order/orders/");
    auto reply = getRequest(query,para);
    connect(reply, &QNetworkReply::finished, [=](){handleOrder(reply);});
}
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

QNetworkReply* QueryDialog::getRequest(const QString &apimethod, const std::map<QString, QString>& params)
{
    QString apiType = "GET\n";

    std::map<QString, QString> ps(params);

  //  for(auto& each : params)
  //  {
      //  qDebug() << each.first << "  " << each.second;
   // }

    ps["AccessKeyId"] = accessKey->text().trimmed();
    ps["SignatureMethod"] = "HmacSHA256";
    ps["SignatureVersion"] = "2";
    ps["Timestamp"] = currentDateTime().c_str();



    QString apikey;
    for(auto & each : ps)
    {
        apikey.append(each.first + "=" + each.second + "&");
    }
    // remove last &
    apikey.resize(apikey.size()-1);

    QByteArray  calcString (apiType.toLatin1());
    calcString.append("api.huobi.pro\n");
    calcString.append(apimethod);
    calcString.append("\n");
    calcString.append(apikey);

    QByteArray sign = QMessageAuthenticationCode::hash(calcString, passKey->text().trimmed().toLatin1(), QCryptographicHash::Sha256).toBase64();
    QByteArray signstr = sign.toPercentEncoding();
    QString parms = apikey  + "&Signature=" + signstr;




    QString rs("https://api.huobi.pro");
    rs =  rs + apimethod + "?" + parms;
    QUrl url = rs;
    QNetworkRequest request(url);


    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.71 Safari/537.36");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply* reply = qnam->get(request);
    return reply;
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

void QueryDialog::handleOrder(QNetworkReply *r)
{
    qDebug() << r->readAll();
}

void QueryDialog::handleTrade(QNetworkReply *r)
{
    QJsonObject json =  getJson(r->readAll());
    // if(json[""])
   // qDebug() <<json;
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
            std::map<QString, QString> para;
            para["symbol"] = querySymbol->text();
            para["start-date"] = dateStart->date().toString("yyyy-MM-dd");
            para["end-date"] = dateEnd->date().toString("yyyy-MM-dd");
            para["states"] = "filled";
            para["size"] = "10";
            para["from"] = QString::number(lastTradeId);
            para["direct"] = "next";
            QString query("/v1/order/matchresults");
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