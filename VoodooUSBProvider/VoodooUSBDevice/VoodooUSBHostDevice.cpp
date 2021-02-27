//
//  VoodooUSBHostDevice.cpp
//  VoodooUSBProvider
//
//  Created by Charlie Jiang on 2/26/21.
//  Copyright © 2021 Charlie Jiang. All rights reserved.
//

#include "VoodooUSBDevice.h"

OSDefineMetaClassAndAbstractStructors(VoodooUSBDevice, USBDevice)

inline UInt16 VoodooUSBDevice::getVendorID()
{
    return USBToHost16(super::getDeviceDescriptor()->idVendor);
}

inline UInt16 VoodooUSBDevice::getProductID()
{
    return USBToHost16(super::getDeviceDescriptor()->idProduct);
}

IOReturn VoodooUSBDevice::getStringDescriptor(UInt8 index, char * buf, int maxLen, UInt16 lang)
{
    memset(buf, 0, maxLen);
    
    const StringDescriptor * desc = super::getStringDescriptor(index);
    
    if (!desc)
    {
        return kIOReturnBadArgument;
    }
    
    if (desc->bLength <= StandardUSB::kDescriptorSize)
    {
        VoodooUSBSafeDeleteNULL(desc);
        return kIOReturnBadArgument;
    }
    
    size_t utf8len = 0;
    utf8_encodestr(reinterpret_cast<const u_int16_t *> (desc->bString), desc->bLength - StandardUSB::kDescriptorSize, reinterpret_cast<u_int8_t *> (buf), &utf8len, maxLen, '/', UTF_LITTLE_ENDIAN);
    
    VoodooUSBSafeDeleteNULL(desc);
    return kIOReturnSuccess;
}

inline UInt16 VoodooUSBDevice::getDeviceRelease()
{
    return USBToHost16(super::getDeviceDescriptor()->bcdDevice);
}

inline IOReturn VoodooUSBDevice::getDeviceStatus(IOService * forClient, USBStatus * status)
{
    UInt16 stat;
    IOReturn result = sendStandardRequestIn(forClient, kDeviceRequestGetStatus, &stat, sizeof(stat));
    *status = stat;
    return result;
}

inline IOReturn VoodooUSBDevice::resetDevice()
{
    sendHCIRequestOut((IOService *) this, HCI_OP_RESET, 0, NULL);
    
    // Setting configuration value 0 (unconfigured) releases all opened interfaces / pipes
    super::setConfiguration(0);
    
    return kIOReturnSuccess;
}

inline UInt8 VoodooUSBDevice::getNumConfigurations()
{
    return super::getDeviceDescriptor()->bNumConfigurations;
}

inline const USBConfigurationDescriptor * VoodooUSBDevice::getFullConfigurationDescriptor(UInt8 configIndex)
{
    return super::getConfigurationDescriptor(configIndex);
}

inline IOReturn VoodooUSBDevice::getConfiguration(IOService * forClient, UInt8 * configNumber)
{
    UInt8 config;
    IOReturn result = sendStandardRequestIn(forClient, kDeviceRequestGetConfiguration, &config, sizeof(config));
    *configNumber = config;
    return result;
}

inline IOReturn VoodooUSBDevice::setConfiguration(IOService * forClient, UInt8 configValue, bool startInterfaceMatching)
{
    return super::setConfiguration(configValue, startInterfaceMatching);
}

bool VoodooUSBDevice::findFirstInterface(VoodooUSBInterface * interface)
{
    VoodooUSBFuncLog("findFirstInterface");
    
    OSIterator * iterator = super::getChildIterator(gIOServicePlane);
    
    if (!iterator)
    {
        return NULL;
    }
    
    IOUSBHostInterface * tempInterface;
    
    while (1)
    {
        if ((tempInterface = OSDynamicCast(IOUSBHostInterface, iterator->getNextObject())))
        {
            setInterface(interface, tempInterface);
            OSSafeReleaseNULL(tempInterface);
            break;
        }
    }
    
    OSSafeReleaseNULL(iterator);

    IOService * result = interface;
    VoodooUSBInfoLog("findFirstInterface() - getInterface() returns %p.\n", result);
    return result;
}

inline bool VoodooUSBDevice::open(IOService * forClient, IOOptionBits options, void *arg)
{
    return super::open(forClient, options, arg);
}

inline void VoodooUSBDevice::close(IOService * forClient, IOOptionBits options)
{
    if (isOpen(forClient))
    {
        super::close(forClient, options);
    }
}

inline UInt8 VoodooUSBDevice::getManufacturerStringIndex()
{
    return super::getDeviceDescriptor()->iManufacturer;
}

inline UInt8 VoodooUSBDevice::getProductStringIndex()
{
    return super::getDeviceDescriptor()->iProduct;
}

inline UInt8 VoodooUSBDevice::getSerialNumberStringIndex()
{
    return super::getDeviceDescriptor()->iSerialNumber;
}

IOReturn VoodooUSBDevice::sendRequest(IOService * forClient, UInt8 bRequest, void * dataBuffer, UInt16 size, UInt8 direction, UInt8 type, UInt8 recipient)
{
    UInt32 bytesTransferred;
    
    StandardUSB::DeviceRequest request =
    {
        .bmRequestType  = static_cast <UInt8> (USBmakebmRequestType(direction, type, recipient)),
        .bRequest       = bRequest,
        .wValue         = 0,
        .wIndex         = 0,
        .wLength        = size
    };
    
    return super::deviceRequest(forClient, request, dataBuffer, bytesTransferred, kUSBHostStandardRequestCompletionTimeout);
}

inline IOReturn VoodooUSBDevice::sendVendorRequestIn(IOService * forClient, UInt8 bRequest, void * dataBuffer, UInt16 size)
{
    return sendRequest(forClient, bRequest, dataBuffer, size, kRequestDirectionIn, kRequestTypeVendor, kRequestRecipientDevice);
}

inline IOReturn VoodooUSBDevice::sendVendorRequestOut(IOService * forClient, UInt8 bRequest, void * dataBuffer, UInt16 size)
{
    return sendRequest(forClient, bRequest, dataBuffer, size, kRequestDirectionOut, kRequestTypeVendor, kRequestRecipientDevice);
}
               
inline IOReturn VoodooUSBDevice::sendStandardRequestIn(IOService * forClient, UInt8 bRequest, void * dataBuffer, UInt16 size)
{
   return sendRequest(forClient, bRequest, dataBuffer, size, kRequestDirectionIn, kRequestTypeStandard, kRequestRecipientDevice);
}

inline IOReturn VoodooUSBDevice::sendStandardRequestOut(IOService * forClient, UInt8 bRequest, void * dataBuffer, UInt16 size)
{
   return sendRequest(forClient, bRequest, dataBuffer, size, kRequestDirectionOut, kRequestTypeStandard, kRequestRecipientDevice);
}

IOReturn VoodooUSBDevice::sendHCIRequest(IOService * forClient, UInt16 opCode, UInt8 paramLen, const void * param, UInt8 direction)
{
    UInt32 bytesTransferred;
    
    HciCommandHdr * command = NULL;
    bzero(command, sizeof(HciCommandHdr));
    command->opCode = opCode;
    command->pLength = paramLen;
    memcpy((void *) command->pData, param, paramLen);
    
    StandardUSB::DeviceRequest request =
    {
        .bmRequestType = makeDeviceRequestbmRequestType((tDeviceRequestDirection) direction, kRequestTypeClass, kRequestRecipientDevice),
        .bRequest = 0,
        .wValue = 0,
        .wIndex = 0,
        .wLength = (UInt16)(HCI_COMMAND_HDR_SIZE + paramLen)
    };
    
    return super::deviceRequest(forClient, request, (void *) command, bytesTransferred);
}

inline IOReturn VoodooUSBDevice::sendHCIRequestIn(IOService * forClient, UInt16 opCode, UInt8 paramLen, const void * param)
{
    return sendHCIRequest(forClient, opCode, paramLen, param, kRequestDirectionIn);
}

inline IOReturn VoodooUSBDevice::sendHCIRequestOut(IOService * forClient, UInt16 opCode, UInt8 paramLen, const void * param)
{
    return sendHCIRequest(forClient, opCode, paramLen, param, kRequestDirectionOut);
}

IOReturn VoodooUSBDevice::sendHCICommand(IOService * forClient, void * command, UInt16 length, UInt8 direction)
{
    UInt32 bytesTransfered;
    
    StandardUSB::DeviceRequest request =
    {
        .bmRequestType = makeDeviceRequestbmRequestType((tDeviceRequestDirection) direction, kRequestTypeClass, kRequestRecipientDevice),
        .bRequest = 0,
        .wValue = 0,
        .wIndex = 0,
        .wLength = length
    };
    
    return super::deviceRequest(forClient, request, command, bytesTransfered, 0);
}

inline IOReturn VoodooUSBDevice::sendHCICommandIn(IOService * forClient, void * command, UInt16 length)
{
    return sendHCICommand(forClient, command, length, kRequestDirectionIn);
}

inline IOReturn VoodooUSBDevice::sendHCICommandOut(IOService * forClient, void * command, UInt16 length)
{
    return sendHCICommand(forClient, command, length, kRequestDirectionOut);
}
