#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QUdpSocket>

#include "../ssl-playbook.pb.h"

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);

signals:

public slots:
    void readyRead();

private:
    QUdpSocket *socket;

};


#endif // CLIENT_H
