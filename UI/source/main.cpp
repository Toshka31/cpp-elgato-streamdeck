#include "MainWindow.h"
#include "StreamDeckClient.h"

#include <QApplication>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    auto client = std::make_shared<StreamDeckClient>();

    MainWindow mainWin(client);
    mainWin.show();
    return QApplication::exec();
}