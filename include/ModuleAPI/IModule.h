#pragma once

#include "ProvidedProfile.h"

#include <string>
#include <vector>
#include <memory>
#include <optional>

class IComponent;

class IModule
{
 public:
  virtual std::string getName() const = 0;

  virtual std::shared_ptr<IComponent> createComponent(const std::string& name) = 0;

  virtual std::vector<std::string> getComponentList() const = 0;

  virtual std::optional<ProvidedProfile> getProvidedProfile() const = 0;
};