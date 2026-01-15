#pragma once
#include <QMainWindow>
#include <QObject>
class HzMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    HzMainWindow(QWidget* parent = nullptr);
    ~HzMainWindow();

};