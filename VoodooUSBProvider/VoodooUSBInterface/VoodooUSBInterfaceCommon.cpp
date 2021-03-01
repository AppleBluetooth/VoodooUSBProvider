//
//  VoodooUSBInterfaceCommon.cpp
//  VoodooUSBProvider15
//
//  Created by Charlie Jiang on 3/1/21.
//  Copyright © 2021 Charlie Jiang. All rights reserved.
//

#include "VoodooUSBInterface.h"

inline bool VoodooUSBInterface::open(IOService * forClient, IOOptionBits options, void * arg)
{
    return super::open(forClient, options, arg);
}

inline void VoodooUSBInterface::close(IOService * forClient, IOOptionBits options)
{
    if (isOpen(forClient))
    {
        super::close(forClient, options);
    }
}
