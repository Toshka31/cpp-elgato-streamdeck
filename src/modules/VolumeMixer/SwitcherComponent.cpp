
#include "module.h"
#include "SwitcherComponent.h"

void MixerSwitchComponent::init(std::shared_ptr<IStreamDeck> deck)
    {
        image::helper::TargetImageParameters image_params = {
            deck->key_image_format().size.first,
            deck->key_image_format().size.second,
            deck->key_image_format().flip.first,
            deck->key_image_format().flip.second };

    }

    std::string MixerSwitchComponent::name() const
    {
        return "Show Mixer";
    }

    void MixerSwitchComponent::tick()
    {

    }

    void MixerSwitchComponent::actionPress(std::shared_ptr<IStreamDeck> deck, ushort key)
    {

    }

    void MixerSwitchComponent::actionRelease(std::shared_ptr<IStreamDeck> deck, ushort key)
    {

    }

    std::vector<unsigned char> MixerSwitchComponent::getImage() const
    {

    }