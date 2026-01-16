#include "HzMainWindow.hpp"
#include <sstream>

HzMainWindow::HzMainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    resize(800, 600);
    m_videoWidget = new HzVideoWidget(this);
    setCentralWidget(m_videoWidget);
    connect(this, &HzMainWindow::updateImage, m_videoWidget, &HzVideoWidget::onUpdateImage, Qt::QueuedConnection);
    m_engine.Init("test.mkv");
    m_bRun = true;
    m_thdDisply = std::thread(&HzMainWindow::display, this);
}

HzMainWindow::~HzMainWindow()
{
    m_bRun = false;
    if (m_thdDisply.joinable())
        m_thdDisply.join();
}

void HzMainWindow::display()
{
    int nIndex = 0;
    while (m_bRun)
    {
        auto pFrame = m_engine.GetFrame();
        if (pFrame.data == nullptr)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        QImage qimg(pFrame.data, pFrame.width, pFrame.height, pFrame.linesize, QImage::Format_RGB888);
        if (qimg.isNull())
            return;

        emit updateImage(qimg.copy());
        // std::stringstream ss;
        // ss << "test" << nIndex++ << ".jpg";
        // qimg.save(ss.str().c_str(), "JPG", 90);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
