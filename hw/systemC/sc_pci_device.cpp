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
 * This static variable is used to register device into QEMU.
 */
std::vector<SCPCIInfo> SCPCIDevice::devicesToBeRegistered;

SCPCIDevice::SCPCIDevice(std::string deviceName) :
    SCDevice(deviceName)
{
    DBG("device created: " << deviceName);
    this->deviceName = deviceName;
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

/*
 * Device must be registred in QEMU by sc_pci.
 * This is linked with sc_pci abstract device.
 */
void SCPCIDevice::registerQEMUDevice()
{
    /*
     * Fill the scPCIDeviceToBeRegistered vector.
     */
    DBG("device to be registered: " << deviceName);
    SCPCIInfo deviceInfo;
    if (deviceName.length() > 63)
    {
        WRN("Device name exceeds 63 characters.");
    }
    strncpy(deviceInfo.name, deviceName.c_str(), 64);
    deviceInfo.name[63] = '\0';
    devicesToBeRegistered.push_back(deviceInfo);
}

SCPCIInfo *SCPCIDevice::getDeviceInfo()
{
    return (SCPCIInfo *)&devicesToBeRegistered[0];
}

unsigned int SCPCIDevice::getPCIDeviceCounter()
{
    return devicesToBeRegistered.size();
}


SCPCIInfo *getPCIDeviceInfo()
{
    return SCPCIDevice::getDeviceInfo();
}

unsigned int getPCIDeviceCounter(void)
{
    return SCPCIDevice::getPCIDeviceCounter();
}
