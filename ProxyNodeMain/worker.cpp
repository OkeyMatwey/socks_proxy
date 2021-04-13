#include "worker.h"

Worker::Worker()
{

}

Worker::~Worker()
{

}

void Worker::start()
{
    for(int i=0;i < 100;i++)
    {
        QTcpSocket *server = new QTcpSocket;
        QTcpSocket *world = new QTcpSocket;
        map_server[server] = world;
        map_world[world] = server;
        connect(server, &QTcpSocket::connected, this, &Worker::connected);
        connect(server, &QTcpSocket::disconnected, this, &Worker::serverDisconnected);

        server->connectToHost(url, 81);
        while(!server->waitForConnected())
        {
            server->abort();
        }
    }
}

void Worker::connected()
{
    QTcpSocket *server = reinterpret_cast<QTcpSocket*>(sender());
    qDebug() << "Connection";
    connect(server, &QTcpSocket::readyRead, this, &Worker::readyRead);
}

void Worker::readyRead()
{
    QTcpSocket *server = reinterpret_cast<QTcpSocket*>(sender());
    QTcpSocket *world = map_server[server];

    disconnect(server, &QTcpSocket::readyRead, this, &Worker::readyRead);

    //VER
    if(*server->read(1).data() == 0x05)
    {
        //CMD
        char ch = *server->read(1).data();
        //CONNECT
        if(ch == 0x01)
        {
            //RSV
            if(*server->read(1).data() == 0x00)
            {
               //Type address
               ch = *server->read(1).data();
               if(ch == 0x01)
               {
                   uint32_t address_in = *reinterpret_cast<uint32_t*>(server->read(4).data());
                   uint16_t port_in = *reinterpret_cast<uint32_t*>(server->read(2).data());
                    #if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
                    const QHostAddress address(qFromBigEndian(address_in));
                    const uint16_t port = qFromBigEndian(port_in);
                    #else
                    const QHostAddress address(address_in);
                    const uint16_t port = port_in;
                    #endif

                    qDebug() << address << port;
                    world->connectToHost(address.toString(),port);
                    if(world->waitForConnected())
                    {
                        connect(server, &QTcpSocket::readyRead, this, &Worker::server2world);
                        connect(world, &QTcpSocket::readyRead, this, &Worker::world2server);
                        connect(world, &QTcpSocket::disconnected, this, &Worker::worldDisconected);
                        #if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
                            scw.address = qFromBigEndian(world->localAddress().toIPv4Address());
                            scw.port = qFromBigEndian(world->localPort());
                        #else
                            scw.address = world->localAddress().toIPv4Address();
                            scw.port = world->localPort();
                        #endif
                        server->write(reinterpret_cast<char*>(&scw), 10);
                        server->flush();
                        return;
                    }
                    else
                    {
                         qDebug() << "wait";
                    }
               }
               if(ch == 0x03)
               {
                   uint8_t domen_len = *reinterpret_cast<uint8_t*>(server->read(1).data());
                   QString domen = QString(server->read(domen_len));
                   uint16_t port_in = *reinterpret_cast<uint32_t*>(server->read(2).data());

                    #if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
                    const uint16_t port = qFromBigEndian(port_in);
                    #else
                    const uint16_t port = port_in;
                    #endif

                    qDebug() << domen << port;
                    world->connectToHost(domen, port);
                    if(world->waitForConnected())
                    {
                        connect(server, &QTcpSocket::readyRead, this, &Worker::server2world);
                        connect(world, &QTcpSocket::readyRead, this, &Worker::world2server);
                        connect(world, &QTcpSocket::disconnected, this, &Worker::worldDisconected);

                        #if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
                            scw.address = qFromBigEndian(world->localAddress().toIPv4Address());
                            scw.port = qFromBigEndian(world->localPort());
                        #else
                            scw.address = world->localAddress().toIPv4Address();
                            scw.port = world->localPort();
                        #endif
                        server->write(reinterpret_cast<char*>(&scw), 10);
                        server->flush();
                        return;
                    }
                    else
                    {
                         qDebug() << "wait";
                    }
               }
               if(ch == 0x04)
               {
                   qDebug() << "ipv6";
               }
            }
        }
        //BIND
        if(ch == 0x01)
        {}
        //UDP
        if(ch == 0x02)
        {}
    }
    close(server,world);
}

void Worker::server2world()
{
    QTcpSocket *server = reinterpret_cast<QTcpSocket*>(sender());
    QTcpSocket *world = map_server[server];
    world->write(server->readAll());
    world->flush();
}

void Worker::world2server()
{
    QTcpSocket *world = reinterpret_cast<QTcpSocket*>(sender());
    QTcpSocket *server = map_world[world];
    server->write(world->readAll());
    server->flush();
}

void Worker::serverDisconnected()
{
    QTcpSocket *server = reinterpret_cast<QTcpSocket*>(sender());
    QTcpSocket *world = map_server[server];
    close(server,world);
    qDebug() << "server disconnect";
}

void Worker::worldDisconected()
{
    QTcpSocket *world = reinterpret_cast<QTcpSocket*>(sender());
    QTcpSocket *server = map_world[world];
    close(server,world);
    qDebug() << "world dissconnect";
}

void Worker::close(QTcpSocket* server, QTcpSocket*world)
{
    disconnect(server,nullptr, nullptr, nullptr);
    disconnect(world,nullptr, nullptr, nullptr);
    server->close();
    world->close();
    server->abort();
    world->abort();

    connect(server, &QTcpSocket::connected, this, &Worker::connected);
    connect(server, &QTcpSocket::disconnected, this, &Worker::serverDisconnected);

    server->connectToHost(url, 81);
    while(!server->waitForConnected())
    {
        server->abort();
    }
}


