#include "mainwindow.h"

#include <QApplication>
#include <QTimer>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    QSplashScreen splash(QPixmap(":/images/bfsh.jpg").scaled(800, 600, Qt::IgnoreAspectRatio, Qt::FastTransformation)); // 替换为你的图片路径
    splash.show();

    QTimer::singleShot(2000, &splash, [&splash, &w]() {
        splash.close();
        w.show();
    });

    return a.exec();
}
