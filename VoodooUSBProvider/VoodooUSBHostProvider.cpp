//
//  VoodooUSBHostProvider.cpp
//  VoodooUSBProvider
//
//  Copyright © 2021 cjiang. All rights reserved.
//
//-----------------------------------------------------------------------------
/*
 *  Released under "The GNU General Public License (GPL-2.0)"
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include "VoodooUSBProvider.h"

VoodooUSBDevice::VoodooUSBDevice()
{
    m_pDevice = NULL;
}

VoodooUSBDevice::~VoodooUSBDevice()
{
    OSSafeReleaseNULL(m_pDevice);
}

void VoodooUSBDevice::setDevice(IOService * provider)
{
    OSSafeReleaseNULL(m_pDevice);
    
    m_pDevice = OSDynamicCast(IOUSBHostDevice, provider);
    
    if (m_pDevice)
    {
        m_pDevice->retain();
    }
}

UInt16 VoodooUSBDevice::getVendorID()
{
    return USBToHost16(m_pDevice->getDeviceDescriptor()->idVendor);
}

UInt16 VoodooUSBDevice::getProductID()
{
    return USBToHost16(m_pDevice->getDeviceDescriptor()->idProduct);
}

OSObject * VoodooUSBDevice::getProperty(const char * name)
{
    return m_pDevice->getProperty(name);
}

void VoodooUSBDevice::setProperty(const char * name, bool value)
{
    m_pDevice->setProperty(name, value);
}

void VoodooUSBDevice::removeProperty(const char * name)
{
    m_pDevice->removeProperty(name);
}

IOReturn VoodooUSBDevice::getStringDescriptor(UInt8 index, char * buf, int maxLen, UInt16 lang)
{
    memset(buf, 0, maxLen);
    
    const StringDescriptor * desc = m_pDevice->getStringDescriptor(index);
    
    if (!desc)
    {
        return kIOReturnBadArgument;
    }
    
    if (desc->bLength <= StandardUSB::kDescriptorSize)
    {
        SafeDeleteNULL(desc);
        return kIOReturnBadArgument;
    }
    
    size_t utf8len = 0;
    utf8_encodestr(reinterpret_cast<const u_int16_t*> (desc->bString), desc->bLength - StandardUSB::kDescriptorSize, reinterpret_cast<u_int8_t*> (buf), &utf8len, maxLen, '/', UTF_LITTLE_ENDIAN);
    
    SafeDeleteNULL(desc);
    return kIOReturnSuccess;
}

UInt16 VoodooUSBDevice::getDeviceRelease()
{
    return USBToHost16(m_pDevice->getDeviceDescriptor()->bcdDevice);
}

IOReturn VoodooUSBDevice::getDeviceStatus(IOService * forClient, USBStatus * status)
{
    uint16_t stat;
    IOReturn result = sendStandardRequestIn(forClient, kDeviceRequestGetStatus, &stat, sizeof(stat));
    *status = stat;
    return result;
}

IOReturn VoodooUSBDevice::resetDevice()
{
    // Setting configuration value 0 (unconfigured) releases all opened interfaces / pipes
    m_pDevice->setConfiguration(0);
    return kIOReturnSuccess;
}

UInt8 VoodooUSBDevice::getNumConfigurations()
{
    return m_pDevice->getDeviceDescriptor()->bNumConfigurations;
}

const USBConfigurationDescriptor * VoodooUSBDevice::getFullConfigurationDescriptor(UInt8 configIndex)
{
    return m_pDevice->getConfigurationDescriptor(configIndex);
}

IOReturn VoodooUSBDevice::getConfiguration(IOService * forClient, UInt8 * configNumber)
{
    UInt8 config;
    IOReturn result = sendStandardRequestIn(forClient, kDeviceRequestGetConfiguration, &config, sizeof(config));
    *configNumber = config;
    return result;
}

IOReturn VoodooUSBDevice::setConfiguration(IOService * forClient, UInt8 configValue, bool startInterfaceMatching)
{
    return m_pDevice->setConfiguration(configValue, startInterfaceMatching);
}

bool VoodooUSBDevice::findFirstInterface(VoodooUSBInterface * interface)
{
    FuncLog("findFirstInterface");
    
    OSIterator * iterator = m_pDevice->getChildIterator(gIOServicePlane);
    
    if (!iterator)
    {
        return false;
    }
    
    IOUSBHostInterface * tempInterface;
    
    while (1)
    {
        if ((tempInterface = OSDynamicCast(IOUSBHostInterface, iterator->getNextObject())))
        {
            interface->setInterface(tempInterface);
            OSSafeReleaseNULL(tempInterface);
            break;
        }
    }
    
    OSSafeReleaseNULL(iterator);

    IOService * result = interface->getInterface();
    DebugLog("findFirstInterface(): getInterface() returns %p.\n", result);
    return result;
}

bool VoodooUSBDevice::open(IOService *forClient, IOOptionBits options, void *arg)
{
    return m_pDevice->open(forClient, options, arg);
}

void VoodooUSBDevice::close(IOService *forClient, IOOptionBits options)
{
    return m_pDevice->close(forClient, options);
}

UInt8 VoodooUSBDevice::getManufacturerStringIndex()
{
    return m_pDevice->getDeviceDescriptor()->iManufacturer;
}

UInt8 VoodooUSBDevice::getProductStringIndex()
{
    return m_pDevice->getDeviceDescriptor()->iProduct;
}

UInt8 VoodooUSBDevice::getSerialNumberStringIndex()
{
    return m_pDevice->getDeviceDescriptor()->iSerialNumber;
}

IOReturn VoodooUSBDevice::sendRequest(IOService * forClient, UInt8 bRequest, void * dataBuffer, UInt16 size, UInt8 direction, UInt8 type, UInt8 recipient)
{
    UInt32 bytesTransferred;
    
    StandardUSB::DeviceRequest request =
    {
        .bmRequestType  = static_cast <UInt8> (USBmakebmRequestType( direction, type, recipient )),
        .bRequest       = bRequest,
        .wValue         = 0,
        .wIndex         = 0,
        .wLength        = size
    };
    
    return m_pDevice->deviceRequest( forClient, request, dataBuffer, bytesTransferred, kUSBHostStandardRequestCompletionTimeout );
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

VoodooUSBInterface::VoodooUSBInterface()
{
    m_pInterface = NULL;
}

VoodooUSBInterface::~VoodooUSBInterface()
{
    OSSafeReleaseNULL(m_pInterface);
}

void VoodooUSBInterface::setInterface(IOService * interface)
{
    OSSafeReleaseNULL(m_pInterface);

    m_pInterface = OSDynamicCast(IOUSBHostInterface, interface);

    if (m_pInterface)
    {
        m_pInterface->retain();
    }
}

bool VoodooUSBInterface::open(IOService *forClient, IOOptionBits options, void *arg)
{
    return m_pInterface->open(forClient, options, arg);
}

void VoodooUSBInterface::close(IOService *forClient, IOOptionBits options)
{
    m_pInterface->close(forClient, options);
}

#ifdef DEBUG
UInt8 VoodooUSBInterface::getInterfaceNumber()
{
    return m_pInterface->getInterfaceDescriptor()->bInterfaceNumber;
}

UInt8 VoodooUSBInterface::getInterfaceClass()
{
    return m_pInterface->getInterfaceDescriptor()->bInterfaceClass;
}

UInt8 VoodooUSBInterface::getInterfaceSubClass()
{
    return m_pInterface->getInterfaceDescriptor()->bInterfaceSubClass;
}

UInt8 VoodooUSBInterface::getInterfaceProtocol()
{
    return m_pInterface->getInterfaceDescriptor()->bInterfaceProtocol;
}
#endif

bool VoodooUSBInterface::findPipe(VoodooUSBPipe * pipe, UInt8 type, UInt8 direction)
{
    DebugLog("findPipe(): direction = %d, type = %d\n", direction, type);
    const StandardUSB::ConfigurationDescriptor * configDesc = m_pInterface->getConfigurationDescriptor();
    const StandardUSB::InterfaceDescriptor     * ifaceDesc  = m_pInterface->getInterfaceDescriptor();
    if (!configDesc || !ifaceDesc)
    {
        DebugLog("findPipe(): configDesc = %p, ifaceDesc = %p\n", configDesc, ifaceDesc);
        return false;
    }
    const EndpointDescriptor * ep = NULL;
    IOUSBHostPipe * tempPipe = NULL;
    while ((ep = StandardUSB::getNextEndpointDescriptor(configDesc, ifaceDesc, ep)))
    {
        // check if endpoint matches type and direction
        UInt8 epDirection = StandardUSB::getEndpointDirection(ep);
        UInt8 epType      = StandardUSB::getEndpointType(ep);
        DebugLog("findPipe(): Endpoint found: epDirection = %d, epType = %d\n", epDirection, epType);
        if (direction == epDirection && type == epType)
        {
            DebugLog("findPipe(): Found matching endpoint!\n");
            
            // matches... try to make a pipe from the endpoint address
            tempPipe = m_pInterface->copyPipe(StandardUSB::getEndpointAddress(ep));
            if (!tempPipe)
            {
                ErrorLog("findPipe(): copyPipe() failed!!!\n");
                return false;
            }
            
            // set it in the pipe
            pipe->setPipe(tempPipe);
            tempPipe->release();
            return true;
        }
    }
    DebugLog("findPipe(): No matching endpoint found!\n");
    return false;
}

IOReturn VoodooUSBInterface::hciCommand(void * command, UInt16 length)
{
    UInt32 bytesTransfered;
    
    StandardUSB::DeviceRequest request =
    {
        .bmRequestType = makeDeviceRequestbmRequestType(kRequestDirectionOut, kRequestTypeClass, kRequestRecipientDevice),
        .bRequest = 0,
        .wValue = 0,
        .wIndex = 0,
        .wLength = length
    };
    
    return m_pInterface->deviceRequest(request, command, bytesTransfered, 0);
}

IOReturn VoodooUSBInterface::sendHCIRequest(uint16_t opCode, uint8_t paramLen, const void * param)
{
    UInt32 bytesTransfered;
    
    HciCommandHdr * command = NULL;
    bzero(command, sizeof( HciCommandHdr ));
    command->opCode = opCode;
    command->pLength = paramLen;
    memcpy((void *) command->pData, param, paramLen);
    
    StandardUSB::DeviceRequest request =
    {
        .bmRequestType = makeDeviceRequestbmRequestType(kRequestDirectionOut, kRequestTypeClass, kRequestRecipientDevice),
        .bRequest = 0,
        .wValue = 0,
        .wIndex = 0,
        .wLength = (uint16_t)(HCI_COMMAND_HDR_SIZE + paramLen)
    };
    
    return m_pInterface->deviceRequest(request, (void *) command, bytesTransfered);
}

VoodooUSBPipe::VoodooUSBPipe()
{
    m_pPipe = NULL;
}

VoodooUSBPipe::~VoodooUSBPipe()
{
    OSSafeReleaseNULL(m_pPipe);
}

void VoodooUSBPipe::setPipe(OSObject * pipe)
{
    OSSafeReleaseNULL(m_pPipe);
    
    m_pPipe = OSDynamicCast(IOUSBHostPipe, pipe);

    if (m_pPipe)
    {
        m_pPipe->retain();
    }
}

IOReturn VoodooUSBPipe::abort()
{
    return m_pPipe->abort();
}

IOReturn VoodooUSBPipe::read(IOMemoryDescriptor * buffer, UInt32 noDataTimeout, UInt32 completionTimeout, IOByteCount reqCount, USBCompletion *	completion, IOByteCount * bytesRead)
{
    if (completion)
    {
        return m_pPipe->io(buffer, (uint32_t) reqCount, completion, completionTimeout);
    }
        
    uint32_t bytesTransfered;
    IOReturn result = m_pPipe->io(buffer, (uint32_t) reqCount, bytesTransfered, completionTimeout);
    if (bytesRead)
    {
        *bytesRead = bytesTransfered;
    }
    return result;
}

IOReturn VoodooUSBPipe::write(IOMemoryDescriptor * buffer, UInt32 noDataTimeout, UInt32 completionTimeout, IOByteCount reqCount, USBCompletion *	completion)
{
    if (completion)
    {
        return m_pPipe->io(buffer, (uint32_t) reqCount, completion, completionTimeout);
    }
    uint32_t bytesTransfered;
    return m_pPipe->io(buffer, (uint32_t) reqCount, bytesTransfered, completionTimeout);
}

const USBEndpointDescriptor* VoodooUSBPipe::getEndpointDescriptor()
{
    return m_pPipe->getEndpointDescriptor();
}

IOReturn VoodooUSBPipe::clearStall()
{
    return m_pPipe->clearStall(false);
}
