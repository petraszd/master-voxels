#include <mainwindow.h>
#include <config.h>
#include <QApplication>
#include <QTime>

int main(int argc, char *argv[])
{
    QApplication qapp(argc, argv);
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));

    Config::init("constants.cfg");

    MainWindow win;
    win.show();

    return qapp.exec();
}

