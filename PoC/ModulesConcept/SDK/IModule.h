#include <boost/config.hpp>
#include <boost/dll/alias.hpp> 

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>

class IComponent
{
public:
    virtual std::string name() const = 0;
};

class IModule
{
 public:
  virtual std::string getName() const = 0;
  virtual std::shared_ptr<IComponent> createComponent(const std::string& name) = 0;
  virtual std::vector<std::string> getComponentList() const = 0;
};

#define DECLARE_MODULE(name) \
class BOOST_SYMBOL_VISIBLE Module##name : public IModule { \
  public:  \
    static std::shared_ptr<IModule> create() { return std::make_shared<Module##name>(); } \
    std::string getName() const override { return #name; } \
    std::shared_ptr<IComponent> createComponent(const std::string& name) override { return Module##name::components[name](); } \
    std::vector<std::string> getComponentList() const override { \
      std::vector<std::string> res; \
      for (const auto &comp : Module##name::components) \
        res.push_back(comp.first); \
      return res; \
    } \
    struct ComponentRegistrator { \
      ComponentRegistrator(const std::string &name, std::shared_ptr<IComponent>(*fn)()) { \
        Module##name::components.insert(std::make_pair(name, fn)); \
      } \
    }; \
    virtual ~Module##name() = default; \
  private: \
    static std::map<std::string, std::shared_ptr<IComponent>(*)()> components; \
}; \
BOOST_DLL_ALIAS(Module##name::create, create);             \
std::map<std::string, std::shared_ptr<IComponent>(*)()> Module##name::components;

#define DECLARE_MODULE_COMPONENT(module_name, component_name) \
  static Module##module_name::ComponentRegistrator reg; \
  static std::shared_ptr<IComponent> create() { \
    return std::make_shared< component_name >(); \
  }

#define REGISTER_MODULE_COMPONENT(module_name, component_name) \
  Module##module_name::ComponentRegistrator component_name::reg(#component_name, &component_name::create);