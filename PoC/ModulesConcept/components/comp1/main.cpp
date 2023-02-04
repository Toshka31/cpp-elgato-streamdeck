#include <IModule.h>

DECLARE_MODULE(module1)
DEFINE_MODULE(module1)

class Component1 : public IComponent
{
  DECLARE_MODULE_COMPONENT(module1, Component1)
public:
    std::string name() const override
    {
        return "Component1";
    };
};

REGISTER_MODULE_COMPONENT(module1, Component1)