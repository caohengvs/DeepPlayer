#include "HzApp.hpp"
#include "HzMainWindow.hpp"
#include "HzSplashWindow.hpp"
int main(int argc, char* argv[])
{
    HzApp app(argc, argv);

    // HzSplashWindow splash;
    // splash.show();

    HzMainWindow mainWindow;
    mainWindow.show();

    // splash.finish(&mainWindow);

    // mainWindow.raise();           // 拿到最前
    // mainWindow.activateWindow();  // 抢焦点

    return app.exec();
}