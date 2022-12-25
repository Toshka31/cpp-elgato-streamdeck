#include <ModuleAPI/IModule.h>
#include <StreamDeckLib/Device/IStreamDeck.h>
#include <StreamDeckLib/ImageHelper/ImageHelper.h>

#include <map>
#include <memory>

class CallbackEngine
{
public:
    void setKeyComponent(std::shared_ptr<IStreamDeck> deck, ushort key, std::shared_ptr<IComponent> component)
    {
        image::helper::TargetImageParameters image_params = { 
            deck->key_image_format().size.first, 
            deck->key_image_format().size.second, 
            deck->key_image_format().flip.first, 
            deck->key_image_format().flip.second };
        image::helper::EImageFormat format;
        auto image = component->getImage(format);
        deck->set_key_image(key, image::helper::prepareImageForDeck(image, format, image_params));
        m_key_mapping[key] = component;
    }

    void callback(std::shared_ptr<IStreamDeck> deck, ushort key, bool val)
    {
        auto it = m_key_mapping.find(key);
        if (it != m_key_mapping.end())
        {
            if (val)
                it->second->actionPress(deck, key);
            else
                it->second->actionRelease(deck, key);
        }
    }
private:
    std::map<ushort, std::shared_ptr<IComponent>> m_key_mapping;
};