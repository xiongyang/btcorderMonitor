#ifndef MONITORDIALOG_H
#define MONITORDIALOG_H

#include <QDialog>

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

private:
    QLineEdit* addressEdit;
    QLineEdit* messageEdit;
    QLabel* statusLable;

    QTcpSocket* clientConnection;
};

#endif // MONITORDIALOG_H
