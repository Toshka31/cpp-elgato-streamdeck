#pragma once

#include <boost/config.hpp>
#include <boost/dll/alias.hpp>

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>

#define DECLARE_MODULE(name) \
class BOOST_SYMBOL_VISIBLE Module##name : public IModule { \
 public:\
    static std::shared_ptr<IModule> create();\
    std::string getName() const override;\
    std::shared_ptr<IComponent> createComponent(const std::string& name) override; \
    bool hasComponent(const std::string& name) override;                         \
    std::vector<std::string> getComponentList() const override;                   \
    std::optional<ProvidedProfile> getProvidedProfile() const override;           \
    struct ProfileRegistrator {\
      ProfileRegistrator(ProvidedProfile profile);\
    };\
    struct ComponentRegistrator {\
      ComponentRegistrator(const std::string &name, std::shared_ptr<IComponent>(*fn)());\
    };\
    virtual ~Module##name() = default;\
  private:\
    static std::map<std::string, std::shared_ptr<IComponent>(*)()> components;    \
    static std::optional<ProvidedProfile> provided_profile; \
};\
BOOST_DLL_ALIAS(Module##name::create, create);

#define DEFINE_MODULE(name) \
std::shared_ptr<IModule> Module##name::create() { return std::make_shared<Module##name>(); }\
std::string Module##name::getName() const { return #name; }\
std::shared_ptr<IComponent> Module##name::createComponent(const std::string& name) {        \
    return Module##name::components[name]();                                                \
}                           \
bool Module##name::hasComponent(const std::string& name) {                                  \
    return Module##name::components.count(name);                        \
}                       \
std::vector<std::string> Module##name::getComponentList() const {\
  std::vector<std::string> res;\
  for (const auto &comp : Module##name::components)\
    res.push_back(comp.first);\
  return res;\
}                       \
std::optional<ProvidedProfile> Module##name::getProvidedProfile() const {                   \
    return Module##name::provided_profile;           \
}                           \
Module##name::ProfileRegistrator::ProfileRegistrator(ProvidedProfile profile) {             \
    Module##name::provided_profile = ProvidedProfile(); \
    for (const auto &btn : profile.key_mapping)                                             \
        if (Module##name::components.find(btn.second) != Module##name::components.end())    \
            Module##name::provided_profile.value().key_mapping.insert(btn);\
}\
Module##name::ComponentRegistrator::ComponentRegistrator(const std::string &name, std::shared_ptr<IComponent>(*fn)()) {\
    Module##name::components.insert(std::make_pair(name, fn));\
}\
std::map<std::string, std::shared_ptr<IComponent>(*)()> Module##name::components; \
std::optional<ProvidedProfile> Module##name::provided_profile;

#define SET_MODULE_PROVIDED_PROFILE(module_name, profile) \
struct Profile##module_name : public ProvidedProfile { \
    static Module##module_name::ProfileRegistrator reg; \
};\
Module##module_name::ProfileRegistrator Profile##module_name::reg(ProvidedProfile(profile));

#define DECLARE_MODULE_COMPONENT(module_name, component_name) \
  static Module##module_name::ComponentRegistrator reg; \
  static std::shared_ptr<IComponent> create() { \
    return std::make_shared< component_name >(); \
  }                                                           \
  private:                                                    \
    ComponentParameters m_params;                             \
  public: \
    ComponentParameters getParameters() override              \
        { return m_params; } \
    void setParameters(ComponentParameters::Variables variables) override   \
        {                                                     \
            for (const auto var : variables )                 \
                m_params.setParameter(var.first, var.second); \
        };

#define REGISTER_MODULE_COMPONENT(module_name, component_name) \
  Module##module_name::ComponentRegistrator component_name::reg(#component_name, &component_name::create);

#define ADD_PARAMETER(name, value) m_params.createParameter(name, value)

#define GET_INT_PARAMETER(name) m_params.getParameterValue<int>(name)

// Profile things

#define EVAL0(...) __VA_ARGS__
#define EVAL1(...) EVAL0(EVAL0(EVAL0(__VA_ARGS__)))
#define EVAL2(...) EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL3(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL4(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL(...)  EVAL4(EVAL4(EVAL4(__VA_ARGS__)))

#define MAP_END(...)
#define MAP_OUT

#define MAP_GET_END2() 0, MAP_END
#define MAP_GET_END1(...) MAP_GET_END2
#define MAP_GET_END(...) MAP_GET_END1
#define MAP_NEXT0(test, next , ...) next MAP_OUT
#define MAP_NEXT1(test, next) MAP_NEXT0(test, next, 0)
#define MAP_NEXT(test, next)  MAP_NEXT1(MAP_GET_END test, next)

#define MAP_TUPLES0(f, x, peek, ...) ,f x MAP_NEXT(peek, MAP_TUPLES1)(f, peek, __VA_ARGS__)
#define MAP_TUPLES1(f, x, peek, ...) ,f x MAP_NEXT(peek, MAP_TUPLES0)(f, peek, __VA_ARGS__)
#define MAP_TUPLES_BEGIN(f, x, peek, ...) f x MAP_NEXT(peek, MAP_TUPLES1)(f, peek, __VA_ARGS__)
#define MAP_TUPLES(f, ...) EVAL(MAP_TUPLES_BEGIN(f, __VA_ARGS__, ()()(),  0))

#define DECL_PAIR(type,var)\
  {type, var}

#define CREATE_PROFILE(...) { MAP_TUPLES(DECL_PAIR, __VA_ARGS__) }
#define SET_BUTTON(btn, val) (btn, val)