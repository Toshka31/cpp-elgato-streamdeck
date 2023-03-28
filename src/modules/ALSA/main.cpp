#include <alsa/asoundlib.h>
#include <alsa/mixer.h>

#include <ModuleAPI/IModule.h>
#include <ModuleAPI/IComponent.h>
#include <ModuleAPI/Module.h>
#include <StreamDeckLib/Device/IStreamDeck.h>
#include <StreamDeckLib/ImageHelper/ImageHelper.h>

#include <iostream>
#include <memory>

DECLARE_MODULE(AlsaModule)
DEFINE_MODULE(AlsaModule)

#include "image_mute.h"
#include "image_unmute.h"

class MuteComponent : public IComponent
{
  DECLARE_MODULE_COMPONENT(AlsaModule, MuteComponent)
public:
    void init(std::shared_ptr<IDeviceButtonRestricted> deck) override
    {
        m_deck = deck;

        image::helper::TargetImageParameters image_params = m_deck->getImageFormat();

        m_img_mute = image::helper::prepareImageForDeck(IMAGE_MUTE, image_params);
        m_img_unmute = image::helper::prepareImageForDeck(IMAGE_UNMUTE, image_params);
    }

    std::string name() const override
    {
        return "Mute";
    }

    void tick() override
    {

    }

    void actionPress() override
    {
        if (m_prev_value)
            m_deck->updateButtonImage();
        else
            m_deck->updateButtonImage();
        SetAlsaMasterVolume(m_prev_value);
    }

    void actionRelease() override
    {

    }

    std::vector<unsigned char> getImage() const override
    {
        return IMAGE_UNMUTE;
    }
private:
    long m_prev_value = 0;

    std::shared_ptr<IDeviceButtonRestricted> m_deck;

    std::vector<unsigned char> m_img_mute;
    std::vector<unsigned char> m_img_unmute;

    void SetAlsaMasterVolume(long volume)
    {
        long min, max;
        snd_mixer_t *handle;
        snd_mixer_selem_id_t *sid;
        const char *card = "default";
        const char *selem_name = "Master";

        snd_mixer_open(&handle, 0);
        snd_mixer_attach(handle, card);
        snd_mixer_selem_register(handle, NULL, NULL);
        snd_mixer_load(handle);

        snd_mixer_selem_id_alloca(&sid);
        snd_mixer_selem_id_set_index(sid, 0);
        snd_mixer_selem_id_set_name(sid, selem_name);
        snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

        snd_mixer_selem_get_playback_volume_range(elem, &min, &max);

        snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_MONO, &m_prev_value);
        m_prev_value /= (max / 100);

        snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

        snd_mixer_close(handle);
    }
};
REGISTER_MODULE_COMPONENT(AlsaModule, MuteComponent)


