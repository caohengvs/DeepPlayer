#include "HzApp.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "HzLogger.hpp"
#include "HzUtils.hpp"
#include "HzEngine.hpp"

HzApp::HzApp(int& argc, char** argv)
    : QApplication(argc, argv)
{
    setApplicationName("DeepPlayer");
    initLogger();
    HzEngine engine;
    engine.Init("test.mkv");
    qInfo() << "Starting DeepPlayer";
}

HzApp::~HzApp()
{
    qInfo() << "Stopping DeepPlayer";
    HzLogger::s_DeleteInstance();
}

void HzApp::initLogger()
{
    auto& loggger = HzLogger::s_GetInstance();

    std::ostringstream oss;
    oss << "logs/system_" << HzUtils::GetTime().c_str() << ".log";

    loggger.Init("DeepPlayer", HzLogger::DEBUG_L, true, false, oss.str());
    qInstallMessageHandler(
        [](QtMsgType type, const QMessageLogContext& context, const QString& msg)
        {
            auto& logger = HzLogger::s_GetInstance();

            switch (type)
            {
                case QtDebugMsg:
                {
                    logger.Debug(context.file, context.line, context.function) << msg.toStdString();
                    break;
                }
                case QtInfoMsg:
                {
                    logger.Info(context.file, context.line, context.function) << msg.toStdString();
                    break;
                }
                case QtWarningMsg:
                {
                    logger.Warn(context.file, context.line, context.function) << msg.toStdString();
                    break;
                }
                case QtCriticalMsg:
                {
                    logger.Error(context.file, context.line, context.function) << msg.toStdString();
                    break;
                }
                case QtFatalMsg:
                    logger.Critical(context.file, context.line, context.function) << msg.toStdString();
                    abort();
            }
        });
}