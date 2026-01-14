#include <QApplication>
class HzApp : public QApplication
{
    Q_OBJECT
public:
    HzApp(int& argc, char** argv);
    ~HzApp();

private:
    void initLogger();

private:
    bool m_isQuit;
};