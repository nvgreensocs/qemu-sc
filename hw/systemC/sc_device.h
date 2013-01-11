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

#include <iostream>
#include <string>

#ifndef SC_DEVICE_H
#define SC_DEVICE_H

/*
 * Abstract class for SystemC Device.
 * This can be MMIO, PCI,...
 * Basically it is used to get the GSParam and hold the MMIO or PCI specific
 * variable.
 */

class SCDevice
{
    public:
    SCDevice(std::string deviceName);
    virtual void parseGSParam() = 0;
    virtual void registerQEMUDevice() = 0;
    std::string getDeviceName();
    unsigned int getDeviceID();
    protected:
    unsigned int deviceID;
    std::string deviceName;
    private:
    static unsigned int deviceCounter;
};

#endif /* SC_DEVICE_H */
