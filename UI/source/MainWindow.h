#ifndef ELGATO_STREAMDECK_MAINWINDOW_H
#define ELGATO_STREAMDECK_MAINWINDOW_H

#include "StreamDeckClient.h"

#include <QMainWindow>

class MainWindow : public QMainWindow
{
public:
    MainWindow(
            std::shared_ptr<StreamDeckClient> client,
            QWidget *parent = nullptr,
            Qt::WindowFlags flags = Qt::WindowFlags());
private:
    std::shared_ptr<StreamDeckClient> m_client;
};


#endif //ELGATO_STREAMDECK_MAINWINDOW_H
