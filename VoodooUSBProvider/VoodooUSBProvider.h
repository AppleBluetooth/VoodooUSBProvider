//
//  VoodooUSBProvider.h
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

#ifndef VoodooUSBProvider_H
#define VoodooUSBProvider_H

#include "VoodooUSBCommon.h"
#include "VoodooHCI.h"

#include <IOKit/usb/USB.h>

#if defined(TARGET_ELCAPITAN) || defined(TARGET_CATALINA)

#include <IOKit/usb/IOUSBHostInterface.h>
#include <sys/utfconv.h>

#define USBDevice                       IOUSBHostDevice
#define USBInterface                    IOUSBHostInterface
#define USBPipe                         IOUSBHostPipe

#define USBCompletion                   IOUSBHostCompletion
#define USBConfigurationDescriptor      StandardUSB::ConfigurationDescriptor
#define USBEndpointDescriptor           StandardUSB::EndpointDescriptor
#define USBStatus                       UInt16

#else

#include <IOKit/usb/IOUSBInterface.h>

#define USBDevice                       IOUSBDevice
#define USBInterface                    IOUSBInterface
#define USBPipe                         IOUSBPipe

#define USBCompletion                   IOUSBCompletion
#define USBConfigurationDescriptor      IOUSBConfigurationDescriptor
#define USBEndpointDescriptor           IOUSBEndpointDescriptor

#endif

class VoodooUSBInterface;
class VoodooUSBPipe;

class VoodooUSBDevice
{
    friend class VoodooUSBInterface;
    friend class VoodooUSBPipe;

private:
    USBDevice * m_pDevice;
    
public:
    VoodooUSBDevice();
    ~VoodooUSBDevice();
    
    void setDevice(IOService * provider);
    inline IOService * getDevice() { return m_pDevice; }
    
    UInt16 getVendorID();
    UInt16 getProductID();
    
    OSObject * getProperty(const char * name);
    void setProperty(const char * name, bool value);
    void removeProperty(const char * name);
    
    IOReturn getStringDescriptor(UInt8 index, char * buf, int maxLen, UInt16 lang = 0x409);
    
    UInt16 getDeviceRelease();
    IOReturn getDeviceStatus(IOService * forClient, USBStatus * status);
    IOReturn resetDevice();
    
    UInt8 getNumConfigurations();
    const USBConfigurationDescriptor * getFullConfigurationDescriptor(UInt8 configIndex);
    IOReturn getConfiguration(IOService * forClient, UInt8 * configNumber);
    IOReturn setConfiguration(IOService * forClient, UInt8 configValue, bool startInterfaceMatching = true);
    
    bool findFirstInterface(VoodooUSBInterface * interface);
    
    bool open(IOService * forClient, IOOptionBits options = 0, void * arg = 0 );
    void close(IOService * forClient, IOOptionBits options = 0);
    
    UInt8 getManufacturerStringIndex();
    UInt8 getProductStringIndex();
    UInt8 getSerialNumberStringIndex();
    
    IOReturn sendRequest(IOService * forClient, UInt8 bRequest, void * dataBuffer, UInt16 size, UInt8 direction, UInt8 type, UInt8 recipient);
    IOReturn sendVendorRequestIn(IOService * forClient, UInt8 bRequest, void * dataBuffer, UInt16 size);
    IOReturn sendVendorRequestOut(IOService * forClient, UInt8 bRequest, void * dataBuffer, UInt16 size);
    IOReturn sendStandardRequestIn(IOService * forClient, UInt8 bRequest, void * dataBuffer, UInt16 size);
    IOReturn sendStandardRequestOut(IOService * forClient, UInt8 bRequest, void * dataBuffer, UInt16 size);
    IOReturn sendHCIRequest(IOService * forClient, UInt16 opCode, UInt8 paramLen, const void * param, UInt8 direction);
    IOReturn sendHCIRequestIn(IOService * forClient, UInt16 opCode, UInt8 paramLen, const void * param);
    IOReturn sendHCIRequestOut(IOService * forClient, UInt16 opCode, UInt8 paramLen, const void * param);
    IOReturn sendHCICommand(IOService * forClient, void * command, UInt16 length, UInt8 direction);
    IOReturn sendHCICommandIn(IOService * forClient, void * command, UInt16 length);
    IOReturn sendHCICommandOut(IOService * forClient, void * command, UInt16 length);
};

class VoodooUSBInterface
{
    friend class VoodooUSBDevice;
    friend class VoodooUSBPipe;
    
private:
    USBInterface * m_pInterface;
    
public:
    VoodooUSBInterface();
    ~VoodooUSBInterface();
    
    void setInterface(IOService * interface);
    inline IOService * getInterface() { return (IOService *) m_pInterface; }
    
    bool open(IOService * forClient, IOOptionBits options = 0, void * arg = 0 );
    void close(IOService * forClient, IOOptionBits options = 0);
    
#ifdef DEBUG
    UInt8 getInterfaceNumber();
    UInt8 getInterfaceClass();
    UInt8 getInterfaceSubClass();
    UInt8 getInterfaceProtocol();
#endif
    
    bool findPipe(VoodooUSBPipe * pipe, UInt8 type, UInt8 direction);
};

class VoodooUSBPipe
{
    friend class VoodooUSBDevice;
    friend class VoodooUSBInterface;
    
private:
    USBPipe * m_pPipe;
    
public:
    VoodooUSBPipe();
    ~VoodooUSBPipe();
    
    void setPipe(OSObject * pipe);
    inline OSObject * getPipe() { return m_pPipe; }
    
    IOReturn abort();
    
    IOReturn read(IOMemoryDescriptor * buffer, UInt32 noDataTimeout, UInt32 completionTimeout, IOByteCount reqCount, USBCompletion * completion = 0, IOByteCount * bytesRead = 0);
    IOReturn write(IOMemoryDescriptor *	buffer, UInt32 noDataTimeout, UInt32 completionTimeout, IOByteCount reqCount, USBCompletion * completion = 0);
    const USBEndpointDescriptor * getEndpointDescriptor();
    IOReturn clearStall();
};

#endif /* VoodooUSBProvider_H */
