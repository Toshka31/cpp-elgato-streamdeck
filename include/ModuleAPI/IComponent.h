#pragma once

#include "IDeviceButtonRestricted.h"
#include "ComponentParameters.h"

#include <string>
#include <vector>
#include <memory>

class IStreamDeck;

class IComponent
{
public:
    virtual void init(std::shared_ptr<IDeviceButtonRestricted> device) = 0;

    virtual std::string name() const = 0;

    virtual std::vector<unsigned char> getImage() const = 0;

    virtual void tick() = 0;

    virtual void actionPress() = 0;

    virtual void actionRelease() = 0;

    virtual ComponentParameters getParameters() = 0;

    virtual void setParameters(ComponentParameters::Variables variables) = 0;

    virtual ~IComponent() = default;
};
