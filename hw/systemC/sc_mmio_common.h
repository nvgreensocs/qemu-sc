/*
 * SC_MMIO_COMMON
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

#ifndef SC_MMIO_COMMON_H
#define SC_MMIO_COMMON_H

/*
 * SCMMIOInfo:
 * name:
 * The name of the QEMU device.
 * size:
 * The size of the IO area.
 */

typedef struct SCMMIOInfo {
    char name[64];
    uint64_t size;
} SCMMIOInfo;

/*
 * Get the infos of the SystemC registered devices.
 */
SCMMIOInfo *getMMIODeviceInfo(void);

/*
 * Get the amount of registered devices.
 */
unsigned int getMMIODeviceCounter(void);

/*
 * Return the QEMU IRQ for a given device.
 * Called in sc_mmio_device.cpp to keep the IRQ pointer.
 * Then the IRQ can be Raised/Lowed in the wrapper code.
 */
qemu_irq get_mmio_irq(DeviceState *dev);

#endif /* SC_MMIO_COMMON_H */
