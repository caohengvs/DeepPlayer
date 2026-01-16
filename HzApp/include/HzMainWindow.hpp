#pragma once
#include <QMainWindow>
#include <QObject>
#include <thread>
#include "HzEngine.hpp"

#include <QImage>
#include <QMutex>
#include <QPainter>
#include <QWidget>


class HzVideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HzVideoWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
    }

    // 更新图像的接口
public slots:
    void onUpdateImage(const QImage& img)
    {
        m_mutex.lock();
        // 使用 .copy() 确保线程安全，防止外部内存释放导致崩溃
        m_showImage = img.copy();
        m_mutex.unlock();
        update();  // 触发 paintEvent
    }

protected:
    void paintEvent(QPaintEvent* event) override
    {
        QPainter painter(this);
        painter.fillRect(rect(), Qt::black);  // 底色设为黑色

        m_mutex.lock();
        if (!m_showImage.isNull())
        {
            // 保持比例缩放并绘制在中心
            QImage scaledImg = m_showImage.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            int x = (width() - scaledImg.width()) / 2;
            int y = (height() - scaledImg.height()) / 2;
            painter.drawImage(x, y, scaledImg);
        }
        m_mutex.unlock();
    }

private:
    QImage m_showImage;
    QMutex m_mutex;
};

class HzMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    HzMainWindow(QWidget* parent = nullptr);
    ~HzMainWindow();

private:
    void display();

signals:
    void updateImage(const QImage& img);

private:
    std::thread m_thdDisply;
    std::atomic<bool> m_bRun{false};
    HzEngine m_engine;
    HzVideoWidget* m_videoWidget{nullptr};
};