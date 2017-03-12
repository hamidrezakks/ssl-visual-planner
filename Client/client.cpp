#include "client.h"

Client::Client(QObject *parent) :
    QObject(parent)
{
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::LocalHost, 10002);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void Client::readyRead()
{
    QByteArray buffer;
    buffer.resize(socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    socket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);

    PlanBook pb;
    if( !pb.ParseFromArray(buffer, buffer.size())){
        qDebug() << "ERROR";
        return;
    }

    int numOfAgents = -1;
    if( pb.plans().size() != 0 )
        numOfAgents = pb.plans().Get(0).agents_size();

    int numOfPlans = pb.plans().size();

    qDebug() << "apiversion: " << pb.apiversion();
    qDebug() << "id: " << pb.id();
    qDebug() << "plan size: " << numOfPlans;
    qDebug() << "num of agents: " << numOfAgents;
    qDebug() << "----------------------------\n";
}
