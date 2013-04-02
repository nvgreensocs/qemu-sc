/*
 * SYSTEMC MMIO DEVICE
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
    std::cout << "sc_mmio_device.cpp: debug: " << str << std::endl;  \
}                                                                   \
while(0)
#else
#define DBG(str) do { } while (0)
#endif

#define WRN(str)                                                    \
do                                                                  \
{                                                                   \
    std::cout << "sc_mmio_device.cpp: warning: " << str << std::endl;\
}                                                                   \
while(0)

#define ERR(str)                                                    \
do                                                                  \
{                                                                   \
    std::cout << "sc_mmio_device.cpp: error: " << str << std::endl;  \
}                                                                   \
while(0)

#include "sc_mmio_device.h"
#include "cstring"

/*
 * This static variable is used to register device into QEMU.
 */
std::vector<SCMMIOInfo> SCMMIODevice::devicesToBeRegistered;

SCMMIODevice::SCMMIODevice(sc_core::sc_module_name name,
                           std::string deviceName, SCMMIOInfo *deviceInfo,
                           uint64_t baseAddress,
                           qemu_irq IRQ,
                           unsigned int nRegister):
    SCDevice(name, deviceName, nRegister)
{
    this->deviceName = deviceName;
    this->mmioDeviceInfo = deviceInfo;

    /*
     * Create a sysbus device called deviceName into QEMU.
     */
    qemuDevice = sysbus_create_varargs(deviceName.c_str(), baseAddress, IRQ,
                                                                        NULL);
    DBG("device created: " << deviceName << "@" << baseAddress);

    /*
     * Keep the pointer for interrupt handling.
     */
    this->qemuDevice = qemuDevice;

    /*
     * Keep the pointer to IRQ.
     */
    if (IRQ != NULL)
    {
        this->qemuIRQ = get_mmio_irq(qemuDevice);
    }
    else
    {
        this->qemuIRQ = NULL;
    }

    /*
     * Target Socket configuration.
     */
    target_port.base_addr = baseAddress;
    target_port.high_addr = baseAddress + mmioDeviceInfo->size - 1;
    target_port.setAddress(baseAddress, baseAddress + mmioDeviceInfo->size - 1);
}

SCMMIODevice::~SCMMIODevice()
{
    DBG("device deleted: " << deviceName);
}

/*
 * Parse all the information from the config file.
 */
void SCMMIODevice::parseGSParam()
{
    /*
     * To be defined later.
     */
}

/*
 * Connect the device to a greenrouter.
 */
void SCMMIODevice::connect(gs::gp::GenericRouter<32> *router)
{
    DBG("connect " << deviceName << " on a router.");

    /*
     * Nothing more to do.
     */
    router->init_socket(target_port);
}

SCMMIOInfo *SCMMIODevice::getMMIODeviceInfo()
{
    return (SCMMIOInfo *)&devicesToBeRegistered[0];
}

unsigned int SCMMIODevice::getMMIODeviceCounter()
{
    return devicesToBeRegistered.size();
}


SCMMIOInfo *getMMIODeviceInfo()
{
    return SCMMIODevice::getMMIODeviceInfo();
}

unsigned int getMMIODeviceCounter(void)
{
    return SCMMIODevice::getMMIODeviceCounter();
}

qemu_irq SCMMIODevice::getQEMUIRQ()
{
    return get_mmio_irq(qemuDevice);
}
