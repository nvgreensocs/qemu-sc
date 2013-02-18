/*
 * SC PCI COUNTER
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

#ifndef SC_PCI_COUNTER_H
#define SC_PCI_COUNTER_H

#include <iostream>
#include <systemc>
#include "sc_pci_device.h"
#include "gsgpsocket/transport/GSGPSlaveSocket.h"

class SCPCICounter:
    /*
     * SCPCIDevice: Allow the device to be registered by QEMU.
     */
    public SCPCIDevice
{
    public:
    SC_HAS_PROCESS(SCPCICounter);
    SCPCICounter();

    /*
     * Info needed by QEMU.
     */
    static SCPCIInfo deviceInfo;
    static void registerQEMUDevice();

    /*
     * Transaction for GenericSlaveAccessHandle.
     */
    void b_transact(gs::gp::GenericSlaveAccessHandle gp);

    /*
     * Notification from payload_event_queue_output_if.
     */
    void notify(gs::gp::master_atom& tc);
    void notify(gs::gp::slave_atom& tc);

    /*
     * IP variables: here just the counter.
     */
    uint8_t counter;
    int IPmodel(accessHandle t);
};

#endif /* SC_PCI_COUNTER_H */
