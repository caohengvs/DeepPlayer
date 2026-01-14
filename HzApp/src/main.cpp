#include "HzApp.hpp"
#include <QMainWindow>
int main(int argc, char *argv[]) 
{
    // TODO: Implement the main function
    HzApp app(argc, argv);

    QMainWindow win;
    win.show();



    return app.exec();
}