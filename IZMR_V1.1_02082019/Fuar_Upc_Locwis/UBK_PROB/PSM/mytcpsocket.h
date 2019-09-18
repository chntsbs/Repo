#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QThread>
#include "mainwindow.h"
#include <QtNetwork/qnetworkaccessmanager.h>
#include "database.h"



class MyTcpSocket : public QObject
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = 0);
    QTimer *tcptimer;
    bool whitelist_all;
    bool whitelist_version;
    bool person_version;    
    bool sales_send;
    Database *database;
    void doConnect();
    void TcptoThread(QThread &TcpThread);
signals:

public slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();
    void update();
    void tcp_scan_timer();
    void getversion();
    void write(QByteArray bytes);
    void getconfig();
    void getperson();
    void getwhiteversion();
    void getwhiteall();
    void sendsales();
    void saveversion();
    void saveconfig();
    void saveperson();
    void savewhiteversion();
    void savewhiteall();
    void savesales();

private:
    QTcpSocket *socket;
};

#endif // MYTCPSOCKET_H
