#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/files/Images/app-icon.png"));

    a.setOrganizationName("Navin Reddy");
    a.setOrganizationDomain("https://github.com/navinreddy23");
    a.setApplicationName("Qt Server");
    a.setApplicationDisplayName("Qt TCP UDP Server");
    a.setApplicationVersion(GIT_VERSION);

    qDebug() << "Git version: " << GIT_VERSION;

    MainWindow w;

    w.show();
    return a.exec();
}
