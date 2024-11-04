#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include <QObject>
#include <QTcpSocket>

class NetworkController : public QObject
{
    Q_OBJECT
public:
    explicit NetworkController(QObject *parent = nullptr);
    void connectToHost(const QString &hostAddress, quint16 port);
    void sendData(const QString &data);

private slots:
    void onConnected();
    void onDataReceived();

private:
    QTcpSocket *tcpSocket;
};

#endif // NETWORKCONTROLLER_H
