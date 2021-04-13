#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QtEndian>
#include <QTimer>

struct socks4connectworld
{
    uchar VER = 0x05;
    uchar REP = 0x00;
    uchar RSP = 0x00;
    uchar ATYP = 0x01;
    uint32_t address;
    uint16_t port;
};

class Worker: public QObject
{
    Q_OBJECT
    //QString url = "188.120.241.71";
    QString url = "51.13.84.217";
    QMap<QTcpSocket*, QTcpSocket*> map_server;
    QMap<QTcpSocket*, QTcpSocket*> map_world;
    socks4connectworld scw;
public:
    Worker();
    ~Worker();
public slots:
    void start();
private slots:
    //Server
    void connected();
    void readyRead();
    void server2world();
    void serverDisconnected();
    //World
    void world2server();
    void worldDisconected();
    //
    void close(QTcpSocket* server, QTcpSocket*world);
};

#endif // WORKER_H
