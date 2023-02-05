#pragma once

#include "RegisteredDevice.h"

RegisteredDevice::RegisteredDevice(std::shared_ptr<IStreamDeck> deck, std::shared_ptr<ModuleLoader> module_loader)
      : m_streamdeck(deck),
        m_current_profile(loadDeckProfile(deck->get_serial_number())),
        m_module_loader(module_loader)
{
  m_streamdeck->set_key_callback(std::bind(&RegisteredDevice::callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

  m_image_params = {
      m_streamdeck->key_image_format().size.first,
      m_streamdeck->key_image_format().size.second,
      m_streamdeck->key_image_format().flip.first,
      m_streamdeck->key_image_format().flip.second };

  // apply profile
  unsigned short key_count = m_streamdeck->key_count();
  for (auto key = 0; key < key_count; ++key)
  {
    updateButton(key);
  }
}

void RegisteredDevice::tick()
{
  {
    std::scoped_lock<std::mutex> lock(m_mutex);
    for (auto event : m_events)
    {
      std::cout << "["<< m_streamdeck->get_serial_number() << "]:" << "\t" << event.key << "\t" << event.value << std::endl;

      auto it = m_key_mapping.find(event.key);
      if (it != m_key_mapping.end())
      {
        if (event.value)
          it->second->actionPress(m_streamdeck, event.key);
        else
          it->second->actionRelease(m_streamdeck, event.key);
      }
    }
    m_events.clear();
  }

  for (auto &key : m_key_mapping)
    key.second->tick();
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
  updateButton(key);
}

void RegisteredDevice::setButtonLabel(ushort key, const std::string &label)
{
  m_current_profile.setButtonLabel(key, label);
  updateButton(key);
}

void RegisteredDevice::setButtonComponent(ushort key, const std::string &module, const std::string &component)
{
  m_current_profile.setButtonComponent(key, module, component);
  auto comp = m_module_loader->getModuleComponent(module, component);
  m_key_mapping[key] = comp;
  updateButton(key);
}

std::string RegisteredDevice::getCurrentProfileName() const
{
  return m_current_profile.getName();
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

void RegisteredDevice::callback(std::shared_ptr<IStreamDeck> deck, ushort key, bool val)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  m_events.emplace_back(key, val);
}

void RegisteredDevice::updateButton(ushort key)
{
  auto key_profile = m_current_profile.getCurrentKeyProfile(key);

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
