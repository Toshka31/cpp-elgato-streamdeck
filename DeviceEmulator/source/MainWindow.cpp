#include "MainWindow.h"

#include <QVBoxLayout>


MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    auto *central_widget = new QWidget(this);
    m_wdg = new OriginalV2Widget(central_widget);
    auto *lt_main = new QVBoxLayout();
    lt_main->addWidget(m_wdg);

    central_widget->setLayout(lt_main);
    setCentralWidget(central_widget);
}
