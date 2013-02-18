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

#include "sc_mmio.h"
#include "hw/hw.h"
#include "hw/sysbus.h"

#define DEBUG_SC_MMIO

#ifdef DEBUG_SC_MMIO
#define DPRINTF(fmt, ...) \
do { printf("sc_mmio.c: " fmt , ## __VA_ARGS__); } while (0)
#else
#define DPRINTF(fmt, ...) do { } while (0)
#endif

static uint64_t mmio_read(void *opaque, hwaddr addr, unsigned size)
{
    SysBusDevice *dev = opaque;
    return sc_read(dev->mmio[0].addr + addr, size);
}

static void mmio_write(void *opaque, hwaddr addr, uint64_t data, unsigned size)
{
    SysBusDevice *dev = opaque;
    sc_write(dev->mmio[0].addr + addr, data, size);
}

static const MemoryRegionOps sc_ops = {
    .read = mmio_read,
    .write = mmio_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static int sc_mmio_init(SysBusDevice *bus_dev)
{
    SCMMIOState *dev = SC_MMIO(bus_dev);

    /*
     * Find the right device type.
     */
    Object *obj = OBJECT(bus_dev);
    int i;
    SCMMIOInfo *info = getMMIODeviceInfo();
    dev->deviceType = -1;
    
    for (i = 0; i < getMMIODeviceCounter(); i++) {
        if (object_dynamic_cast(obj, info[i].name) != NULL) {
            dev->deviceType = i;
        }
    }
    assert(dev->deviceType >= 0);
    DPRINTF("%s initialisation.\n", info[dev->deviceType].name);
    
    memory_region_init_io(&dev->mmio, &sc_ops, dev, info[dev->deviceType].name,
                          info[dev->deviceType].size);

    sysbus_init_mmio(bus_dev, &dev->mmio);
    return 0;
}

static void sc_mmio_reset(DeviceState *qdev)
{

}

static void sc_mmio_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);
    k->init = sc_mmio_init;
    dc->reset = sc_mmio_reset;
}

static const TypeInfo sc_mmio_info = {
    .name = TYPE_SC_MMIO,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(SCMMIOState),
    .abstract = true,
    .class_init = sc_mmio_class_init,
};

static const TypeInfo generic_info = {
    .name = "",
    .parent = TYPE_SC_MMIO,
    .instance_size = sizeof(SCMMIOState),
    .class_init = sc_mmio_class_init,
};

static void sc_mmio_register_types(void)
{
    /*
     * Info for all child devices.
     */
    TypeInfo infos = generic_info;
    SCMMIOInfo *sc_mmio_infos;

    /*
     * Register SC_MMIO.
     */
    type_register_static(&sc_mmio_info);

    /*
     * Get the array of devices to register.
     */
    sc_mmio_infos = getMMIODeviceInfo();

    /*
     * Register all the child devices.
     */    
    unsigned int counter;
    for (counter = 0; counter < getMMIODeviceCounter(); counter++) {
        infos.name = strdup(sc_mmio_infos[counter].name);
        type_register_static(&infos);
        DPRINTF("%s device registered.\n", sc_mmio_infos[counter].name);
    }
}

type_init(sc_mmio_register_types)

