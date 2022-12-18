#pragma once

#include <memory>
#include <vector>

/*
Base connection device, representing an abstract connected device which
can be communicated with by an upper layer high level protocol.
*/
class IDevice
{
public:
    /*
    Opens the device for input/output. This must be called prior to
    sending or receiving any reports.
    .. seealso:: See :func:`~Transport.Device.close` for the
                    corresponding close method.
    */
    virtual void open() = 0;

    /*
    Closes the device for input/output.
    .. seealso:: See :func:`~~Transport.Device.open` for the
                    corresponding open method.
    */
    virtual void close() = 0;

    /*
    Indicates if the physical device object this instance is attached
    to has been opened by the host.
    :rtype: bool
    :return: `True` if the device is open, `False` otherwise.
    */      
    virtual bool is_open() = 0;

    /*
    Indicates if the physical device object this instance is attached
    to is still connected to the host.
    :rtype: bool
    :return: `True` if the device is still connected, `False` otherwise.
    */        
    virtual bool connected() = 0;

    /*
    Retrieves the logical path of the attached device within the
    current system. This can be used to uniquely differentiate one
    device from another.
    :rtype: str
    :return: Logical device path for the attached device.
    */        
    virtual std::string path() = 0;

    /*
    Retrieves the vendor ID value of the attached device.
    :rtype: int
    :return: Vendor ID of the attached device.
    */        
    virtual unsigned short vendor_id() = 0;

    /*
    Retrieves the product ID value of the attached device.
    :rtype: int
    :return: Product ID of the attached device.
    */        
    virtual unsigned short product_id() = 0;

    /*
    Sends a HID Feature report to the open HID device.
    :param enumerable() payload: Enumerate list of bytes to send to the
                                    device, as a feature report. The first
                                    byte of the report should be the Report
                                    ID of the report being sent.
    :rtype: int
    :return: Number of bytes successfully sent to the device.
    */        
    virtual int write_feature(std::vector<unsigned char>) = 0;

    /*
    Reads a HID Feature report from the open HID device.
    :param int report_id: Report ID of the report being read.
    :param int length: Maximum length of the Feature report to read..
    :rtype: list(byte)
    :return: List of bytes containing the read Feature report. The
                first byte of the report will be the Report ID of the
                report that was read.
    */
    virtual std::vector<unsigned char> read_feature(unsigned char report_id, size_t length) = 0;

    /*
    Sends a HID Out report to the open HID device.
    :param enumerable() payload: Enumerate list of bytes to send to the
                                    device, as an Out report. The first
                                    byte of the report should be the Report
                                    ID of the report being sent.
    :rtype: int
    :return: Number of bytes successfully sent to the device.
    */    
    virtual int write(std::vector<unsigned char>) = 0;

    /*
    Performs a blocking read of a HID In report from the open HID device.
    :param int length: Maximum length of the In report to read.
    :rtype: list(byte)
    :return: List of bytes containing the read In report. The first byte
                of the report will be the Report ID of the report that was
                read.
    */
    virtual std::vector<unsigned char> read(size_t length) = 0;
};

/*
Base transport layer, representing an abstract communication back-end which
can be used to discovery attached StreamDeck devices.
*/
class ITransport
{
public:  
    /*
    Enumerates all available devices on the system using the current
    transport back-end.
    :param int vid: USB Vendor ID to filter all devices by, `None` if the
                    device list should not be filtered by vendor.
    :param int pid: USB Product ID to filter all devices by, `None` if the
                    device list should not be filtered by product.
    :rtype: list(Transport.Device)
    :return: List of discovered devices that are available through this
                transport back-end.
    */
    virtual std::vector<std::shared_ptr<IDevice>> enumerate(unsigned short vid, unsigned short pid) = 0;
};