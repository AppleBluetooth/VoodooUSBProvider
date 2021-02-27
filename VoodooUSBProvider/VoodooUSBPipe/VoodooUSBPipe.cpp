//
//  VoodooUSBPipe.cpp
//  VoodooUSBProvider
//
//  Created by Charlie Jiang on 2/27/21.
//  Copyright © 2021 Charlie Jiang. All rights reserved.
//

#include "VoodooUSBPipe.h"

OSDefineMetaClassAndAbstractStructors(VoodooUSBPipe, USBPipe)

inline IOReturn VoodooUSBPipe::abort()
{
    return super::Abort();
}

inline IOReturn VoodooUSBPipe::read(IOMemoryDescriptor * buffer, UInt32 noDataTimeout, UInt32 completionTimeout, IOByteCount reqCount, USBCompletion *    completion, IOByteCount * bytesRead)
{
    return super::Read(buffer, noDataTimeout, completionTimeout, reqCount, completion, bytesRead);
}

inline IOReturn VoodooUSBPipe::write(IOMemoryDescriptor * buffer, UInt32 noDataTimeout, UInt32 completionTimeout, IOByteCount reqCount, USBCompletion *    completion)
{
    return super::Write(buffer, noDataTimeout, completionTimeout, reqCount, completion);
}

inline const USBEndpointDescriptor * VoodooUSBPipe::getEndpointDescriptor()
{
    return super::GetEndpointDescriptor();
}

inline IOReturn VoodooUSBPipe::clearStall()
{
    return super::Reset();
}
