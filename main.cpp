#include <QApplication>
#include "camerawindow.h"

static void initialize()
{
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(2, &wsaData);
#endif
}

int main(int argc, char *argv[])
{
    initialize();

    QApplication a(argc, argv);
    CameraWindow w;
    w.show();

    return a.exec();
}
