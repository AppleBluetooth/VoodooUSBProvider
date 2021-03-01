//
//  VoodooUSBDeviceCommon.cpp
//  VoodooUSBProvider15
//
//  Created by Charlie Jiang on 3/1/21.
//  Copyright © 2021 Charlie Jiang. All rights reserved.
//

#include "VoodooUSBDevice.h"

inline bool VoodooUSBDevice::open(IOService * forClient, IOOptionBits options, void * arg)
{
    return super::open(forClient, options, arg);
}

inline void VoodooUSBDevice::close(IOService * forClient, IOOptionBits options)
{
    if (isOpen(forClient))
    {
        super::close(forClient, options);
    }
}

inline IOReturn VoodooUSBDevice::getVendorState(IOService * forClient, VendorState * state)
{
    return sendVendorRequestIn(forClient, VENDOR_GETSTATE, state, sizeof(VendorState));
}

inline IOReturn VoodooUSBDevice::getAth3kVendorVersion(IOService * forClient, Ath3KVersion * version)
{
    return sendVendorRequestIn(forClient, VENDOR_QCA_GETVERSION, (void *) version, sizeof(Ath3KVersion));
}

inline IOReturn VoodooUSBDevice::switchAth3kPID(IOService * forClient)
{
    return sendVendorRequestIn(forClient, ATH3K_SWITCH_VID_PID, (void *) NULL, 0);
}

IOReturn VoodooUSBDevice::setAth3kNormalMode(IOService * forClient)
{
    VendorState * state;
    if (getVendorState(forClient, state))
    {
        VoodooUSBErrorLog("Unable to get vendor state!!!\n");
        return kIOReturnError;
    }
    
    if ((*state & ATH3K_MODE_MASK) == ATH3K_NORMAL_MODE)
    {
        VoodooUSBWarningLog("Firmware is already in normal mode!\n");
        return kIOReturnSuccess;
    }
    
    return sendVendorRequestIn(forClient, ATH3K_SET_NORMAL_MODE, (void *) NULL, 0);
}

inline IOReturn VoodooUSBDevice::getQcaUsbVendorVersion(IOService * forClient, QCAVersion * version)
{
    return sendVendorRequestIn(forClient, VENDOR_QCA_GETVERSION, version, sizeof(QCAVersion));
}

bool VoodooUSBDevice::getQcaUsbDeviceInfo(QCAVersion * version, QCADeviceInfo * info)
{
    static QCADeviceInfo QCADevicesTable[] =
    {
        { 0x00000100, 20, 4,  8 },      /* Rome 1.0 */
        { 0x00000101, 20, 4,  8 },      /* Rome 1.1 */
        { 0x00000200, 28, 4, 16 },      /* Rome 2.0 */
        { 0x00000201, 28, 4, 16 },      /* Rome 2.1 */
        { 0x00000300, 28, 4, 16 },      /* Rome 3.0 */
        { 0x00000302, 28, 4, 16 },      /* Rome 3.2 */
        { 0x00130100, 40, 4, 16 },      /* WCN6855 1.0 */
        { 0x00130200, 40, 4, 16 },      /* WCN6855 2.0 */
    };
    
    for (int i = 0; i < ARRAY_SIZE(QCADevicesTable); ++i)
    {
        if ((UInt32)(SInt32)(version->romVersion) == QCADevicesTable[i].romVersion)
        {
            info = &QCADevicesTable[i];
            return true;
        }
    }
    return false;
}

inline bool VoodooUSBDevice::getQcaUsbRamPatchVersion(OSData * firmwareData, QCADeviceInfo * devInfo, QCARamPatchVersion * version)
{
    version = (QCARamPatchVersion *) (firmwareData + devInfo->versionOffset);
    return version ? true : false;
}
