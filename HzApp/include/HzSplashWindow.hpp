#pragma once
#include <QSplashScreen>
#include <thread>
class HzSplashWindow : public QSplashScreen
{
    Q_OBJECT
public:
    HzSplashWindow(QWidget* parent = nullptr);
    ~HzSplashWindow();

private:
    virtual void mousePressEvent(QMouseEvent*) override;

};