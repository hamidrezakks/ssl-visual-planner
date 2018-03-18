#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int c = a.arguments().count();

    for (int i = 0; i < c; i++) {
        QString arg = a.arguments().at(i).toLower();
        if (arg == "open") {

        } else if (arg == "save") {

        } else if (arg == "update") {
            if (i != c-1) {
                QString s = a.arguments().at(i+1);
                Gupdate = true;
                Gdir = s;
            }
        }
    }

    MainWindow w;
    QRect r = w.geometry();
    r.moveCenter(QApplication::desktop()->availableGeometry().center());
    w.setGeometry(r);

    if (!Gupdate)
        w.show();

    return a.exec();
}
