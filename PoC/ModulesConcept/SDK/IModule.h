#include <boost/config.hpp>
#include <boost/dll/alias.hpp> 

#include <string>
#include <vector>
#include <memory>
#include <functional>

class IComponent
{
public:
    virtual std::string name() const = 0;
};

struct ComponentRegistrator
{
    ComponentRegistrator(const std::string &name, std::shared_ptr<IComponent>(*)());
};

class ModuleCore 
{
public:
    virtual std::string getName() const;

    virtual std::vector<std::string> getComponentList();

    virtual std::shared_ptr<IComponent> createComponent(const std::string& name);

    virtual ~ModuleCore() {}
};

#define DECLARE_MODULE(name) class BOOST_SYMBOL_VISIBLE Module##name : public ModuleCore { \
    public: \
        std::string getName() const override { return #name; } \
        static std::shared_ptr<ModuleCore> create() {return std::make_shared<Module##name>(); } \
        virtual ~Module##name() {} \
    }; \
    BOOST_DLL_ALIAS(Module##name::create, create);