#pragma once

// USB Vendor IDs for known StreamDeck devices..
enum USBVendorIDs : unsigned short {
    USB_VID_ELGATO = 0x0fd9
};

// USB Product IDs for known StreamDeck devices..
enum USBProductIDs : unsigned short {
    USB_PID_STREAMDECK_ORIGINAL = 0x0060,
    USB_PID_STREAMDECK_ORIGINAL_V2 = 0x006d,
    USB_PID_STREAMDECK_MINI = 0x0063,
    USB_PID_STREAMDECK_XL = 0x006c,
    USB_PID_STREAMDECK_XL_V2 = 0x008f,
    USB_PID_STREAMDECK_MK2 = 0x0080,
    USB_PID_STREAMDECK_PEDAL = 0x0086,
    USB_PID_STREAMDECK_MINI_MK2 = 0x0090
};