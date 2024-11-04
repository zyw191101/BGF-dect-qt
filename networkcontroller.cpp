#include "networkcontroller.h"
#include <QDebug>

NetworkController::NetworkController(QObject *parent) : QObject(parent), tcpSocket(new QTcpSocket(this))
{
    // 连接QTcpSocket信号到槽
    connect(tcpSocket, &QTcpSocket::connected, this, &NetworkController::onConnected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &NetworkController::onDataReceived);
}

void NetworkController::connectToHost(const QString &hostAddress, quint16 port)
{
    tcpSocket->connectToHost(hostAddress, port);  // 连接到指定IP和端口
}

void NetworkController::sendData(const QString &data)
{
    if (tcpSocket->state() == QAbstractSocket::ConnectedState) {
        tcpSocket->write(data.toUtf8());  // 将数据转换为UTF-8字节并发送
        tcpSocket->flush();  // 刷新缓冲区，确保数据发送
    } else {
        qDebug() << "Not connected!";
    }
}

void NetworkController::onConnected()
{
    qDebug() << "Connected to host!";
}

void NetworkController::onDataReceived()
{
    QByteArray data = tcpSocket->readAll();  // 读取接收到的数据
    qDebug() << "Data received:" << data;
}
