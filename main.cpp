#include <QApplication>
#include "camerawindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CameraWindow w;
    w.show();

    return a.exec();
}
