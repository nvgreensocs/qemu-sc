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

static int sc_pci_init(PCIDevice *pci_dev)
{

    return 0;
}

static void sc_pci_exit(PCIDevice *pci_dev)
{

}

static void sc_pci_reset(DeviceState *qdev)
{

}

static void sc_pci_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);

    k->init = sc_pci_init;
    k->exit = sc_pci_exit;
    k->vendor_id = 0x0000;
    k->revision = 0x00;
    k->class_id = 0x0000;
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
        type_register_static(&infos);
    }
}

type_init(sc_pci_register_types)

