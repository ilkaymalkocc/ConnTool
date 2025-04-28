#include <QApplication>
#include <QSplashScreen>
#include "mainwindow.h"

#ifdef _WIN32
#include <windows.h>
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#else
int main(int argc, char *argv[])
#endif
{
#ifdef _WIN32
    QApplication app(__argc, __argv);
#else
    QApplication app(argc, argv);
#endif

    QPixmap pix(":logo.png");
    QSplashScreen splash(pix, Qt::WindowStaysOnTopHint);
    splash.show();
    app.processEvents();

    MainWindow w;
    w.show();

    splash.finish(&w);

    w.raise();
    w.activateWindow();

    return app.exec();
}


