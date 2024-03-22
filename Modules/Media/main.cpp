#include <alsa/asoundlib.h>
#include <alsa/mixer.h>

#include <ModuleAPI/IModule.h>
#include <ModuleAPI/IComponent.h>
#include <ModuleAPI/Module.h>
#include <StreamDeckLib/ImageHelper/ImageHelper.h>

#include <iostream>
#include <memory>

#include "images/image_play.h"
#include "images/image_pause.h"
#include "images/image_next.h"
#include "images/image_next_gray.h"
#include "images/image_prev.h"
#include "images/image_prev_gray.h"

#include "GDbusMediaWrapper.h"

DECLARE_MODULE(MediaModule)

DEFINE_MODULE(MediaModule)

class MediaPlayComponent : public IComponent {
    DECLARE_MODULE_COMPONENT(MediaModule, MediaPlayComponent)
    COMPONENT_SETTINGS_EMPTY

public:
    void init(std::shared_ptr<IDeviceButtonRestricted> deck) override {
        m_deck = deck;
    }

    std::string name() const override {
        return "Play/Pause";
    }

    void tick() override {
        m_deck->updateButtonImage();
    }

    void actionPress() override {
        GDbusMediaWrapper::playPause();
    }

    void actionRelease() override {

    }

    std::vector<unsigned char> getImage() const override {
        if (GDbusMediaWrapper::isPlaying())
            return IMAGE_PAUSE;
        else
            return IMAGE_PLAY;
    }

private:
    std::shared_ptr<IDeviceButtonRestricted> m_deck;
};

REGISTER_MODULE_COMPONENT(MediaModule, MediaPlayComponent)

class MediaNextComponent : public IComponent {
    DECLARE_MODULE_COMPONENT(MediaModule, MediaNextComponent)
    COMPONENT_SETTINGS_EMPTY

public:
    void init(std::shared_ptr<IDeviceButtonRestricted> deck) override {
        m_deck = deck;
    }

    std::string name() const override {
        return "Next";
    }

    void tick() override {
        m_deck->updateButtonImage();
    }

    void actionPress() override {
        if (GDbusMediaWrapper::hasNext())
            GDbusMediaWrapper::next();
    }

    void actionRelease() override {

    }

    std::vector<unsigned char> getImage() const override {
        if (GDbusMediaWrapper::hasNext())
            return IMAGE_NEXT;
        else
            return IMAGE_NEXT_GRAY;
    }

private:
    std::shared_ptr<IDeviceButtonRestricted> m_deck;
};

REGISTER_MODULE_COMPONENT(MediaModule, MediaNextComponent)

class MediaPrevComponent : public IComponent {
    DECLARE_MODULE_COMPONENT(MediaModule, MediaPrevComponent)
    COMPONENT_SETTINGS_EMPTY

public:
    void init(std::shared_ptr<IDeviceButtonRestricted> deck) override {
        m_deck = deck;
    }

    std::string name() const override {
        return "Previous";
    }

    void tick() override {
        m_deck->updateButtonImage();
    }

    void actionPress() override {
        if (GDbusMediaWrapper::hasPrevious())
            GDbusMediaWrapper::previous();
    }

    void actionRelease() override {

    }

    std::vector<unsigned char> getImage() const override {
        if (GDbusMediaWrapper::hasPrevious())
            return IMAGE_PREV;
        else
            return IMAGE_PREV_GRAY;
    }

private:
    std::shared_ptr<IDeviceButtonRestricted> m_deck;
};

REGISTER_MODULE_COMPONENT(MediaModule, MediaPrevComponent)