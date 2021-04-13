#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtEndian>
#include <QQueue>

class Backend : public QObject
{
    Q_OBJECT
    char METHOD_TRUE[2];
    char METHOD_FALSE[2];
    char AUTH_TRUE[2];
    char AUTH_FALSE[2];

    QTcpServer server_client;
    QTcpServer server_node;
    QMap<QTcpSocket*, QTcpSocket*> map_node;
    QMap<QTcpSocket*, QTcpSocket*> map_client;
    QQueue<QTcpSocket*> free_node;
public:
    explicit Backend(QObject *parent = nullptr);
private slots:
    //Server Client
    void ClientConnection();
    //Client
    void readyRead();
    void client2node();
    void onClientDisconnected();
    //Server Node
    void NodeConnection();
    //Node
    void node2client();
    void onNodeDisconnected();
signals:

};

#endif // BACKEND_H
