//
//  VoodooUSBHostInterface.cpp
//  VoodooUSBProvider
//
//  Created by Charlie Jiang on 2/27/21.
//  Copyright © 2021 Charlie Jiang. All rights reserved.
//

#include "VoodooUSBInterface.h"

OSDefineMetaClassAndAbstractStructors(VoodooUSBInterface, USBInterface)

inline UInt8 VoodooUSBInterface::getInterfaceNumber()
{
    return super::getInterfaceDescriptor()->bInterfaceNumber;
}

inline UInt8 VoodooUSBInterface::getInterfaceClass()
{
    return super::getInterfaceDescriptor()->bInterfaceClass;
}

inline UInt8 VoodooUSBInterface::getInterfaceSubClass()
{
    return super::getInterfaceDescriptor()->bInterfaceSubClass;
}

inline UInt8 VoodooUSBInterface::getInterfaceProtocol()
{
    return super::getInterfaceDescriptor()->bInterfaceProtocol;
}

bool VoodooUSBInterface::findPipe(VoodooUSBPipe * pipe, UInt8 type, UInt8 direction)
{
    VoodooUSBInfoLog("findPipe() - direction = %d, type = %d\n", direction, type);
    
    const StandardUSB::ConfigurationDescriptor * configDesc = super::getConfigurationDescriptor();
    const StandardUSB::InterfaceDescriptor     * ifaceDesc  = super::getInterfaceDescriptor();
    
    if (!configDesc || !ifaceDesc)
    {
        VoodooUSBErrorLog("findPipe() - Descriptor(s) invalid!!!\n");
        VoodooUSBInfoLog("findPipe() - configDesc = %p, ifaceDesc = %p\n", configDesc, ifaceDesc);
        return false;
    }
    
    const EndpointDescriptor * ep = NULL;
    IOUSBHostPipe * tempPipe = NULL;
    
    while ((ep = StandardUSB::getNextEndpointDescriptor(configDesc, ifaceDesc, ep)))
    {
        // Check if endpoint matches type and direction
        UInt8 epDirection = StandardUSB::getEndpointDirection(ep);
        UInt8 epType      = StandardUSB::getEndpointType(ep);
        VoodooUSBInfoLog("findPipe() - Endpoint found! epDirection = %d, epType = %d\n", epDirection, epType);
        if (direction == epDirection && type == epType)
        {
            VoodooUSBDebugLog("findPipe() - Found matching endpoint!\n");
            
            // Try to make a pipe from the endpoint address after it is matched
            tempPipe = super::copyPipe(StandardUSB::getEndpointAddress(ep));
            if (!tempPipe)
            {
                VoodooUSBErrorLog("findPipe() - copyPipe() failed!!!\n");
                return false;
            }
            
            setPipe(pipe, tempPipe);
            OSSafeReleaseNULL(tempPipe);
            return true;
        }
    }
    VoodooUSBErrorLog("findPipe() - No matching endpoint found!!!\n");
    return false;
}
