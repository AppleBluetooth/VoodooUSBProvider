//
//  VoodooUSBInterface.cpp
//  VoodooUSBProvider
//
//  Created by Charlie Jiang on 2/27/21.
//  Copyright © 2021 Charlie Jiang. All rights reserved.
//

#include "VoodooUSBInterface.h"

OSDefineMetaClassAndAbstractStructors(VoodooUSBInterface, USBInterface)

inline UInt8 VoodooUSBInterface::getInterfaceNumber()
{
    return super::GetInterfaceNumber();
}

inline UInt8 VoodooUSBInterface::getInterfaceClass()
{
    return super::GetInterfaceClass();
}

inline UInt8 VoodooUSBInterface::getInterfaceSubClass()
{
    return super::GetInterfaceSubClass();
}

inline UInt8 VoodooUSBInterface::getInterfaceProtocol()
{
    return super::GetInterfaceProtocol();
}

bool VoodooUSBInterface::findPipe(VoodooUSBPipe * pipe, UInt8 type, UInt8 direction)
{
    VoodooUSBInfoLog("findPipe() - direction = %d, type = %d\n", direction, type);
    IOUSBFindEndpointRequest findEndpointRequest =
    {
        .type = type,
        .direction = direction
    };
    
    IOUSBPipe * tempPipe;
    
    if ((tempPipe = super::FindNextPipe(NULL, &findEndpointRequest)))
    {
        VoodooUSBDebugLog("findPipe() - Found matching endpoint!\n");
        setPipe(pipe, tempPipe);
        return true;
    }
    
    VoodooUSBErrorLog("findPipe() - No matching endpoint found!!!\n");
    return false;
}
