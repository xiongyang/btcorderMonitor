#ifndef MONITORDIALOG_H
#define MONITORDIALOG_H

#include <QDialog>
#include <QAbstractSocket>

QT_BEGIN_NAMESPACE
class QTcpSocket;
class QLineEdit;
class QLabel;

QT_END_NAMESPACE

class MonitorDialog : public QDialog
{
    Q_OBJECT

public:
    MonitorDialog(QWidget *parent = 0);
    ~MonitorDialog();

public slots:
    void onClickConnect();
    void onClickSend();
    void onClickReset();

    void onClientConnected();
    void onClientDisconnect();
    void onClientError(QAbstractSocket::SocketError);

private:
    QLineEdit* addressEdit;
    QLineEdit* messageEdit;
    QLabel* statusLable;


    QTcpSocket* clientConnection;
};

#endif // MONITORDIALOG_H
