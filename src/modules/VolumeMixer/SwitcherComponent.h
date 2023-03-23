#pragma once

#include "module.h"

class MixerSwitchComponent : public IComponent
{
  DECLARE_MODULE_COMPONENT(MixerModule, MixerSwitchComponent)
 public:
    void init(std::shared_ptr<IStreamDeck> deck);

    std::string name() const override;
    void tick() override;

    void actionPress(std::shared_ptr<IStreamDeck> deck, ushort key) override;

    void actionRelease(std::shared_ptr<IStreamDeck> deck, ushort key) override;
    std::vector<unsigned char> getImage() const override;
};