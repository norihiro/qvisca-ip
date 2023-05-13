#include <QApplication>
#include "camerawindow.h"

static void initialize()
{
#if defined(_WIN32) || defined(OS_WINDOWS)
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
