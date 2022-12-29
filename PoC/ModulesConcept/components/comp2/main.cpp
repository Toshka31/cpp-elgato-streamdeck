#include <IModule.h>

DECLARE_MODULE(module2)

class Component2 : public IComponent
{
public:
    std::string name() const override
    {
        return "Component2";
    };

    static std::shared_ptr<IComponent> create()
    {
        return std::make_shared<Component2>();
    }
private:
    static ComponentRegistrator reg;
};

ComponentRegistrator Component2::reg("Component2", &Component2::create);