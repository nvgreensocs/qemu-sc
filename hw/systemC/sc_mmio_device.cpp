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

SCMMIODevice::SCMMIODevice(std::string deviceName) :
    SCDevice(deviceName)
{
    DBG("device created: " << deviceName);
    this->deviceName = deviceName;
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
 * Device must be registered in QEMU by sc_mmio.
 * This is linked with sc_mmio abstract device.
 */
void SCMMIODevice::registerQEMUDevice()
{
    /*
     * Fill the scMMIODeviceToBeRegistered vector.
     */
    DBG("device to be registered: " << deviceName);
    SCMMIOInfo deviceInfo;
    if (deviceName.length() > 63)
    {
        WRN("Device name exceeds 63 characters.");
    }
    strncpy(deviceInfo.name, deviceName.c_str(), 64);
    deviceInfo.name[63] = '\0';
    devicesToBeRegistered.push_back(deviceInfo);
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
