//
//  VoodooUSBHostPipe.cpp
//  VoodooUSBProvider
//
//  Created by Charlie Jiang on 2/27/21.
//  Copyright © 2021 Charlie Jiang. All rights reserved.
//

#include "VoodooUSBPipe.h"

OSDefineMetaClassAndAbstractStructors(VoodooUSBPipe, USBPipe)

inline IOReturn VoodooUSBPipe::abort()
{
    return super::abort();
}

IOReturn VoodooUSBPipe::read(IOMemoryDescriptor * buffer, UInt32 noDataTimeout, UInt32 completionTimeout, IOByteCount reqCount, USBCompletion * completion, IOByteCount * bytesRead)
{
    if (completion)
    {
        return super::io(buffer, (UInt32) reqCount, completion, completionTimeout);
    }
        
    UInt32 bytesTransfered;
    IOReturn result = super::io(buffer, (UInt32) reqCount, bytesTransfered, completionTimeout);
    if (bytesRead)
    {
        *bytesRead = bytesTransfered;
    }
    return result;
}

IOReturn VoodooUSBPipe::write(IOMemoryDescriptor * buffer, UInt32 noDataTimeout, UInt32 completionTimeout, IOByteCount reqCount, USBCompletion *    completion)
{
    if (completion)
    {
        return super::io(buffer, (UInt32) reqCount, completion, completionTimeout);
    }
    
    UInt32 bytesTransfered;
    return super::io(buffer, (UInt32) reqCount, bytesTransfered, completionTimeout);
}

inline const USBEndpointDescriptor * VoodooUSBPipe::getEndpointDescriptor()
{
    return super::getEndpointDescriptor();
}

inline IOReturn VoodooUSBPipe::clearStall()
{
    return super::clearStall(false);
}
