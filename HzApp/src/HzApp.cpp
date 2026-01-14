#include "HzApp.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "Logger.hpp"
#include "HzUtils.hpp"

HzApp::HzApp(int& argc, char** argv)
    : QApplication(argc, argv)
{
    setApplicationName("DeepPlayer");
    initLogger();

    qInfo() << "Starting DeepPlayer";
}

HzApp::~HzApp()
{
    qInfo() << "Stopping DeepPlayer";
    Logger::s_DeleteInstance();
}

void HzApp::initLogger()
{
    auto& loggger = Logger::s_GetInstance();

    std::ostringstream oss;
    oss << "logs/system_" << HzUtils::GetTime().c_str() << ".log";

    loggger.Init("DeepPlayer", Logger::DEBUG_L, true, false, oss.str());
    qInstallMessageHandler(
        [](QtMsgType type, const QMessageLogContext& context, const QString& msg)
        {
            auto& logger = Logger::s_GetInstance();

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