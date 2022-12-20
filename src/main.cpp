#include "StreamDeck/DeviceManager.h"
#include "StreamDeck/Devices/BaseStreamDeck.h"
#include "StreamDeck/Devices/StreamDeckOriginalV2.h"
#include "StreamDeck/Transport/UsbTransport.h"
#include "StreamDeck/Devices/DeviceRegistrator.h"

#include <jpeglib.h>

#include <iostream>


void callback(std::shared_ptr<BaseStreamDeck> deck, ushort key, bool val)
{
	std::cout << "Key " << key << " state " << val << std::endl;
}

std::vector<unsigned char> create_image(std::shared_ptr<BaseStreamDeck> deck)
{
	auto size = deck->key_image_format().size;
    int sz = 72 * 72;
	std::vector<unsigned char> image;
	image.reserve(sz);
	for (int i = 0; i < sz; ++i)
	{
		image.push_back(0xFF);
		image.push_back(0xFF);
		image.push_back(0xFF);
	}

	return image;
}

void registerKnownProducts()
{
    StreamDeckRegistrator<StreamDeckOriginalV2>::insertDeckProduct(USB_PID_STREAMDECK_ORIGINAL_V2);
}

int main()
{
    using namespace std::chrono_literals;

    registerKnownProducts();

	std::shared_ptr<ITransport> transport(new UsbTransport());
	DeviceManager mngr(transport);
	auto streamdecks = mngr.enumerate();
	
    for (auto &deck : streamdecks)
	{
		deck->open();
		deck->reset();

		deck->set_brightness(25);

		deck->set_key_callback(&callback);

		deck->set_key_image(1, create_image(deck));
		deck->set_key_image(2, create_image(deck));
		deck->set_key_image(5, create_image(deck));
		deck->set_key_image(12, create_image(deck));
	}

    for ( ;; )
    {
        const auto have_opened_decks = std::any_of(begin(streamdecks), end(streamdecks),
                                                   [](const auto &deck) { return deck->is_open(); });

        if (!have_opened_decks)
        {
            break;
        }

        std::this_thread::sleep_for(100ms);
        std::this_thread::yield();
    }

    return 0;
}
