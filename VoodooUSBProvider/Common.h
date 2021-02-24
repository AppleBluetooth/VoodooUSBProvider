//
//  Common.h
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

#ifndef Common_h
#define Common_h

#define SafeDeleteNULL(x) do { if (x) { delete x; x = NULL; } } while (0)
#define SafeDeleteArrayNULL(x) do { if (x) { delete[] x; x = NULL; } } while (0)

#define DRIVER_NAME "VoodooUSBProvider"

#ifdef DEBUG
#define DebugLog(args...) do { IOLog(DRIVER_NAME ": " args); } while (0)
#else
#define DebugLog(args...) do { } while (0)
#endif /* DEBUG */

#define AlwaysLog(args...) do { IOLog(DRIVER_NAME ": " args); } while (0)

#define ErrorLog(args...) AlwaysLog("Error! " args)
#define InfoLog(args...) AlwaysLog(args)
#define WarningLog(args...) DebugLog("Warning! " args)
#define FuncLog(args...) DebugLog(args "()\n")

#endif /* Common_h */
