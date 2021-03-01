//
//  VoodooUSBDevice.h
//  VoodooUSBProvider
//
//  Created by Charlie Jiang on 2/26/21.
//  Copyright © 2021 Charlie Jiang. All rights reserved.
//

#ifndef VoodooUSBDevice_h
#define VoodooUSBDevice_h

#include "VoodooUSBInterface.h"

class VoodooUSBDevice : public USBDevice
{
    typedef USBDevice super;
    
    OSDeclareDefaultStructors(VoodooUSBDevice)
    
public:
    virtual bool open(IOService * forClient, IOOptionBits options = 0, void * arg = 0 ) override;
    virtual void close(IOService * forClient, IOOptionBits options = 0) override;
    
    UInt16 getVendorID();
    UInt16 getProductID();
    IOReturn getStringDescriptor(UInt8 index, char * buf, int maxLen, UInt16 lang = 0x409);
    UInt16 getDeviceRelease();
    IOReturn getDeviceStatus(IOService * forClient, USBStatus * status);
    IOReturn resetDevice();
    UInt8 getNumConfigurations();
    const USBConfigurationDescriptor * getFullConfigurationDescriptor(UInt8 configIndex);
    IOReturn getConfiguration(IOService * forClient, UInt8 * configNumber);
    IOReturn setConfiguration(IOService * forClient, UInt8 configValue, bool startInterfaceMatching = true);
    UInt8 getManufacturerStringIndex();
    UInt8 getProductStringIndex();
    UInt8 getSerialNumberStringIndex();
    
    bool findFirstInterface(VoodooUSBInterface * interface);
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
    
    IOReturn getVendorState(IOService * forClient, VendorState * state);
    IOReturn getAth3kVendorVersion(IOService * forClient, Ath3KVersion * version);
    IOReturn switchAth3kPID(IOService * forClient);
    IOReturn setAth3kNormalMode(IOService * forClient);
    IOReturn getQcaUsbVendorVersion(IOService * forClient, QCAVersion * version);
    bool     getQcaUsbDeviceInfo(QCAVersion * version, QCADeviceInfo * info);
    bool     getQcaUsbRamPatchVersion(OSData * firmwareData, QCADeviceInfo * devInfo, QCARamPatchVersion * version);
};

inline void setDevice(VoodooUSBDevice * device, IOService * provider)
{
    OSSafeReleaseNULL(device);
    
    device = OSDynamicCast(VoodooUSBDevice, provider);
    
    if (device)
    {
        device->retain();
    }
}

#endif /* VoodooUSBDevice_h */
