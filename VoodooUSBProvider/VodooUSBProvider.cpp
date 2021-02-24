//
//  VoodooUSBProvider.cpp
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

inline void VoodooUSBDevice::setDevice(IOService * provider)
{
    OSSafeReleaseNULL(m_pDevice);
    
    m_pDevice = OSDynamicCast(IOUSBDevice, provider);
    
    if (m_pDevice)
    {
        m_pDevice->retain();
    }
}

inline UInt16 VoodooUSBDevice::getVendorID()
{
    return m_pDevice->GetVendorID();
}

inline UInt16 VoodooUSBDevice::getProductID()
{
    return m_pDevice->GetProductID();
}

inline OSObject* VoodooUSBDevice::getProperty(const char * name)
{
    return m_pDevice->getProperty(name);
}

inline void VoodooUSBDevice::setProperty(const char * name, bool value)
{
    m_pDevice->setProperty(name, value);
}

inline void VoodooUSBDevice::removeProperty(const char * name)
{
    m_pDevice->removeProperty(name);
}

IOReturn VoodooUSBDevice::getStringDescriptor(UInt8 index, char * buf, int maxLen, UInt16 lang)
{
    return m_pDevice->GetStringDescriptor(index, buf, maxLen, lang);
}

UInt16 VoodooUSBDevice::getDeviceRelease()
{
    return m_pDevice->GetDeviceRelease();
}

IOReturn VoodooUSBDevice::getDeviceStatus(IOService * forClient, USBStatus * status)
{
    return m_pDevice->GetDeviceStatus(status);
}

IOReturn VoodooUSBDevice::resetDevice()
{
    return m_pDevice->ResetDevice();
}

UInt8 VoodooUSBDevice::getNumConfigurations()
{
    return m_pDevice->GetNumConfigurations();
}

const USBConfigurationDescriptor* VoodooUSBDevice::getFullConfigurationDescriptor(UInt8 configIndex)
{
    return m_pDevice->GetFullConfigurationDescriptor(configIndex);
}

IOReturn VoodooUSBDevice::getConfiguration(IOService * forClient, UInt8 * configNumber)
{
    return m_pDevice->GetConfiguration(configNumber);
}

IOReturn VoodooUSBDevice::setConfiguration(IOService * forClient, UInt8 configValue, bool startInterfaceMatching)
{
    return m_pDevice->SetConfiguration(forClient, configValue, startInterfaceMatching);
}

bool VoodooUSBDevice::findFirstInterface(VoodooUSBInterface * interface)
{
    FuncLog("findFirstInterface");
    IOUSBFindInterfaceRequest request =
    {
        .bAlternateSetting  = kIOUSBFindInterfaceDontCare,
        .bInterfaceClass    = kIOUSBFindInterfaceDontCare,
        .bInterfaceSubClass = kIOUSBFindInterfaceDontCare,
        .bInterfaceProtocol = kIOUSBFindInterfaceDontCare
    };
    interface->setInterface(m_pDevice->FindNextInterface(NULL, &request));
    
    DebugLog("findFirstInterface(): FindNextInterface() returns %p.\n", interface);
    
    IOService * result = interface->getInterface();
    DebugLog("findFirstInterface(): getInterface returns %p.\n", result);
    return result;
}

bool VoodooUSBDevice::open(IOService * forClient, IOOptionBits options, void * arg)
{
    return m_pDevice->open(forClient, options, arg);
}

void VoodooUSBDevice::close(IOService * forClient, IOOptionBits options)
{
    return m_pDevice->close(forClient, options);
}

UInt8 VoodooUSBDevice::getManufacturerStringIndex()
{
    return m_pDevice->GetManufacturerStringIndex();
}

UInt8 VoodooUSBDevice::getProductStringIndex()
{
    return m_pDevice->GetProductStringIndex();
}
UInt8 VoodooUSBDevice::getSerialNumberStringIndex()
{
    return m_pDevice->GetSerialNumberStringIndex();
}

IOReturn VoodooUSBDevice::sendRequest(IOService * forClient, UInt8 bRequest, void * dataBuffer, UInt16 size, UInt8 direction, UInt8 type, UInt8 recipient)
{
    IOUSBDevRequest request =
    {
        .bmRequestType  = static_cast<UInt8> (USBmakebmRequestType( direction, type, recipient )),
        .bRequest       = bRequest,
        .wValue         = 0,
        .wIndex         = 0,
        .wLength        = size,
        .pData          = dataBuffer
    };
    
    return m_pDevice->DeviceRequest( &request );
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

VoodooUSBInterface::VoodooUSBInterface()
{
    m_pInterface = NULL;
}

VoodooUSBInterface::~VoodooUSBInterface()
{
    OSSafeReleaseNULL(m_pInterface);
}

inline void VoodooUSBInterface::setInterface(IOService * interface)
{
    OSSafeReleaseNULL(m_pInterface);

    m_pInterface = OSDynamicCast(IOUSBInterface, interface);
    
    if (m_pInterface)
    {
        m_pInterface->retain();
    }
}

inline bool VoodooUSBInterface::open(IOService * forClient, IOOptionBits options, void * arg)
{
    return m_pInterface->open(forClient, options, arg);
}

void VoodooUSBInterface::close(IOService * forClient, IOOptionBits options)
{
    m_pInterface->close(forClient, options);
}

#ifdef DEBUG
UInt8 VoodooUSBInterface::getInterfaceNumber()
{
    return m_pInterface->GetInterfaceNumber();
}

UInt8 VoodooUSBInterface::getInterfaceClass()
{
    return m_pInterface->GetInterfaceClass();
}

UInt8 VoodooUSBInterface::getInterfaceSubClass()
{
    return m_pInterface->GetInterfaceSubClass();
}

UInt8 VoodooUSBInterface::getInterfaceProtocol()
{
    return m_pInterface->GetInterfaceProtocol();
}
#endif

bool VoodooUSBInterface::findPipe(VoodooUSBPipe * pipe, UInt8 type, UInt8 direction)
{
    DebugLog("findPipe(): direction = %d, type = %d\n", direction, type);
    IOUSBFindEndpointRequest findEndpointRequest =
    {
        .type = type,
        .direction = direction
    };
    
    if ((IOUSBPipe * tempPipe = m_pInterface->FindNextPipe(NULL, &findEndpointRequest)))
    {
        DebugLog("findPipe(): Found matching endpoint!\n");
        pipe->setPipe(tempPipe);
        return true;
    }
    DebugLog("findPipe(): No matching endpoint found!\n");
    return false;
}

IOReturn VoodooUSBInterface::resetDevice()
{
    return hciCommand(&HCI_RESET, sizeof(HCI_RESET));
}

IOReturn VoodooUSBInterface::hciCommand(void * command, UInt16 length)
{
    IOUSBDevRequest request =
    {
        .bmRequestType = USBmakebmRequestType(kUSBOut, kUSBClass, kUSBDevice),
        .bRequest = 0,
        .wValue = 0,
        .wIndex = 0,
        .wLength = length,
        .pData = command
    };
    return m_pInterface->DeviceRequest( &request );
}

IOReturn VoodooUSBInterface::sendHCIRequest(UInt16 opCode, UInt8 paramLen, const void * param)
{
    HciCommandHdr * command = NULL;
    bzero(command, sizeof( HciCommandHdr ));
    command->opCode = opCode;
    command->pLength = paramLen;
    memcpy((void *) command->pData, param, paramLen);
    
    IOUSBDevRequest request =
    {
        .bmRequestType = USBmakebmRequestType(kUSBOut, kUSBClass, kUSBDevice),
        .bRequest = 0,
        .wValue = 0,
        .wIndex = 0,
        .wLength = (UInt16)(HCI_COMMAND_HDR_SIZE + paramLen),
        .pData = command
    };
    
    return m_pInterface->DeviceRequest( &request );
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
    m_pPipe = OSDynamicCast(IOUSBPipe, pipe);

    if (m_pPipe)
    {
        m_pPipe->retain();
    }
}

IOReturn VoodooUSBPipe::abort()
{
    return m_pPipe->Abort();
}

IOReturn VoodooUSBPipe::read(IOMemoryDescriptor * buffer, UInt32 noDataTimeout, UInt32 completionTimeout, IOByteCount reqCount, USBCompletion *	completion, IOByteCount * bytesRead)
{
    return m_pPipe->Read(buffer, noDataTimeout, completionTimeout, reqCount, completion, bytesRead);
}

IOReturn VoodooUSBPipe::write(IOMemoryDescriptor * buffer, UInt32 noDataTimeout, UInt32 completionTimeout, IOByteCount reqCount, USBCompletion *	completion)
{
    return m_pPipe->Write(buffer, noDataTimeout, completionTimeout, reqCount, completion);
}

const USBEndpointDescriptor* VoodooUSBPipe::getEndpointDescriptor()
{
    return m_pPipe->GetEndpointDescriptor();
}

IOReturn VoodooUSBPipe::clearStall()
{
    return m_pPipe->Reset();
}
