#include <IModule.h>

DECLARE_MODULE(module1)

class Component1 : public IComponent
{
public:
    std::string name() const override
    {
        return "Component1";
    };

    static std::shared_ptr<IComponent> create()
    {
        return std::make_shared<Component1>();
    }
private:
    static ComponentRegistrator reg;
};

ComponentRegistrator Component1::reg("Component1", &Component1::create);