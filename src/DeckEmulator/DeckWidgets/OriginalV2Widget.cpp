#include "OriginalV2Widget.h"

#include <QGridLayout>


OriginalV2Widget::OriginalV2Widget(QWidget *parent)
{
    auto *lt_main = new QGridLayout(this);

    for (auto i = 0; i < BUTTONS_NUM_ROWS; ++i)
    {
        for (auto j = 0; j < BUTTONS_NUM_COLS; ++j)
        {
            auto *btn = new QPushButton(this);
            btn->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
            btn->setToolTip(QString::number(i * BUTTONS_NUM_COLS + j));
            btn->setIconSize(QSize(BUTTON_ICON_SIZE, BUTTON_ICON_SIZE));
            QObject::connect(btn, &QPushButton::pressed, this, &OriginalV2Widget::buttonPressed);
            QObject::connect(btn, &QPushButton::released, this, &OriginalV2Widget::buttonReleased);
            m_buttons.insert(i * BUTTONS_NUM_COLS + j, btn);
            lt_main->addWidget(btn, i, j);
        }
    }
    setLayout(lt_main);
    QObject::connect(&m_transport, &Mediator<OriginalV2Widget>::buttonImage, this, &OriginalV2Widget::buttonChangeImage);
}

void OriginalV2Widget::buttonPressed()
{
    auto button = qobject_cast<QPushButton *>(sender());
    Q_ASSERT(button);

    auto tooltop = button->toolTip();
    m_transport.buttonPress(tooltop.toInt());
}

void OriginalV2Widget::buttonReleased()
{
    auto button = qobject_cast<QPushButton *>(sender());
    Q_ASSERT(button);

    auto tooltop = button->toolTip();
    m_transport.buttonRelease(tooltop.toInt());
}
void OriginalV2Widget::buttonChangeImage(int index, std::vector<unsigned char> data)
{
    QPixmap pix;
    pix.loadFromData(data.data(), data.size(), "JPG");
    QTransform myTransform;
    myTransform.rotate(180);
    pix = pix.transformed(myTransform);
    m_buttons[index]->setIcon(QIcon(pix));
}
