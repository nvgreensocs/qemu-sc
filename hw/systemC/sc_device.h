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

/*
 * For sc_module.
 */
#include <systemc>

/*
 * For slave socket.
 */
#include "gsgpsocket/transport/GSGPSlaveSocket.h"
#include "gsgpsocket/transport/GSGPMasterBlockingSocket.h"
#include "greenrouter/genericRouter.h"

#include <greenreg/greenreg.h>
#include <greenreg/greenreg_socket.h>

/*
 * For interruption socket.
 */
#include "sc_irq.h"

#ifndef SC_DEVICE_H
#define SC_DEVICE_H

/*
 * For QEMU Device Creation.
 */
typedef struct {} BusState;
typedef struct {} DeviceState;

extern "C"
{
    /*
     * From qdev.h.
     */
    DeviceState *qdev_create(BusState *bus, const char *name);
    int qdev_init(DeviceState *dev);
}

/*
 * Abstract class for SystemC Device.
 * This can be MMIO, PCI,...
 * Basically it is used to get the GSParam and hold the MMIO or PCI specific
 * variable.
 */

class SCDevice:
    public gs::reg::gr_device
{
    public:
    SCDevice(sc_core::sc_module_name name, std::string deviceName,
             unsigned int nRegister = 0);
    virtual void parseGSParam() = 0;
    std::string getDeviceName();
    unsigned int getDeviceID();

    /*
     * Target port for the devices:
     * Usually connected on a greenrouter.
     */
	gs::reg::greenreg_socket< gs::gp::generic_slave> target_port;

	/*
     * Socket for interruption.
     */
	gs_generic_signal::initiator_signal_socket irq_socket;

    /*
     * Connect the device to a greenrouter.
     */
    virtual void connect(gs::gp::GenericRouter<32> *router) = 0;

    /*
     * Get the associated QEMU device.
     */
    DeviceState *getQEMUDevice();

    /*
     * Get the associated QEMU IRQ.
     */
    virtual qemu_irq getQEMUIRQ() = 0;
    protected:
    unsigned int deviceID;
    std::string deviceName;
    
    /*
     * The qemu device pointer is kept (for interrupt handling).
     */
    DeviceState *qemuDevice;

    /*
     * The QEMU irq.
     */
    qemu_irq qemuIRQ;
    private:
    static unsigned int deviceCounter;
};

#endif /* SC_DEVICE_H */
