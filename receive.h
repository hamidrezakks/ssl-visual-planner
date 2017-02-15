#ifndef RECEIVE_H
#define RECEIVE_H


#include <QObject>
#include <QTimer>
#include <QDebug>
#include "client.h"

class Receive : public QObject
{

    Q_OBJECT

public:
    Receive(QObject *parent = Q_NULLPTR);
    QTimer* timer;
    Client * cl;
private:
    int counter;
public slots:
    void re();
signals:

};

#endif // RECEIVE_H
