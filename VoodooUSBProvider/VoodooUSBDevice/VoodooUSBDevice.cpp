//
//  VoodooUSBDevice.cpp
//  VoodooUSBProvider8
//
//  Created by Charlie Jiang on 2/26/21.
//  Copyright © 2021 Charlie Jiang. All rights reserved.
//

#include "VoodooUSBDevice.h"

OSDefineMetaClassAndAbstractStructors(VoodooUSBDevice, USBDevice)

inline UInt16 VoodooUSBDevice::getVendorID()
{
    return super::GetVendorID();
}

inline UInt16 VoodooUSBDevice::getProductID()
{
    return super::GetProductID();
}

inline IOReturn VoodooUSBDevice::getStringDescriptor(UInt8 index, char * buf, int maxLen, UInt16 lang)
{
    return super::GetStringDescriptor(index, buf, maxLen, lang);
}

inline UInt16 VoodooUSBDevice::getDeviceRelease()
{
    return super::GetDeviceRelease();
}

inline IOReturn VoodooUSBDevice::getDeviceStatus(IOService * forClient, USBStatus * status)
{
    return super::GetDeviceStatus(status);
}

inline IOReturn VoodooUSBDevice::resetDevice()
{
    sendHCIRequestOut((IOService *) this, HCI_OP_RESET, 0, NULL);
    return super::ResetDevice();
}

inline UInt8 VoodooUSBDevice::getNumConfigurations()
{
    return super::GetNumConfigurations();
}

inline const USBConfigurationDescriptor* VoodooUSBDevice::getFullConfigurationDescriptor(UInt8 configIndex)
{
    return super::GetFullConfigurationDescriptor(configIndex);
}

inline IOReturn VoodooUSBDevice::getConfiguration(IOService * forClient, UInt8 * configNumber)
{
    return super::GetConfiguration(configNumber);
}

inline IOReturn VoodooUSBDevice::setConfiguration(IOService * forClient, UInt8 configValue, bool startInterfaceMatching)
{
    return super::SetConfiguration(forClient, configValue, startInterfaceMatching);
}

inline UInt8 VoodooUSBDevice::getManufacturerStringIndex()
{
    return super::GetManufacturerStringIndex();
}

inline UInt8 VoodooUSBDevice::getProductStringIndex()
{
    return super::GetProductStringIndex();
}

inline UInt8 VoodooUSBDevice::getSerialNumberStringIndex()
{
    return super::GetSerialNumberStringIndex();
}

bool VoodooUSBDevice::findFirstInterface(VoodooUSBInterface * interface)
{
    VoodooUSBFuncLog("findFirstInterface");
    IOUSBFindInterfaceRequest request =
    {
        .bAlternateSetting  = kIOUSBFindInterfaceDontCare,
        .bInterfaceClass    = kIOUSBFindInterfaceDontCare,
        .bInterfaceSubClass = kIOUSBFindInterfaceDontCare,
        .bInterfaceProtocol = kIOUSBFindInterfaceDontCare
    };
    setInterface(interface, super::FindNextInterface(NULL, &request));
    
    VoodooUSBInfoLog("findFirstInterface() - FindNextInterface() returns %p.\n", interface);
    
    IOService * result = interface;
    VoodooUSBInfoLog("findFirstInterface() - getInterface returns %p.\n", result);
    return result;
}

IOReturn VoodooUSBDevice::sendRequest(IOService * forClient, UInt8 bRequest, void * dataBuffer, UInt16 size, UInt8 direction, UInt8 type, UInt8 recipient)
{
    IOUSBDevRequest request =
    {
        .bmRequestType  = static_cast<UInt8> (USBmakebmRequestType(direction, type, recipient)),
        .bRequest       = bRequest,
        .wValue         = 0,
        .wIndex         = 0,
        .wLength        = size,
        .pData          = dataBuffer
    };
    
    return super::DeviceRequest(&request);
}

inline IOReturn VoodooUSBDevice::sendVendorRequestIn(IOService * forClient, UInt8 bRequest, void * dataBuffer, UInt16 size)
{
    return sendRequest(forClient, bRequest, dataBuffer, size, kIOUSBDeviceRequestDirectionIn, kIOUSBDeviceRequestTypeVendor, kIOUSBDeviceRequestRecipientDevice);
}

inline IOReturn VoodooUSBDevice::sendVendorRequestOut(IOService * forClient, UInt8 bRequest, void * dataBuffer, UInt16 size)
{
    return sendRequest(forClient, bRequest, dataBuffer, size, kIOUSBDeviceRequestDirectionOut, kIOUSBDeviceRequestTypeVendor, kIOUSBDeviceRequestRecipientDevice);
}

inline IOReturn VoodooUSBDevice::sendStandardRequestIn(IOService * forClient, UInt8 bRequest, void * dataBuffer, UInt16 size)
{
    return sendRequest(forClient, bRequest, dataBuffer, size, kIOUSBDeviceRequestDirectionIn, kIOUSBDeviceRequestTypeStandard, kIOUSBDeviceRequestRecipientDevice);
}

inline IOReturn VoodooUSBDevice::sendStandardRequestOut(IOService * forClient, UInt8 bRequest, void * dataBuffer, UInt16 size)
{
    return sendRequest(forClient, bRequest, dataBuffer, size, kIOUSBDeviceRequestDirectionOut, kIOUSBDeviceRequestTypeStandard, kIOUSBDeviceRequestRecipientDevice);
}

IOReturn VoodooUSBDevice::sendHCIRequest(IOService * forClient, UInt16 opCode, UInt8 paramLen, const void * param, UInt8 direction)
{
    HciCommandHdr * command = NULL;
    bzero(command, sizeof(HciCommandHdr));
    command->opCode = opCode;
    command->pLength = paramLen;
    memcpy((void *) command->pData, param, paramLen);
    
    IOUSBDevRequest request =
    {
        .bmRequestType = static_cast<UInt8> (USBmakebmRequestType(direction, kUSBClass, kUSBDevice)),
        .bRequest = 0,
        .wValue = 0,
        .wIndex = 0,
        .wLength = (UInt16)(HCI_COMMAND_HDR_SIZE + paramLen),
        .pData = command
    };
    
    return super::DeviceRequest(&request);
}

inline IOReturn VoodooUSBDevice::sendHCIRequestIn(IOService * forClient, UInt16 opCode, UInt8 paramLen, const void * param)
{
    return sendHCIRequest(forClient, opCode, paramLen, param, kUSBIn);
}

inline IOReturn VoodooUSBDevice::sendHCIRequestOut(IOService * forClient, UInt16 opCode, UInt8 paramLen, const void * param)
{
    return sendHCIRequest(forClient, opCode, paramLen, param, kUSBOut);
}

IOReturn VoodooUSBDevice::sendHCICommand(IOService * forClient, void * command, UInt16 length, UInt8 direction)
{
    IOUSBDevRequest request =
    {
        .bmRequestType = static_cast<UInt8> (USBmakebmRequestType(direction, kUSBClass, kUSBDevice)),
        .bRequest = 0,
        .wValue = 0,
        .wIndex = 0,
        .wLength = length,
        .pData = command
    };
    return super::DeviceRequest(&request);
}

inline IOReturn VoodooUSBDevice::sendHCICommandIn(IOService * forClient, void * command, UInt16 length)
{
    return sendHCICommand(forClient, command, length, kUSBIn);
}

inline IOReturn VoodooUSBDevice::sendHCICommandOut(IOService * forClient, void * command, UInt16 length)
{
    return sendHCICommand(forClient, command, length, kUSBOut);
}
