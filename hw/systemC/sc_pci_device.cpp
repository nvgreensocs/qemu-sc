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

#define DEBUG
#ifdef DEBUG
#define DBG(str)                                                    \
do                                                                  \
{                                                                   \
    std::cout << "sc_pci_device.cpp: debug: " << str << std::endl;  \
}                                                                   \
while(0)
#else
#define DBG(str) do { } while (0)
#endif

#define WRN(str)                                                    \
do                                                                  \
{                                                                   \
    std::cout << "sc_pci_device.cpp: warning: " << str << std::endl;\
}                                                                   \
while(0)

#define ERR(str)                                                    \
do                                                                  \
{                                                                   \
    std::cout << "sc_pci_device.cpp: error: " << str << std::endl;  \
}                                                                   \
while(0)

#include "sc_pci_device.h"

/*
 * This static vector is used to register device into QEMU.
 */
std::vector<SCPCIInfo> SCPCIDevice::devicesToBeRegistered;

/*
 * This static vector is used to keep instanciated devices.
 */
std::vector<SCPCIDevice *> SCPCIDevice::instanciatedDevices;

SCPCIDevice::SCPCIDevice(sc_core::sc_module_name name,
                         std::string deviceName,
                         SCPCIInfo *pciDeviceInfo):
    SCDevice(name, deviceName)
{
    PCIBus *rootPCIBus;
    DeviceState *qemuDevice;

    this->deviceName = deviceName;
    this->pciDeviceInfo = pciDeviceInfo;

    /*
     * Look for a PCI Bus in QEMU.
     */
    rootPCIBus = pci_find_root_bus(0);
    if (rootPCIBus == NULL)
    {
        ERR("No pci bus found.");
        return;
    }

    /*
     * Create and Init the QEMU device.
     */
    qemuDevice = qdev_create((BusState *)rootPCIBus, deviceName.c_str());
    qdev_init(qemuDevice);
    DBG("device created: " << deviceName);

    /*
     * Keep the pointer so we can set the TLM address when necessary.
     */
    this->qemuDevice = qemuDevice;
    instanciatedDevices.push_back(this);

    /*
     * PCI Socket configuration.
     */
	target_port.bind_b_if(*this);
	target_port.peq.out_port(*this);
}

SCPCIDevice::~SCPCIDevice()
{
    DBG("device deleted: " << deviceName);
}

/*
 * Parse all the information from the config file.
 */
void SCPCIDevice::parseGSParam()
{
    /*
     * To be defined later.
     */
}

SCPCIInfo *SCPCIDevice::getDeviceInfo()
{
    return (SCPCIInfo *)&devicesToBeRegistered[0];
}

unsigned int SCPCIDevice::getPCIDeviceCounter()
{
    return devicesToBeRegistered.size();
}

void SCPCIDevice::setTLMAddress(DeviceState *dev, uint8_t bar, hwaddr addr)
{
    DBG("TLM Address set to: " << addr << " for device " << dev
        << " BAR " << (unsigned int)(bar) << ".");
    /*
     * Maybe slow when we have a big amount of device. But it's called one time
     * at the first R/W.
     */
    for (int i = 0; i < instanciatedDevices.size(); i++)
    {
        if (instanciatedDevices[i]->qemuDevice == dev)
        {
            /*
             * We found it: just write the TLM Address and refresh the router
             * Address map.
             */
            instanciatedDevices[i]->target_port.base_addr = addr;
            instanciatedDevices[i]->target_port.high_addr = addr
                      + instanciatedDevices[i]->pciDeviceInfo->regionsSize[bar];
            instanciatedDevices[i]->pciRouter->refreshAddressMap();
            break;
        }
    }
}

void SCPCIDevice::connect(gs::gp::GenericRouter<32> *router)
{
    DBG("connect " << deviceName << " on a router.");

    /*
     * Keep the pciRouter to refresh it's address map after changing TLM
     * Address.
     */
    this->pciRouter = router;
    pciRouter->init_socket(target_port);
}

SCPCIInfo *getPCIDeviceInfo()
{
    return SCPCIDevice::getDeviceInfo();
}

unsigned int getPCIDeviceCounter(void)
{
    return SCPCIDevice::getPCIDeviceCounter();
}

void set_tlm_address(DeviceState *dev, uint8_t bar, hwaddr addr)
{
    SCPCIDevice::setTLMAddress(dev, bar, addr);
}
