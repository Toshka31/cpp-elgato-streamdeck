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
	int sz = 72*72;
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

		deck->set_key_image(1, create_image(deck));
		deck->set_key_image(2, create_image(deck));
		deck->set_key_image(5, create_image(deck));
		deck->set_key_image(12, create_image(deck));

		while (deck->is_open())
			std::this_thread::sleep_for(100ms);
	}

    return 0;
}