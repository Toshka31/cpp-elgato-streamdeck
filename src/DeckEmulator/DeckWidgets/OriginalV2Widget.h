#pragma once

#include "../Transport/Mediator.h"

#include <QMap>
#include <QPushButton>


class OriginalV2Widget : public QWidget
{
    Q_OBJECT

public:
    explicit OriginalV2Widget(QWidget *parent = 0);

    enum traits
    {
        BUTTONS_NUM_ROWS = 3,
        BUTTONS_NUM_COLS = 5,
        BUTTON_SIZE      = 72,
        BUTTON_ICON_SIZE = BUTTON_SIZE - 2,

        KEY_STATES_PAYLOAD_OFFSET = 4,
        IMAGE_PAYLOAD_OFFSET      = 8
    };

private slots:
    void buttonPressed();
    void buttonReleased();
    void buttonChangeImage(int index, std::vector<unsigned char> data);

private:
    QMap<int, QPushButton *> m_buttons;
    Mediator<OriginalV2Widget> m_transport;
};
