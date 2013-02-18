/*
 * SC_PCI_COMMON
 *
 *  Copyright (C) 2013 : GreenSocs Ltd
 *      http://www.greensocs.com/ , email: info@greensocs.com
 *
 *  Developed by :
 *  Frederic Konrad   <fred.konrad@greensocs.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "stdint.h"
#include "exec/hwaddr.h"

#ifndef SC_PCI_COMMON_H
#define SC_PCI_COMMON_H

/*
 * PCI BAR Type.
 */
typedef enum barType { NO_BAR = -1,
                       MEM_BAR = 0,
                       IO_BAR = 1 } barType;

typedef struct SCPCIInfo {
    /*
     * TypeName of the device.
     */
    char name[64];
    bool hasINT;
    uint16_t PCIVendorID;
    uint16_t PCIDeviceID;
    uint16_t PCIClassID;
    uint8_t PCIRevisionID;
    int8_t regionsType[6];
    uint32_t regionsSize[6];
} SCPCIInfo;

SCPCIInfo *getPCIDeviceInfo(void);
unsigned int getPCIDeviceCounter(void);

/*
 * See sc_wrapper.cpp for those one:
 */
void set_tlm_address(DeviceState *dev, uint8_t bar, hwaddr addr);

#endif /* SC_PCI_COMMON_H */
