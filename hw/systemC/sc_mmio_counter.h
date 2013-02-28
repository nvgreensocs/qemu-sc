/*
 * SC MMIO COUNTER
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

#ifndef SC_MMIO_COUNTER_H
#define SC_MMIO_COUNTER_H

#include <iostream>
#include <systemc>
#include "sc_mmio_device.h"

/*
 * This is needed for Slave Socket.
 */
#include "gsgpsocket/transport/GSGPSlaveSocket.h"

/*
 * This is needed for interruption.
 */
#include "sc_irq.h"

class SCMMIOCounter:

    /*
     * SCMMIODevice: Base class for MMIO SystemC device registered by QEMU.
     */
    public SCMMIODevice
{
    public:
    SC_HAS_PROCESS(SCMMIOCounter);
    SCMMIOCounter(uint64_t baseAddress, qemu_irq IRQ);

    /*
     * Info needed by QEMU.
     */
    static SCMMIOInfo deviceInfo;
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
     * IP variables.
     */

    /*
     * Counter value.
     */
    uint8_t counter;

    /*
     * IRQ Register.
     * 2^1: 0xFF -> 0x00 overflow occurs.
     * 2^2: 0x00 -> 0xFF overflow occurs.
     */
    uint8_t irqRegister;

    int IPmodel(accessHandle t);
    void updateIRQ();
};

#endif /* SC_MMIO_COUNTER_H */
