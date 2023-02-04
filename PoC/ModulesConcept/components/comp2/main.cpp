#include "module.h"

class Component2 : public IComponent
{
  DECLARE_MODULE_COMPONENT(module2, Component2)
public:
    std::string name() const override
    {
        return "Component2";
    };
};

REGISTER_MODULE_COMPONENT(module2, Component2)
