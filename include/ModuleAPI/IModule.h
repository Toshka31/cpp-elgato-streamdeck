#pragma once

#include <string>
#include <vector>
#include <memory>

class IComponent;

class IModule
{
 public:
  virtual std::string getName() const = 0;

  virtual std::shared_ptr<IComponent> createComponent(const std::string& name) = 0;

  virtual std::vector<std::string> getComponentList() const = 0;
};