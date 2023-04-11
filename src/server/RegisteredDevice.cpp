#include "RegisteredDevice.h"
#include "RestrictedDevice.h"

RegisteredDevice::RegisteredDevice(std::shared_ptr<IStreamDeck> deck, std::shared_ptr<ModuleLoader> module_loader)
    : m_streamdeck(deck),
      m_current_profile(loadDeckProfile(deck->get_serial_number(), getDefaultProfileName(deck->get_serial_number()))),
      m_module_loader(module_loader)
{

}

void RegisteredDevice::init()
{
    m_streamdeck->set_key_callback(std::bind(&RegisteredDevice::callback,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2,
                                             std::placeholders::_3));

    m_image_params = {
        m_streamdeck->key_image_format().size.first,
        m_streamdeck->key_image_format().size.second,
        m_streamdeck->key_image_format().flip.first,
        m_streamdeck->key_image_format().flip.second};

    // apply profile
    refresh();
}

void RegisteredDevice::tick()
{
    {
        std::scoped_lock<std::mutex> lock(m_mutex);
        for (auto event: m_events) {
            std::cout << "[" << m_streamdeck->get_serial_number() << "]:" << "\t" << event.key << "\t" << event.value
                      << std::endl;

            auto it = m_key_mapping.find(event.key);
            if (it != m_key_mapping.end()) {
                if (event.value)
                    it->second->actionPress();
                else
                    it->second->actionRelease();
            }
        }
        m_events.clear();
    }

    for (auto &key: m_key_mapping)
        key.second->tick();
}

std::string RegisteredDevice::getID()
{
    return m_streamdeck->id();
}

bool RegisteredDevice::is_device_open()
{
    return m_streamdeck->is_open();
}

void RegisteredDevice::setBrightness(ushort brightness)
{
    m_current_profile.setBrightness(brightness);
    m_streamdeck->set_brightness(brightness);
}

void RegisteredDevice::setButtonImage(ushort key, std::vector<uint8_t> &image)
{
    auto saved_path = saveButtonImageForDeck(m_streamdeck->get_serial_number(), image);
    m_current_profile.setButtonImage(key, saved_path);
    updateButtonImage(key);
}

void RegisteredDevice::setButtonLabel(ushort key, const std::string &label)
{
    m_current_profile.setButtonLabel(key, label);
    updateButtonImage(key);
}

void RegisteredDevice::setButtonComponent(ushort key, const std::string &module, const std::string &component)
{
    if (m_module_loader->hasModuleComponent(module, component)) {
        m_current_profile.setButtonComponent(key, module, component);
        updateButtonComponent(key);
        updateButtonImage(key);
    }
}

void RegisteredDevice::setButtonParameters(ushort key, const std::string &parameters)
{
    const auto it = m_key_mapping.find(key);
    if (it == m_key_mapping.end())
        return;

    ComponentParameters::Variables vars = ComponentParameters::deserialiseVariables(parameters);

    it->second->setParameters(vars);
}

std::string RegisteredDevice::getButtonParameters(ushort key) const
{
    const auto it = m_key_mapping.find(key);
    if (it == m_key_mapping.end())
        return {};
    auto param = it->second->getParameters();
    return ComponentParameters::serialiseParameters(param);
}

std::string RegisteredDevice::getCurrentProfileName() const
{
    return m_current_profile.getName();
}

void RegisteredDevice::setProfile(const std::string &profile_name)
{
    m_current_profile.save();
    m_current_profile = loadDeckProfile(m_streamdeck->get_serial_number(), profile_name);
    refresh();
}

std::vector<std::string> RegisteredDevice::getProfiles()
{
    return getDeckProfiles(m_streamdeck->get_serial_number());
}

std::string RegisteredDevice::getCurrentPageName() const
{
    return m_current_profile.getCurrentPageName();
}

std::vector<std::string> RegisteredDevice::getPages()
{
    return m_current_profile.getPages();
}

image::helper::TargetImageParameters RegisteredDevice::getImageFormat()
{
    return {
        m_streamdeck->key_image_format().size.first,
        m_streamdeck->key_image_format().size.second,
        m_streamdeck->key_image_format().flip.first,
        m_streamdeck->key_image_format().flip.second};
}

void RegisteredDevice::callback(std::shared_ptr<IStreamDeck> deck, ushort key, bool val)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_events.emplace_back(key, val);
}

void RegisteredDevice::refresh()
{
    m_streamdeck->reset();
    m_key_mapping.clear();

    unsigned short key_count = m_streamdeck->key_count();
    for (auto key = 0; key < key_count; ++key) {
        updateButtonComponent(key);
        updateButtonImage(key);
    }
}

void RegisteredDevice::updateButtonImage(ushort key)
{
    auto opt_key_profile = m_current_profile.getCurrentKeyProfile(key);
    if (opt_key_profile.has_value()) {
        auto key_profile = opt_key_profile.value();
        std::vector<unsigned char> image_data;
        if (!key_profile.m_module_name.empty() && !key_profile.m_component_name.empty())
            image_data = m_key_mapping[key]->getImage();

        if (!key_profile.m_custom_image.empty())
            image_data = image::helper::loadRawImage(key_profile.m_custom_image);

        if (!key_profile.m_custom_label.empty()) {
            if (image_data.empty())
                image_data = image::helper::createEmptyImage(m_image_params);
            image_data = image::helper::applyLabelOnImage(image_data, key_profile.m_custom_label);
        }

        if (!image_data.empty()) {
            image_data = image::helper::prepareImageForDeck(image_data, m_image_params);
            m_streamdeck->set_key_image(key, image_data);
        }
    }
    else
        m_streamdeck->set_key_image(key, {});
}

void RegisteredDevice::updateButtonComponent(ushort key)
{
    if (auto it = m_key_mapping.find(key) != m_key_mapping.end())
        m_key_mapping.erase(it);

    auto opt_key_profile = m_current_profile.getCurrentKeyProfile(key);
    if (!opt_key_profile.has_value())
        return;

    auto key_profile = opt_key_profile.value();

    if (key_profile.m_module_name.empty() || key_profile.m_component_name.empty())
        return;

    auto comp = m_module_loader->getModuleComponent(key_profile.m_module_name, key_profile.m_component_name);
    if (!comp)
        return;
    comp->init(std::make_shared<RestrictedDevice>(key, shared_from_this()));

    auto profile = m_module_loader->getModuleProfile(key_profile.m_module_name);
    if (profile.has_value())
        addProfileFromModule(key_profile.m_module_name, profile.value());

    m_key_mapping[key] = comp;
}

void RegisteredDevice::addProfileFromModule(const std::string &module, const ProvidedProfile &provided_profile)
{
    auto profile = createNewProfile(m_streamdeck->get_serial_number(), module);

    for (const auto &btn: provided_profile.key_mapping)
        profile.setButtonComponent(btn.first, module, btn.second);

    profile.save();
}
