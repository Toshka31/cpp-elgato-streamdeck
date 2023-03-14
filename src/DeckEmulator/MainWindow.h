#ifndef ELGATO_STREAMDECK_SRC_DECKEMULATOR_MAINWINDOW_H_
#define ELGATO_STREAMDECK_SRC_DECKEMULATOR_MAINWINDOW_H_

#include <QMainWindow>
#include <QComboBox>

#include "DeckWidgets/OriginalV2Widget.h"

class MainWindow : public QMainWindow
{
 public:
  explicit MainWindow(
      QWidget *parent = nullptr,
      Qt::WindowFlags flags = Qt::WindowFlags());
 private:
  QComboBox *m_cmb_deck;
  OriginalV2Widget *m_wdg;
};

#endif //ELGATO_STREAMDECK_SRC_DECKEMULATOR_MAINWINDOW_H_
