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

#include "sc_device.h"
extern "C"
{
    #include "sc_mmio_common.h"
}
#include <iostream>
#include <string>
#include <vector>

#ifndef SC_MMIO_DEVICE_H
#define SC_MMIO_DEVICE_H

/*
 * Need that for QEMU device creation.
 */
typedef struct {} SysBusDevice;

extern "C"
{
    /*
     * From sysbus.h.
     */
    void sysbus_mmio_map(SysBusDevice *dev, int n, hwaddr addr);
    #define sysbus_from_qdev(dev) ((SysBusDevice *)(dev))
    #include "sc_mmio_common.h"
}

/*
 * This implement the SCDevice class.
 */

class SCMMIODevice : public SCDevice
{
public:
    SCMMIODevice(sc_core::sc_module_name name, std::string deviceName,
                 SCMMIOInfo *deviceInfo, uint64_t baseAddress);
    ~SCMMIODevice();
    void parseGSParam();
    void registerQEMUDevice();
    static SCMMIOInfo *getMMIODeviceInfo();
    static unsigned int getMMIODeviceCounter();

    /*
     * Connect the device to a greenrouter.
     */
    virtual void connect(gs::gp::GenericRouter<32> *router);
protected:
    static std::vector<SCMMIOInfo> devicesToBeRegistered;
private:
    /*
     * This is the information about MMIO device.
     */
    SCMMIOInfo *mmioDeviceInfo;
};

#endif /* SC_MMIO_DEVICE_H */
