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
#include "util.h"

QueryDialog::QueryDialog(QWidget *parent, QString key ,  QString sec , QString symbol )
    : QDialog(parent)
{
    this->setWindowTitle("huobi");

    dateStart = new QDateTimeEdit(QDate::currentDate());
    dateEnd = new QDateTimeEdit(QDate::currentDate());
    qnam = new QNetworkAccessManager;

    querySymbol = new QLineEdit(symbol,this);

    accountId = new QLineEdit("", this);

    accessKey = new QLineEdit(key, this);
    passKey = new QLineEdit(sec, this);

    statusLable = new QLabel(this);
    statusLable->setWordWrap(true);
    statusLable->setTextInteractionFlags(Qt::TextSelectableByMouse);
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("accessKey",accessKey);
    formLayout->addRow("passKey",passKey);
    formLayout->addRow("symbol",querySymbol);
    formLayout->addRow("accountId",accountId);
    formLayout->addRow("start",dateStart);

    formLayout->addRow("end",dateEnd);
    formLayout->addRow("status",statusLable);


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    QPushButton* orderButton = new QPushButton("Order");
    connect(orderButton, &QAbstractButton::clicked, this, &QueryDialog::onClickQueryOrder);

    QPushButton* tradeButton = new QPushButton("Trade");
    connect(tradeButton, &QAbstractButton::clicked, this, &QueryDialog::onClickQueryTrade);

    QPushButton* accountButton = new QPushButton("Account");
    connect(accountButton, &QAbstractButton::clicked, this, &QueryDialog::onClickQueryAccount);


    QPushButton* quitButton = new QPushButton("Quit");
    connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);

    QPushButton* balanceButton = new QPushButton("Balance");
    connect(balanceButton, &QAbstractButton::clicked, this, &QueryDialog::onClickQueryBalance);

    buttonBox->addButton(orderButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(tradeButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(accountButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(balanceButton, QDialogButtonBox::ActionRole);
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
    //para["states"] = "filled";
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

void QueryDialog::onClickQueryAccount()
{

     std::map<QString, QString> para;
     QString query("/v1/account/accounts");
     auto reply = getRequest(query,para);
     connect(reply, &QNetworkReply::finished, [=](){handleAccount(reply);});
//    std::map<QString, QString> para;
//    para["symbol"] = querySymbol->text();
//    para["start-date"] = dateStart->date().toString("yyyy-MM-dd");
//    para["end-date"] = dateEnd->date().toString("yyyy-MM-dd");
//    para["states"] = "filled";
//    para["size"] = "10";

//    QString query("/v1/order/orders/");
//    auto reply = getRequest(query,para);
//
}
//const std::string currentDateTime()
//{
//    auto reply = getRequest("/v1/account/accounts", std::map<QString,QString>());
//    connect(reply,  &QNetworkReply::finished, [=](){handleAccount(reply);} );
//}

void QueryDialog::onClickQueryBalance()
{
    char buf[256];
    std::string id = accountId->text().toStdString();
    if(!id.empty())
    {

        sprintf(buf, "/v1/account/accounts/%s/balance", id.c_str());
        QString url(buf);
        auto reply = getRequest(url,std::map<QString,QString>());
        connect(reply,  &QNetworkReply::finished, [=](){handleBalance(reply);} );
    }
    else
    {
        for(auto each : accountIDs )
        {
            sprintf(buf, "/v1/account/accounts/%d/balance", each);
            QString url(buf);
            auto reply = getRequest(url,std::map<QString,QString>());
            connect(reply,  &QNetworkReply::finished, [=](){handleBalance(reply);} );
        }

    }

}


QNetworkReply* QueryDialog::getRequest(const QString &apimethod, const std::map<QString, QString>& params)
{
    QString apiType = "GET\n";

    std::map<QString, QString> ps(params);
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


    QString result(apikey);
    result.append("&Signature=");
    result.append(signstr);

    qDebug() << result;


    QString rs("https://api.huobi.pro");
    rs =  rs + apimethod + "?" + result;
    QUrl url = rs;
    QNetworkRequest request(url);

    qDebug() << "Send Request " << url;



    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.71 Safari/537.36");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply* reply = qnam->get(request);
    return reply;
}


void QueryDialog::handleOrder(QNetworkReply *r)
{
    QByteArray data =  r->readAll();
    Logger << data.toStdString();
    qDebug() << data;
    statusLable->setText("Finish Order Query" );
}

void QueryDialog::handleTrade(QNetworkReply *r)
{
    QByteArray data = r->readAll();
  //  Logger << "Handle Trade " << data.toStdString();
    QJsonObject json =  getJson(data);
    // if(json[""])
    // qDebug() <<json;
    if(json["status"].toString() == "ok")
    {

        QJsonArray a = json["data"].toArray();
        for(int i = 0; i != a.size(); ++ i)
        {
            QJsonObject trade = a[i].toObject();
            uint64_t id  = trade["id"].toDouble();
            if(id != preLastTradeId)
            {
                uint64_t time =  trade["created-at"].toDouble();
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
                        << ","   << trade["id"].toDouble()
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
           // para["states"] = "filled";
            para["size"] = "10";
            para["from"] = QString::number(lastTradeId);
            para["direct"] = "next";
            QString query("/v1/order/matchresults");
            auto reply = getRequest(query,para);
            connect(reply, &QNetworkReply::finished, [=](){handleTrade(reply);});
        }
        else
        {
            statusLable->setText("Finish Trade Query " + QString::number(lastTradeId) + " PreLast: "  + QString::number(preLastTradeId));
        }

        preLastTradeId = lastTradeId;
    }

}

void QueryDialog::handleAccount(QNetworkReply *query)
{
    QByteArray r = query->readAll();
    statusLable->setText(r);

    QJsonObject obj = getJson(r);
    QJsonArray accounts = obj["data"].toArray();
    for(int i = 0; i!= accounts.size(); ++i)
    {
        QJsonObject a = accounts[i].toObject();
        int id = a["id"].toInt();
        accountIDs.insert(id);
    }

}

void QueryDialog::handleBalance(QNetworkReply *query)
{
    QByteArray r = query->readAll();
    Logger << QJsonDocument::fromJson(r).toJson().toStdString();
}
