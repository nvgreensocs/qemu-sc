/*
 * SYSTEMC DEVICE
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

unsigned int SCDevice::deviceCounter = 0;

SCDevice::SCDevice(sc_core::sc_module_name name, std::string deviceName):
    target_port("target_port"),
    sc_core::sc_module(name)
{
    this->deviceName = deviceName;
    deviceID = deviceCounter++;
}

std::string SCDevice::getDeviceName()
{
    return deviceName;
}

unsigned int SCDevice::getDeviceID()
{
    return deviceID;
}

