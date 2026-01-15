#include "HzSplashWindow.hpp"
#include <filesystem>

HzSplashWindow::HzSplashWindow(QWidget* parent)
{
    // resize(800,800);
    QPixmap pixmap(":res/imgs/splash.png");
    pixmap = pixmap.scaled(800, 600, Qt::KeepAspectRatio);
    setPixmap(pixmap);
    setWindowFlag(Qt::WindowStaysOnTopHint);
}
HzSplashWindow::~HzSplashWindow()
{
}

void HzSplashWindow::mousePressEvent(QMouseEvent*)
{
    return;
}