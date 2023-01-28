# C++ Elgato Stream Deck Software
Open source crossplatform software, written on C++, for [Elgato Stream Deck](https://www.elgato.com/en/gaming/stream-deck) devices. Includes base library to control device, service (or daemon) to make some actions, command line and GUI applications to control settings.

Development inspired by [python-elgato-streamdeck](https://github.com/abcminiuser/python-elgato-streamdeck) library

## Status and Plans
> Currently checked only for StreamDeck Original V2 on Manjaro Linux

- ### StreamDeck Library
  - [x] Work fine with stream deck original v2 
  - [x] Make more helper functions for ImageHelper like a text on image 
  - [ ] Maybe make image helpers as separate library
  - [ ] Make error handling
  - [ ] Make logging
  - [ ] Make support for other streamdecks
- ### Modules SDK
  - [x] Base SDK for modules development
  - [ ] Make better soulution for SDK
  - [ ] Allow Module(Component) to take full control (tick and press events) for all buttons
- ### Server side
  - [x] Engine with event loop
  - [x] Base RPC server-side implementation
  - [x] Make profile support (save and load)
  - [x] Make pages support
  - [x] Placing label on button (also saved to profile)
  - [ ] More customisation for label (color, alignment, size)
  - [ ] Make header with constants
  - [ ] Make error handling
  - [ ] Make logging
- ### Command Line Tool
  - [x] Base argument parsing
  - [x] RPC
  - [ ] Make better arguments parsing and error handling
- ### GUI Tool
  - [ ] Make first version :)
- ### Modules
  - [x] Test module for volume control via ALSA
  - [ ] Make base module (brightness control, page control, profile control)

## Dependencies
- boost
- libusb
- rpclib
- libjpeg
- libpng
- openSSL
- openCV

## Build 
> TODO
## Liscence
> TODO