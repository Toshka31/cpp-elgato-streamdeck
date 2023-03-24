#pragma once

#include "IDeviceButtonRestricted.h"

#include <string>

class IComponent
{
public:
    virtual void init(std::shared_ptr<IDeviceButtonRestricted> device) = 0;

    virtual std::string name() const = 0;

    virtual std::vector<unsigned char> getImage() const = 0;

    virtual void tick() = 0;

    virtual void actionPress() = 0;

    virtual void actionRelease() = 0;

    virtual ~IComponent() = default;
};
