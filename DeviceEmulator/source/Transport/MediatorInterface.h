#pragma once

#include <QObject>


class MediatorInterface : public QObject
{
    Q_OBJECT

signals:
    void buttonImage(int index, std::vector<unsigned char> data);

public:
    virtual void buttonPress(int index) = 0;
    virtual void buttonRelease(int index) = 0;
};
