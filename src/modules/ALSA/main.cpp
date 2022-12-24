#include <alsa/asoundlib.h>
#include <alsa/mixer.h>

#include <boost/config.hpp> 
#include <boost/dll/alias.hpp> 
#include <ModuleAPI/IModule.h>

#include <iostream>
#include <memory>

class MuteComponent : public IComponent
{
public:
    std::string name() const override
    {
        return "AlsaModule:VolumeUp";
    }

    void actionPress() override
    {
        SetAlsaMasterVolume(0);
    }

    void actionRelease() override
    {
        SetAlsaMasterVolume(50);
    }
private:
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
        snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

        snd_mixer_close(handle);
    }
};

class AlsaModule : public IModule 
{
public:
    AlsaModule() = default;

    std::string name() const override
    {
        return "AlsaModule";
    }

    std::vector<std::shared_ptr<IComponent>> getComponents() const override
    {
        std::vector<std::shared_ptr<IComponent>> components;
        components.push_back(std::make_shared<MuteComponent>());
        return components;
    }

    MAKE_MODULE_FACTORY(AlsaModule)
};

BOOST_DLL_ALIAS(
    AlsaModule::create,
    create
)


