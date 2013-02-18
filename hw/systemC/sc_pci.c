/*
 * SC_PCI
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

#include "sc_pci.h"
#include "hw/hw.h"
#include "hw/pci/pci.h"

//#define DEBUG_SC_PCI

#ifdef DEBUG_SC_PCI
#define DPRINTF(fmt, ...) \
do { printf("sc_pci.c: " fmt , ## __VA_ARGS__); } while (0)
#else
#define DPRINTF(fmt, ...) do { } while (0)
#endif

static uint64_t sc_pci_read(void *opaque, hwaddr addr, unsigned size,
                            unsigned int bar_id);
static void sc_pci_write(void *opaque, hwaddr addr, uint64_t data,
                         unsigned size, unsigned int bar_id);

#define REGION_BAR_OPS(bar) \
static uint64_t sc_pci_read_##bar## _(void *opaque, hwaddr addr, unsigned size)\
{                                                                              \
    SCPCIState *dev = opaque;                                                  \
    if (!dev->tlmAddressSet[bar]) {                                            \
        DPRINTF("first time read on bar: sending tlm address.\n");             \
        dev->tlmAddressSet[bar] = true;                                        \
        set_tlm_address((DeviceState *)opaque, bar,                            \
                        pci_get_bar_addr((PCIDevice *)opaque, bar));           \
    }                                                                          \
    return sc_pci_read(opaque, addr, size, bar);                               \
}                                                                              \
                                                                               \
static void sc_pci_write_##bar##_(void *opaque, hwaddr addr, uint64_t data,    \
                           unsigned size)                                      \
{                                                                              \
    SCPCIState *dev = opaque;                                                  \
    if (!dev->tlmAddressSet[bar]) {                                            \
        DPRINTF("first time write on bar: sending tlm address.\n");            \
        dev->tlmAddressSet[bar] = true;                                        \
        set_tlm_address((DeviceState *)opaque, bar,                            \
                        pci_get_bar_addr((PCIDevice *)opaque, bar));           \
    }                                                                          \
    sc_pci_write(opaque, addr, data, size, bar);                               \
}

REGION_BAR_OPS(0)
REGION_BAR_OPS(1)
REGION_BAR_OPS(2)
REGION_BAR_OPS(3)
REGION_BAR_OPS(4)
REGION_BAR_OPS(5)

static const MemoryRegionOps sc_pci_ops[6] = {{
    .read = sc_pci_read_0_,
    .write = sc_pci_write_0_,
    .endianness = DEVICE_NATIVE_ENDIAN,
},{
    .read = sc_pci_read_1_,
    .write = sc_pci_write_1_,
    .endianness = DEVICE_NATIVE_ENDIAN,
},{
    .read = sc_pci_read_2_,
    .write = sc_pci_write_2_,
    .endianness = DEVICE_NATIVE_ENDIAN,
},{
    .read = sc_pci_read_3_,
    .write = sc_pci_write_3_,
    .endianness = DEVICE_NATIVE_ENDIAN,
},{
    .read = sc_pci_read_4_,
    .write = sc_pci_write_4_,
    .endianness = DEVICE_NATIVE_ENDIAN,
},{
    .read = sc_pci_read_5_,
    .write = sc_pci_write_5_,
    .endianness = DEVICE_NATIVE_ENDIAN,
}};

static uint64_t sc_pci_read(void *opaque, hwaddr addr, unsigned size,
                            unsigned int bar_id)
{
    DPRINTF("read BAR %u Memory address %lu\n", bar_id,
            pci_get_bar_addr((PCIDevice *)opaque, bar_id));
    return sc_read(pci_get_bar_addr((PCIDevice *)opaque, bar_id) + addr, size);
}

static void sc_pci_write(void *opaque, hwaddr addr, uint64_t data,
                         unsigned size, unsigned int bar_id)
{
    DPRINTF("write BAR %u offset %lu data %lu\n", bar_id,
            pci_get_bar_addr((PCIDevice *)opaque, bar_id), data);
    sc_write(pci_get_bar_addr((PCIDevice *)opaque, bar_id) + addr, data, size);
}

static int sc_pci_init(PCIDevice *pci_dev)
{
    char bar_name[66];
    SCPCIState *dev = SC_PCI(pci_dev);

    /*
     * Find the right device type.
     */
    Object *obj = OBJECT(pci_dev);
    int i;
    SCPCIInfo *info = getPCIDeviceInfo();
    dev->deviceType = -1;
    
    for (i = 0; i < getPCIDeviceCounter(); i++) {
        if (object_dynamic_cast(obj, info[i].name) != NULL) {
            dev->deviceType = i;
        }
    }
    assert(dev->deviceType >= 0);
    DPRINTF("%s initialisation.\n", info[dev->deviceType].name);

    /*
     * set the PCI BARs.
     */
    for (i = 0; i < 6; i++) {
        dev->tlmAddressSet[i] = false;
        switch (info[dev->deviceType].regionsType[i])
        {
            case MEM_BAR:
                sprintf(bar_name, "%s.%u", info[dev->deviceType].name, i);
                
            break;
            case IO_BAR:
                sprintf(bar_name, "%s.%u", info[dev->deviceType].name, i);
                memory_region_init_io(&dev->mem, &(sc_pci_ops[i]), dev,
                                      bar_name,
                                      info[dev->deviceType].regionsSize[i]);
                pci_register_bar(pci_dev, i, PCI_BASE_ADDRESS_SPACE_IO,
                                 &dev->mem);
                DPRINTF("BAR[%u]: IO: %u: %s.\n",
                        (unsigned int)i,
                        info[dev->deviceType].regionsSize[i], bar_name);
            break;
            case NO_BAR:
            break;
            default:
                DPRINTF("BAR[%u]: Unknown bar type: %s.\n", (unsigned int)i,
                                                            bar_name);
                return -1;
            break;
        }
    }
    return 0;
}

static void sc_pci_exit(PCIDevice *pci_dev)
{

}

static void sc_pci_reset(DeviceState *qdev)
{

}

static void sc_pci_class_init(ObjectClass *klass, void *class_data)
{
    /*
     * Here class_data is the data passed in info structure.
     */
    SCPCIInfo *sc_pci_info = class_data;
    DeviceClass *dc = DEVICE_CLASS(klass);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);

    k->init = sc_pci_init;
    k->exit = sc_pci_exit;
    if (class_data != NULL) {
        /*
         * Set the value for our PCI Device.
         */
        k->vendor_id = sc_pci_info->PCIVendorID;
        k->device_id = sc_pci_info->PCIDeviceID;
        k->revision = sc_pci_info->PCIRevisionID;
        k->class_id = sc_pci_info->PCIClassID;
    }
    dc->reset = sc_pci_reset;
}

static const TypeInfo sc_pci_info = {
    .name = TYPE_SC_PCI,
    .parent = TYPE_PCI_DEVICE,
    .instance_size = sizeof(SCPCIState),
    .abstract = true,
    .class_init = sc_pci_class_init,
};

static const TypeInfo generic_info = {
    .name = "",
    .parent = TYPE_SC_PCI,
    .instance_size = sizeof(SCPCIState),
    .class_init = sc_pci_class_init,
};

static void sc_pci_register_types(void)
{
    /*
     * Info for all child devices.
     */
    TypeInfo infos = generic_info;
    SCPCIInfo *sc_pci_infos;

    /*
     * Register SC_PCI.
     */
    type_register_static(&sc_pci_info);

    /*
     * Get the array of devices to register.
     */
    sc_pci_infos = getPCIDeviceInfo();

    /*
     * Register all the child devices.
     */    
    unsigned int counter;
    for (counter = 0; counter < getPCIDeviceCounter(); counter++) {
        infos.name = strdup(sc_pci_infos[counter].name);
        infos.class_data = &sc_pci_infos[counter];
        type_register_static(&infos);
        DPRINTF("%s device registered.\n", sc_pci_infos[counter].name);
    }
}

type_init(sc_pci_register_types)

