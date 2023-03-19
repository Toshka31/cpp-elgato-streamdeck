#pragma once

#include <QMainWindow>
#include <QComboBox>

#include "DeckWidgets/OriginalV2Widget.h"


class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr,
                        Qt::WindowFlags flags = Qt::WindowFlags());

private:
    OriginalV2Widget *m_wdg;
};
