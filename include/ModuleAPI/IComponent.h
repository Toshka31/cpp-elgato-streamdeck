#pragma once

#include "StreamDeckLib/Device/IStreamDeck.h"

#include <string>

class IComponent
{
 public:
  virtual std::string name() const = 0;

  virtual std::vector<unsigned char> getImage() const = 0;

  virtual void tick() = 0;

  virtual void actionPress(std::shared_ptr<IStreamDeck> deck, ushort key) = 0;

  virtual void actionRelease(std::shared_ptr<IStreamDeck> deck, ushort key) = 0;

  virtual ~IComponent() {}
};
