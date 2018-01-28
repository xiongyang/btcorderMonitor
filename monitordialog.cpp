#include "monitordialog.h"

#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>

MonitorDialog::MonitorDialog(QWidget *parent)
    : QDialog(parent)
    , addressEdit(new QLineEdit("127.0.0.1:47777"))
    , messageEdit(new QLineEdit("hello"))
    , clientConnection(new QTcpSocket)
    , statusLable(new QLabel)
{

    QFormLayout *formLayout = new QFormLayout;


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

    buttonBox->addButton(connectButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(sendButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    mainLayout->addWidget(buttonBox);
    this->setLayout(mainLayout);
}

MonitorDialog::~MonitorDialog()
{

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
