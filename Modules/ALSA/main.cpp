#include <alsa/asoundlib.h>
#include <alsa/mixer.h>

#include <ModuleAPI/IModule.h>
#include <ModuleAPI/IComponent.h>
#include <ModuleAPI/Module.h>
#include <StreamDeckLib/ImageHelper/ImageHelper.h>

#include <iostream>
#include <memory>

DECLARE_MODULE(AlsaModule)
DEFINE_MODULE(AlsaModule)

#define ALSA_VOLUME_UP_STEP     (5)
#define ALSA_VOLUME_DOWN_STEP   (5)

#include "image_bytes.h"


class VolumeMuteComponent : public IComponent
{
    DECLARE_MODULE_COMPONENT(AlsaModule, VolumeMuteComponent)
        COMPONENT_SETTINGS_EMPTY

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
        m_deck->updateButtonImage();
        SetAlsaMasterVolume(m_prev_value);
    }

    void actionRelease() override
    {

    }

    std::vector<unsigned char> getImage() const override
    {
        return m_prev_value ? m_img_mute : m_img_unmute;
    }

private:
    long m_prev_value = 0;

    std::shared_ptr<IDeviceButtonRestricted> m_deck;

    std::vector<unsigned char> m_img_mute;
    std::vector<unsigned char> m_img_unmute;

    void SetAlsaMasterVolume(long volume)
    {
        long min{}, max{};
        snd_mixer_t *handle = nullptr;
        snd_mixer_selem_id_t *sid = nullptr;
        const char *card = "default";
        const char *selem_name = "Master";

        snd_mixer_open(&handle, 0);
        snd_mixer_attach(handle, card);
        snd_mixer_selem_register(handle, NULL, NULL);
        snd_mixer_load(handle);

        snd_mixer_selem_id_alloca(&sid);
        snd_mixer_selem_id_set_index(sid, 0);
        snd_mixer_selem_id_set_name(sid, selem_name);
        snd_mixer_elem_t *elem = snd_mixer_find_selem(handle, sid);

        snd_mixer_selem_get_playback_volume_range(elem, &min, &max);

        snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_MONO, &m_prev_value);
        m_prev_value /= (max / 100);

        snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);
        snd_mixer_close(handle);
    }
};

REGISTER_MODULE_COMPONENT(AlsaModule, VolumeMuteComponent)

class VolumeDownComponent : public IComponent
{
    DECLARE_MODULE_COMPONENT(AlsaModule, VolumeDownComponent)
        COMPONENT_SETTINGS_EMPTY

public:
    void init(std::shared_ptr<IDeviceButtonRestricted> deck) override
    {
        image::helper::TargetImageParameters image_params = deck->getImageFormat();
        m_image = image::helper::prepareImageForDeck(IMAGE_VOLUME_DOWN, image_params);
    }

    std::string name() const override
    {
        return "VolumeDown";
    }

    void tick() override
    {

    }

    void actionPress() override
    {
        SetVolumeDown(ALSA_VOLUME_DOWN_STEP);
    }

    void actionRelease() override
    {

    }

    std::vector<unsigned char> getImage() const override
    {
        return m_image;
    }

private:
    std::vector<unsigned char> m_image;

    void SetVolumeDown(long step)
    {
        long min{}, max{};
        snd_mixer_t *handle = nullptr;
        snd_mixer_selem_id_t *sid = nullptr;
        const char *card = "default";
        const char *selem_name = "Master";

        snd_mixer_open(&handle, 0);
        snd_mixer_attach(handle, card);
        snd_mixer_selem_register(handle, NULL, NULL);
        snd_mixer_load(handle);

        snd_mixer_selem_id_alloca(&sid);
        snd_mixer_selem_id_set_index(sid, 0);
        snd_mixer_selem_id_set_name(sid, selem_name);
        snd_mixer_elem_t *elem = snd_mixer_find_selem(handle, sid);

        snd_mixer_selem_get_playback_volume_range(elem, &min, &max);

        long curr_volume {};
        snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_MONO, &curr_volume);
        if (curr_volume > min)
        {   const long step_value = (max - min) * step / 100;
            curr_volume = std::max(min, curr_volume - step_value);
            snd_mixer_selem_set_playback_volume_all(elem, curr_volume);
        }
        snd_mixer_close(handle);
    }
};

REGISTER_MODULE_COMPONENT(AlsaModule, VolumeDownComponent)


class VolumeUpComponent : public IComponent
{
    DECLARE_MODULE_COMPONENT(AlsaModule, VolumeUpComponent)
        COMPONENT_SETTINGS_EMPTY

public:
    void init(std::shared_ptr<IDeviceButtonRestricted> deck) override
    {
        image::helper::TargetImageParameters image_params = deck->getImageFormat();
        m_image = image::helper::prepareImageForDeck(IMAGE_VOLUME_UP, image_params);
    }

    std::string name() const override
    {
        return "VolumeUp";
    }

    void tick() override
    {

    }

    void actionPress() override
    {
        SetVolumeUp(ALSA_VOLUME_UP_STEP);
    }

    void actionRelease() override
    {

    }

    std::vector<unsigned char> getImage() const override
    {
        return m_image;
    }

private:
    std::vector<unsigned char> m_image;

    void SetVolumeUp(long step)
    {
        long min{}, max{};
        snd_mixer_t *handle = nullptr;
        snd_mixer_selem_id_t *sid = nullptr;
        const char *card = "default";
        const char *selem_name = "Master";

        snd_mixer_open(&handle, 0);
        snd_mixer_attach(handle, card);
        snd_mixer_selem_register(handle, NULL, NULL);
        snd_mixer_load(handle);

        snd_mixer_selem_id_alloca(&sid);
        snd_mixer_selem_id_set_index(sid, 0);
        snd_mixer_selem_id_set_name(sid, selem_name);
        snd_mixer_elem_t *elem = snd_mixer_find_selem(handle, sid);

        snd_mixer_selem_get_playback_volume_range(elem, &min, &max);

        long curr_volume {};
        snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_MONO, &curr_volume);
        if (curr_volume < max)
        {   const long step_value = (max - min) * step / 100;
            curr_volume = std::max(min, curr_volume + step_value);
            snd_mixer_selem_set_playback_volume_all(elem, curr_volume);
        }
        snd_mixer_close(handle);
    }
};

REGISTER_MODULE_COMPONENT(AlsaModule, VolumeUpComponent)
