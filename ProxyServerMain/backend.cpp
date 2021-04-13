#include "backend.h"


Backend::Backend(QObject *parent) : QObject(parent)
{
    METHOD_TRUE[0] = 0x05;
    METHOD_TRUE[1] = 0x02;
    METHOD_FALSE[0] = 0x05;
    METHOD_FALSE[1] = 0xff;
    AUTH_TRUE[0] = 0x01;
    AUTH_TRUE[1] = 0x00;
    AUTH_FALSE[0] = 0x01;
    AUTH_FALSE[1] = 0xff;

    connect(&server_client, &QTcpServer::newConnection, this, &Backend::ClientConnection);
    server_client.listen(QHostAddress::Any, 80);

    connect(&server_node, &QTcpServer::newConnection, this, &Backend::NodeConnection);
    server_node.listen(QHostAddress::Any, 81);
}

void Backend::ClientConnection()
{
    QTcpSocket *client = server_client.nextPendingConnection();
    client->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    qDebug() << "ClientConnection" << client << map_client.size() << free_node.size();
    if(!free_node.empty())
    {
        QTcpSocket *node = free_node.dequeue();

        map_client[client] = node;
        map_node[node] = client;

        connect(client, &QTcpSocket::readyRead, this, &Backend::readyRead);
        connect(client, &QTcpSocket::disconnected, this, &Backend::onClientDisconnected);
        connect(node, &QTcpSocket::readyRead, this, &Backend::node2client);
        connect(node, &QTcpSocket::disconnected, this, &Backend::onNodeDisconnected);
        return;
    }
    client->close();
}

void Backend::readyRead()
{
    QTcpSocket *client = reinterpret_cast<QTcpSocket*>(sender());
    disconnect(client, &QTcpSocket::readyRead, this, &Backend::readyRead);
    if(*client->read(1).data() == 0x05)
    {
        uint8_t nmethods = *reinterpret_cast<uint8_t*>(client->read(1).data());
        if(nmethods > 0)
        {
            if(client->read(nmethods).contains(0x02))
            {
                client->write(METHOD_TRUE, 2);
                client->flush();
                if(client->waitForReadyRead())
                {
                    if(*client->read(1).data() == 0x01)
                    {
                        uint8_t username_len = *reinterpret_cast<uint8_t*>(client->read(1).data());
                        QString username = client->read(username_len).data();
                        uint8_t passworld_len = *reinterpret_cast<uint8_t*>(client->read(1).data());
                        QString passworld = client->read(passworld_len).data();
                        if(true)
                        {
                            connect(client, &QTcpSocket::readyRead, this, &Backend::client2node);
                            client->write(AUTH_TRUE, 2);
                            client->flush();
                            return;
                        }
                        else
                        {
                            client->write(AUTH_FALSE, 2);
                            client->flush();
                        }
                    }
                }
            }
            else
            {
                client->write(METHOD_FALSE, 2);
                client->flush();
            }
        }
    }
    client->close();
}

void Backend::client2node()
{
    QTcpSocket *client = reinterpret_cast<QTcpSocket*>(sender());
    qDebug() << "client2node" << client;
    QTcpSocket *node = map_client[client];
    QByteArray data = client->readAll();
//    qDebug() << data.toHex();
    node->write(data);
    node->flush();
}

void Backend::onClientDisconnected()
{
    QTcpSocket *client = reinterpret_cast<QTcpSocket*>(sender());
    QTcpSocket *node = map_client[client];
    qDebug() << "onClientDisconnected" << client;
    disconnect(node,nullptr, nullptr, nullptr);
    disconnect(client,nullptr, nullptr, nullptr);
    node->close();
    map_node.remove(node);
    map_client.remove(client);
}

void Backend::NodeConnection()
{
    QTcpSocket *node = server_node.nextPendingConnection();
    qDebug() << "NodeConnection" << node;
    free_node.enqueue(node);
}

void Backend::node2client()
{
    QTcpSocket *node = reinterpret_cast<QTcpSocket*>(sender());
    qDebug() << "node2client" << node;
    QTcpSocket *client = map_node[node];
    QByteArray data = node->readAll();
//    qDebug() << data.toHex();
    client->write(data);
    client->flush();
}

void Backend::onNodeDisconnected()
{
    QTcpSocket *node = reinterpret_cast<QTcpSocket*>(sender());
    QTcpSocket *client = map_node[node];
    qDebug() << "onNodeDisconnected" << node;
    disconnect(client, nullptr, nullptr, nullptr);
    disconnect(node, nullptr, nullptr, nullptr);
    client->close();
    map_node.remove(node);
    map_client.remove(client);
}
