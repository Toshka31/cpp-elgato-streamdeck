#include "Engine.h"

#include <StreamDeckLib/DeviceManager.h>
#include <StreamDeckLib/Transport/TransportFactory.h>
#include <StreamDeckLib/ImageHelper/ImageHelper.h>

#include <boost/program_options.hpp>

#include <iostream>

int main()
{
    Engine engine;

    engine.start();

    return 0;
}
