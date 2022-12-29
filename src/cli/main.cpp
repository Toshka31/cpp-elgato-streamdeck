#include "ModuleLoader.h"
#include "CallbackEngine.h"

#include <ModuleAPI/IModule.h>
#include <StreamDeckLib/DeviceManager.h>
#include <StreamDeckLib/Transport/TransportFactory.h>
#include <StreamDeckLib/ImageHelper/ImageHelper.h>

#include <boost/program_options.hpp>

#include <iostream>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

int main()
{
    ModuleLoader module_loader("/home/anton/dev/streamdeck/build/src/modules/ALSA");
    module_loader.printModules();

    CallbackEngine engine;
    
	DeviceManager mngr(TransportFactory::createUsbTransport());
	auto streamdecks = mngr.enumerate();
	
	for (auto deck : streamdecks)
	{
		deck->open();
		deck->reset();

		deck->set_brightness(25);

		deck->set_key_callback(std::bind(&CallbackEngine::callback, &engine, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        auto component = module_loader.getModuleComponent("AlsaModule", "Mute");
        component->init(deck);
        engine.setKeyComponent(deck, 0, component);

        // TODO: refactor Base deck class API, cause this code looks ugly
        image::helper::TargetImageParameters image_params = { 
            deck->key_image_format().size.first, 
            deck->key_image_format().size.second, 
            deck->key_image_format().flip.first, 
            deck->key_image_format().flip.second };

		deck->set_key_image(3, image::helper::prepareImageForDeck(
            std::string(SAMPLES_FOLDER) + "/horse_unscaled.jpg", image_params));
		deck->set_key_image(7, image::helper::prepareImageForDeck(
            std::string(SAMPLES_FOLDER) + "/spartak.png", image_params));
	}

    for ( ;; )
    {
        bool all_devices_closed = true;
        for (auto &deck : streamdecks)
        {
            if (deck->is_open())
            {
                all_devices_closed = false;
                break;
            }
        }

        if (all_devices_closed)
        {
            break;
        }

        std::this_thread::sleep_for(100ms);
        std::this_thread::yield();
    }

    return 0;
}
