/*
 * SC_MMIO
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

#ifndef SC_MMIO_H
#define SC_MMIO_H

#include "hw/hw.h"
#include "hw/sysbus.h"
#include "sc_common.h"
#include "sc_mmio_common.h"


#define TYPE_SC_MMIO "sc-mmio"
#define SC_MMIO(obj) OBJECT_CHECK(SCMMIOState, (obj), TYPE_SC_MMIO)
#define SC_MMIO_GET_CLASS(obj) \
        OBJECT_GET_CLASS(SCMMIOClass, obj, TYPE_SC_MMIO)
#define SC_PCI_CLASS(klass) \
        OBJECT_CLASS_CHECK(SCMMIOClass, klass, TYPE_SC_MMIO)

typedef struct SCMMIOState {
    SysBusDevice parent_obj;

    /*
     * Base address of the MMIO area.
     */
    hwaddr base_address;
    MemoryRegion mmio;

    /*
     * IRQ connected to the device.
     */
    qemu_irq irq;

    /*
     * Device type id, infos about the device can be found with this field.
     */
    int32_t deviceType;
} SCMMIOState;


#endif
