/*
 * SC_WRAPPER
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

#ifndef SC_WRAPPER_H
#define SC_WRAPPER_H

/*
 * For hwaddr type.
 */
#include <stdint.h>
#include "exec/hwaddr.h"

/*
 * For sc_module.
 */
#include <systemc>

/*
 * For quantum keeper.
 */
#include "tlm_utils/tlm_quantumkeeper.h"

/*
 * For gsgpsocket.
 */
#include "gsgpsocket/transport/GSGPMasterBlockingSocket.h"
#include "gsgpsocket/transport/GSGPSlaveSocket.h"
#include "gsgpsocket/transport/GSGPconfig.h"

/*
 * For interrupt signal.
 */
#include "sc_irq.h"
#include "sc_device.h"

/*
 * Todo: find a better way to get this information.
 */
#define RUN_STATE_PAUSED 4

class SCWrapper: 
    public sc_core::sc_module,
    /*public gs::tlm_b_if<gs::gp::GenericSlaveAccessHandle>,*/
    public gs::payload_event_queue_output_if<gs::gp::master_atom>/*,
    public gs::payload_event_queue_output_if<gs::gp::slave_atom> */
{
    public:
    SC_HAS_PROCESS(SCWrapper);

    /*
     * Need sc_module_name and quantum in nanoseconds.
     */
    SCWrapper(sc_core::sc_module_name name, uint64_t quantum);

    /*
     * QBOX ports.
     */
    gs::gp::GenericMasterBlockingPort<32> master_socket;
    typedef gs::gp::GenericMasterBlockingPort<32>::accessHandle
                                                              transactionHandle;
    
    /*
     * IRQ Slave socket.
     */
    gs_generic_signal::target_signal_multi_socket<SCWrapper> irq_socket;
    typedef gs_generic_signal::gs_generic_signal_payload irqPayload;

    /*
     * Memory Port: TODO.
     */
    /*gs::gp::GenericSlavePort<32> memPort;
    gs::gp::GenericSlavePort<32>::accessHandle memPortTransaction;*/

    void run();

    /*
     * Transaction for GenericSlaveAccessHandle.
     */
    void b_transact(gs::gp::GenericSlaveAccessHandle gp);

    /*
     * Transaction for IRQ Socket.
     */
    void irq_b_transport(unsigned int port, irqPayload& payload,
                         sc_core::sc_time& time);

    /*
     * Notification from payload_event_queue_output_if.
     */
    void notify(gs::gp::master_atom& tc);
    void notify(gs::gp::slave_atom& tc);

    /*
     * Get the quantumKeeper.
     */
    static tlm_utils::tlm_quantumkeeper *getQuantumKeeper();

    /*
     * Get the quantum.
     */
    static uint64_t getQuantum();
    
    static void write(hwaddr addr, uint32_t value, uint32_t size);
    static uint32_t read(hwaddr addr, uint32_t size);

    /*
     * Get the default Wrapper.
     */
    static SCWrapper *getDefaultWrapper();

    /*
     * TODO.
     */
    void dmiWrite(hwaddr addr, uint32_t value, uint32_t size);
    uint32_t dmiRead(hwaddr addr, uint32_t size);

    void sc_write(hwaddr addr, uint32_t value, uint32_t len);
    uint32_t sc_read(hwaddr addr, uint32_t size);

    /*
     * IRQ is a little tricky, they must be exported from the QEMU platform with
     * exportIRQ(..), then the device can connect to one QEMU IRQ with
     * getQEMUIRQ(n).
     */

    /*
     * Get the IRQ number n.
     */
    static qemu_irq getQEMUIRQ(unsigned int n);

    /*
     * Export the IRQ.
     */
    static void exportIRQ(qemu_irq *pic, unsigned int count);

    /*
     * Plug a device into an IRQ, that's on the SystemC side.
     */
    void plugIRQ(SCDevice *scDevice);
    private:

    /*
     * A pointer to the wrapper used by QEMU.
     */
    static SCWrapper *defaultWrapper;

    /*
     * TLM Quantum Keeper used for synchronisation with the VM.
     */
    static tlm_utils::tlm_quantumkeeper quantumKeeper;

    /*
     * Quantum for the simulation.
     */
    static uint64_t quantum;

    /*
     * QEMU IRQ Lines.
     */
    static vector<qemu_irq> qemuExportedIRQ;

    /*
     * QEMU IRQ Tables.
     */
    static vector<qemu_irq> qemuIRQTable;
};

#endif /* SC_WRAPPER_H */
