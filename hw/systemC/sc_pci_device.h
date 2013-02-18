/*
 * SYSTEMC PCI DEVICE
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

#include "sc_device.h"
#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#ifndef SC_PCI_DEVICE_H
#define SC_PCI_DEVICE_H

/*
 * Need that for QEMU device creation.
 */
typedef struct {} PCIBus;

extern "C"
{
    /*
     * From pci.h.
     */
    PCIBus *pci_find_root_bus(int domain);
    #include "sc_pci_common.h"
}

/*
 * This implement the SCDevice class.
 */
class SCPCIDevice:
    public SCDevice
{
public:
    SCPCIDevice(sc_core::sc_module_name name, std::string deviceName,
                SCPCIInfo *pciDeviceInfo);
    ~SCPCIDevice();
    void parseGSParam();
    static SCPCIInfo *getDeviceInfo();
    static unsigned int getPCIDeviceCounter();

    /*
     * Called in sc_pci: to set the TLM address for the pciPort.
     */
    static void setTLMAddress(DeviceState *dev, uint8_t bar, hwaddr addr);
    
    /*
     * Connect the device to a greenrouter.
     */
    void connect(gs::gp::GenericRouter<32> *router);
protected:
    static std::vector<SCPCIInfo> devicesToBeRegistered;
private:
    static std::vector<SCPCIDevice *> instanciatedDevices;
    /*
     * This is the information about PCI device.
     */
    SCPCIInfo *pciDeviceInfo;

    /*
     * This is the PCI router on which the device is connected.
     */
    gs::gp::GenericRouter<32> *pciRouter;
};

#endif /* SC_DEVICE_H */
