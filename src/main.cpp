#include "StreamDeck/DeviceManager.h"
#include "StreamDeck/Transport/UsbTransport.h"

#include <jpeglib.h>

#include <iostream>

using namespace std::chrono_literals;

void callback(std::shared_ptr<BaseStreamDeck> deck, ushort key, bool val)
{
	std::cout << "Key " << key << " state " << val << std::endl;
}

std::vector<unsigned char> create_image(std::shared_ptr<BaseStreamDeck> deck)
{
	auto size = deck->key_image_format().size;

}

int main()
{
	std::shared_ptr<ITransport> transport(new UsbTransport());
	DeviceManager mngr(transport);
	auto streamdecks = mngr.enumerate();
	
	for (auto deck : streamdecks)
	{
		deck->open();
		deck->reset();

		deck->set_brightness(25);

		deck->set_key_callback(&callback);

create_image(deck);

		//deck->set_key_image(0, {});

		while (deck->is_open())
			std::this_thread::sleep_for(100ms);
	}

    return 0;
}