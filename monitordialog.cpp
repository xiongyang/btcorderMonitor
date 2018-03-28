#include "monitordialog.h"

#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QHostAddress>

#include <QLabel>

MonitorDialog::MonitorDialog(QWidget *parent)
    : QDialog(parent)
    , addressEdit(new QLineEdit("47.52.44.14:47100"))
    , messageEdit(new QLineEdit("Leg=btcusdt@huobi#LegAWeight=bitfinex@1#LegAWeight=okex@1#LegAWeight=bian@1#LegAWeight=gdax@1#refMaWeight=0.005#legMaWeight=0.0001#pricePrecis=100#pplPrecis=10000#stopEnter=0#stopExit=0#feeRate=0.0004#enterSignal=25#resendFactor=0.0003#enterSignalFactor=1#spreadBP=0#spreadResendBp=15"))
    , clientConnection(new QTcpSocket)
    , statusLable(new QLabel)
{

    QFormLayout *formLayout = new QFormLayout;

    connect(clientConnection, &QTcpSocket::connected, this, &MonitorDialog::onClientConnected);
    connect(clientConnection, &QTcpSocket::disconnected, this, &MonitorDialog::onClientDisconnect);
    connect(clientConnection, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &MonitorDialog::onClientError);



    formLayout->addRow("address",addressEdit);
    formLayout->addRow("message",messageEdit);
    formLayout->addRow("status",statusLable);


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    QPushButton* connectButton = new QPushButton("connect");
    //connect(connectButton, &QPushButton::clicked, this, &MonitorDialog::onClickConnect);

    connect(connectButton, &QAbstractButton::clicked, this, &MonitorDialog::onClickConnect);

    QPushButton* quitButton = new QPushButton("Quit");
    connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);

    QPushButton* sendButton = new QPushButton("Send");
    connect(sendButton, &QAbstractButton::clicked, this, &MonitorDialog::onClickSend);

    QPushButton* resetButton = new QPushButton("Reset");
    connect(resetButton, &QAbstractButton::clicked, this, &MonitorDialog::onClickReset);

    buttonBox->addButton(connectButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(sendButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(resetButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    mainLayout->addWidget(buttonBox);
    this->setLayout(mainLayout);
}

MonitorDialog::~MonitorDialog()
{

}
void MonitorDialog::onClickReset()
{
    clientConnection->close();
    QString peer = clientConnection->peerAddress().toString();
    peer += ":" + QString::number( clientConnection->peerPort());
    statusLable->setText("Connection Close " + peer);
}

void MonitorDialog::onClientConnected()
{
    QString peer = clientConnection->peerAddress().toString();
    peer += ":" + QString::number( clientConnection->peerPort());

    statusLable->setText("Connection Connected to " + peer);
}

void MonitorDialog::onClientDisconnect()
{
    QString peer = clientConnection->peerAddress().toString();
    peer += ":" + QString::number( clientConnection->peerPort());

    statusLable->setText("Connection Close to " + peer);
}

void MonitorDialog::onClientError(QAbstractSocket::SocketError err)
{
    qDebug() << err;
    statusLable->setText("Connection Error ");
}



void MonitorDialog::onClickConnect()
{
    QString address = addressEdit->text();
    auto list = address.split(":");
    qDebug() << "Click Connect" <<  address ;

    clientConnection->connectToHost(list[0], list[1].toInt());
}

void MonitorDialog::onClickSend()
{
    std::string msg = messageEdit->text().toStdString();

    uint32_t len = msg.size();
    QByteArray buf;
    buf.append((char*)&len, sizeof(len));
    buf.append(msg.c_str(), len);
    clientConnection->write(buf);
    qDebug() << "Click Send" <<  msg.c_str()  << " msgSize is " << sizeof(len) << " + " << len << " --> "<< buf.size();
}
