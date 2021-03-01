//
//  VoodooUSBInterface.h
//  VoodooUSBProvider
//
//  Created by Charlie Jiang on 2/27/21.
//  Copyright © 2021 Charlie Jiang. All rights reserved.
//

#ifndef VoodooUSBInterface_H
#define VoodooUSBInterface_H

#include "VoodooUSBPipe.h"

class VoodooUSBInterface : public USBInterface
{
    typedef USBInterface super;
    
    OSDeclareDefaultStructors(VoodooUSBInterface)
    
public:
    virtual bool open(IOService * forClient, IOOptionBits options = 0, void * arg = 0) override;
    virtual void close(IOService * forClient, IOOptionBits options = 0) override;
    
    UInt8 getInterfaceNumber();
    UInt8 getInterfaceClass();
    UInt8 getInterfaceSubClass();
    UInt8 getInterfaceProtocol();
    
    bool findPipe(VoodooUSBPipe * pipe, UInt8 type, UInt8 direction);
};

inline void setInterface(VoodooUSBInterface * interface, IOService * provider)
{
    OSSafeReleaseNULL(interface);

    interface = OSDynamicCast(VoodooUSBInterface, provider);
    
    if (interface)
    {
        interface->retain();
    }
}

#endif /* VoodooUSBInterface_H */
