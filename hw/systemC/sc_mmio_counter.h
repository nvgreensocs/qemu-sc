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
#include "greencontrol/config.h"

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
    GC_HAS_CALLBACKS();
    SC_HAS_PROCESS(SCMMIOCounter);
    SCMMIOCounter(uint64_t baseAddress, qemu_irq IRQ);
    ~SCMMIOCounter();

    /*
     * Info needed by QEMU.
     */
    static SCMMIOInfo deviceInfo;
    static void registerQEMUDevice();
    
    /*
     * Needed by GreenReg.
     */
    void end_of_elaboration();

    /*
     * IP Modeling.
     */
    
    /*
     * GreenSocs Parameters.
     */
    gs::gs_param<unsigned char> counter;
    gs::gs_param<unsigned char> irqRegister;

    /*
     * Callbacks.
     */
    void incrementCounter();
    void preReadCounter();
    void decrementCounter();
    void resetCounter();
    void clearIRQ();
    void updateIRQ();
};

#endif /* SC_MMIO_COUNTER_H */
