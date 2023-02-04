#include "module.h"

class Component3 : public IComponent
{
  DECLARE_MODULE_COMPONENT(module2, Component3)
 public:
  std::string name() const override
  {
    return "Component3";
  };
};

REGISTER_MODULE_COMPONENT(module2, Component3)