//
//  VoodooUSBPipe.h
//  VoodooUSBProvider
//
//  Created by Charlie Jiang on 2/27/21.
//  Copyright © 2021 Charlie Jiang. All rights reserved.
//

#ifndef VoodooUSBPipe_h
#define VoodooUSBPipe_h

#include "VoodooUSBCommon.h"

class VoodooUSBPipe : public USBPipe
{
    typedef USBPipe super;
    
public:
    OSDeclareDefaultStructors(VoodooUSBPipe)
    
    IOReturn abort();
    
    IOReturn read(IOMemoryDescriptor * buffer, UInt32 noDataTimeout, UInt32 completionTimeout, IOByteCount reqCount, USBCompletion * completion = 0, IOByteCount * bytesRead = 0);
    IOReturn write(IOMemoryDescriptor * buffer, UInt32 noDataTimeout, UInt32 completionTimeout, IOByteCount reqCount, USBCompletion * completion = 0);
    const USBEndpointDescriptor * getEndpointDescriptor();
    IOReturn clearStall();
};

void setPipe(VoodooUSBPipe * pipe, OSObject * provider)
{
    OSSafeReleaseNULL(pipe);
    pipe = OSDynamicCast(VoodooUSBPipe, provider);

    if (pipe)
    {
        pipe->retain();
    }
}

#endif /* VoodooUSBPipe_h */
