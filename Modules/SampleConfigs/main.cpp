#include <alsa/asoundlib.h>
#include <alsa/mixer.h>

#include <ModuleAPI/IModule.h>
#include <ModuleAPI/IComponent.h>
#include <ModuleAPI/Module.h>
#include <StreamDeckLib/ImageHelper/ImageHelper.h>

#include <iostream>
#include <memory>

DECLARE_MODULE(SampleConfig)
DEFINE_MODULE(SampleConfig)

class SampleConfigurableComponent : public IComponent
{
    DECLARE_MODULE_COMPONENT(SampleConfig, SampleConfigurableComponent)
        COMPONENT_SETTINGS_START
                ADD_INT_VARIABLE("varint", 0, 0, 10)
                ADD_DOUBLE_VARIABLE("vardbl", 1.0, 0.0, 10.0)
                ADD_STRING_VARIABLE("varstr", "tet", 0)
                ADD_BOOL_VARIABLE("varbool", false)
                ADD_COMBO_VARIABLE("varcombo", 0, {
                    COMBO_VARIANT(0, "variant1")
                    COMBO_VARIANT(1, "variant2")
                })
        COMPONENT_SETTINGS_END

public:
    void init(std::shared_ptr<IDeviceButtonRestricted> deck) override
    {
        m_deck = deck;
        ADD_COMBO_VARIABLE_VARIANT("varcombo", 2, "variant3");

        SET_CONFIG_VALUE("varint", 5);
    }

    std::string name() const override
    {
        return "Sample Configurable";
    }

    void tick() override
    {
        std::string name = "varint";
        std::any value_int = GET_CONFIG_VALUE(name);

        CHANGE_DOUBLE_VARIABLE_RESTRICTIONS("vardouble", 0.0, 1.0);

        CHANGE_INT_VARIABLE_RESTRICTIONS("varint", 0, 5);
    }

    void actionPress() override
    {
    }

    void actionRelease() override
    {
    }

    std::vector<unsigned char> getImage() const override
    {
        return {};
    }

private:
    std::shared_ptr<IDeviceButtonRestricted> m_deck;
};

REGISTER_MODULE_COMPONENT(SampleConfig, SampleConfigurableComponent)