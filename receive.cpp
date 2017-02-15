#include "receive.h"

Receive::Receive(QObject* parent) : QObject(parent)
{
    timer = new QTimer(this);
    timer->start(10);
    cl = new Client;
    counter = 0;
    connect(timer, SIGNAL(timeout()), this, SLOT(re()));
}


void Receive::re() {
    PlanBook pb;
    qDebug() << "--" << counter++;

    if (cl->receive(pb)) {
        if (pb.has_id()) {
            qDebug() << pb.id();
            qDebug() << pb.apiversion();
        }
    }
}
