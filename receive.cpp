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
   qDebug() << "--" << cl->receive(pb);
    if (cl->receive(pb)) {
        qDebug() << "--ahsdgfJHFhgfbvlzfdjxngbvz.fdjgvzj,fhgvb<Fxjmgvb<Fjhb<JhdgfB<hgfb<DSh";
    }

//    if (cl->receive(pb)) {
//        if (pb.has_id()) {
//            qDebug() << pb.id();
//            qDebug() << pb.apiversion();
//        }
//    }
}
