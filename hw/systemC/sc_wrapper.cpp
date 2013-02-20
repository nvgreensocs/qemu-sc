/*
 * SYSTEMC WRAPPER
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

#include "sc_wrapper.h"
#include "sc_pci_device.h"
#include "sc_mmio_device.h"
#include <iostream>
#include <string>
#include <systemc>

extern "C"
{
    #include "sc_common.h"
    int64_t sc_simulation(int64_t qemu_time);
    int qemu_loop(void);
    /*
     * in sysemu.h
     */
    void vm_stop(int);
    void vm_start();
}

/*
 * Greensocs library.
 */
#include "gsgpsocket/transport/GSGPMasterBlockingSocket.h"
#include "greenrouter/genericRouter.h"
#include "greenrouter/protocol/SimpleBus/simpleBusProtocol.h"
#include "greenrouter/scheduler/fixedPriorityScheduler.h"

/*
 * TLM-2.0 quantumkeeper.
 */
#include "tlm_utils/tlm_quantumkeeper.h"

#define DEBUG

#ifdef DEBUG
#define DBG(str)                                                    \
do                                                                  \
{                                                                   \
    std::cout << "sc_wrapper.cpp: debug: " << str << std::endl;     \
}                                                                   \
while(0)
#else
#define DBG(str) do { } while (0)
#endif

#define WRN(str)                                                    \
do                                                                  \
{                                                                   \
    std::cout << "sc_wrapper.cpp: warning: " << str << std::endl;   \
}                                                                   \
while(0)

#define ERR(str)                                                    \
do                                                                  \
{                                                                   \
    std::cout << "sc_wrapper.cpp: error: " << str << std::endl;     \
}                                                                   \
while(0)

/*
 * This static variable is the wrapper used by QEMU to read/write.
 */
SCWrapper *SCWrapper::defaultWrapper = NULL;

/*
 * This static variable is the quantum keeper for the simulation.
 */
tlm_utils::tlm_quantumkeeper SCWrapper::quantumKeeper;

/*
 * This static variable is the quantum of the simulation in nanoseconds.
 */
uint64_t SCWrapper::quantum = 0;

SCWrapper::SCWrapper(sc_core::sc_module_name name, uint64_t quantum):
    pciPort("iport")
    //memPort("memPort")
{
    /*
     * For port configuration.
     */
    //gs::gp::GSGPSocketConfig cnf;

    DBG(name << " created.");
    if (defaultWrapper == NULL)
    {
        defaultWrapper = this;
    }
    else
    {
        ERR("Only one wrapper can be created!");
        return;
    }
    pciPort.out_port(*this);
    SC_THREAD(run);

    /*
     * Configure the pciPort.
     */
    /*cnf.use_wr_resp = false;
    pciPort.set_config(cnf);*/

    /*
     * Set the quantum.
     */
    SCWrapper::quantum = quantum;
}

void SCWrapper::run()
{
    DBG("Starting VM.");
    
    /*
     * This just start the VM, it is defined in vl.c.
     */
    qemu_loop();
    DBG("VM exited.");
}

/*
 * This is used for the synchronization mechanism.
 * It is trigged by sc_timer in vl.c.
 * It must return the next time of trigerring.
 */
int64_t sc_simulation(int64_t qemu_time)
{
    /*
     * Time to catch.
     */
    sc_core::sc_time late = sc_core::sc_time((double)qemu_time, sc_core::SC_NS)
                            - sc_core::sc_time_stamp();

    /*
     * Next trigger.
     */
    sc_core::sc_time next = sc_core::sc_time((double)qemu_time, sc_core::SC_NS)
                            + sc_core::sc_time((double)SCWrapper::getQuantum(),
                                               sc_core::SC_NS);

    /*
     * Quantum Keeper.
     */
    tlm_utils::tlm_quantumkeeper *quantumKeeper = SCWrapper::getQuantumKeeper();

    /*
     * Catch the time with QEMU VM.
     */
    /*DBG("QEMU TIME:    " << qemu_time);
    DBG("SYSTEMC TIME: " << sc_core::sc_time_stamp());
    DBG("Setting quantum keeper to: " << late);*/

	quantumKeeper->set(late);

	if (quantumKeeper->need_sync())
    {
		vm_stop(RUN_STATE_PAUSED);
		quantumKeeper->sync();
		vm_start();
	}

    /*DBG("NEXT SYNCHRO: " << next.value());*/
	return next.value() / 1000;
}

/*
 * Get the quantum keeper.
 */
tlm_utils::tlm_quantumkeeper *SCWrapper::getQuantumKeeper()
{
    return &SCWrapper::quantumKeeper;
}

/*
 * Get the quantum.
 */
uint64_t SCWrapper::getQuantum()
{
    return SCWrapper::quantum;
}

void SCWrapper::sc_write(hwaddr addr, uint32_t value, uint32_t size)
{
    DBG("attempt to write value " << value << " @" << addr);
    uint32_t aux = value;
	gs::GSDataType data;
    transactionHandle tHandle;

	data.setData(gs::GSDataType::dtype(sizeof(uint32_t), value));

    /*
     * Make the transaction.
     */
	tHandle = pciPort.create_transaction();
	tHandle->setMCmd(gs::Generic_MCMD_WR);
	tHandle->setMData(data);
	tHandle->setMAddr(addr);
	tHandle->setMBurstLength(size);
	pciPort.Transact(tHandle);

    wait(sc_core::SC_ZERO_TIME);
}

uint32_t SCWrapper::sc_read(hwaddr addr, uint32_t size)
{
    DBG("attempt to read @" << addr);
	gs::GSDataType data;
    transactionHandle tHandle;
	uint32_t aux = 0;

	data.setPointer(&aux, sizeof(uint32_t));

    /*
     * Make the transaction.
     */
    tHandle = pciPort.create_transaction();
    tHandle->setMCmd(gs::Generic_MCMD_RD);
    tHandle->setMAddr(addr);
    tHandle->setMBurstLength(size);
    tHandle->setMData(data);
    pciPort.Transact(tHandle);

	wait(sc_core::SC_ZERO_TIME);
	aux = *(uint32_t*) (data.getPointer());

	return aux;
}

/*
 * Those one are called by QEMU.
 */
void sc_write(hwaddr addr, uint32_t value, uint32_t size)
{
    SCWrapper::write(addr, value, size);
}

uint32_t sc_read(hwaddr addr, uint32_t size)
{
    SCWrapper::read(addr, size);
}

/*
 * Read / Write to memory.
 */
void SCWrapper::write(hwaddr addr, uint32_t value, uint32_t size)
{
    defaultWrapper->sc_write(addr, value, size);
}

uint32_t SCWrapper::read(hwaddr addr, uint32_t size)
{
	return defaultWrapper->sc_read(addr, size);
}

/*
 * Transaction for GenericSlavePort.
 */
void SCWrapper::b_transact(gs::gp::GenericSlaveAccessHandle gp)
{
    /*
     * Look at it after.
     */
    DBG("b_transact");
}

/*
 * Notification on devicePort.
 */
void SCWrapper::notify(gs::gp::master_atom& tc)
{
	DBG("Master notify.");
}

/*
 * Notification on memoryPort.
 */
void SCWrapper::notify(gs::gp::slave_atom& tc)
{
	DBG("Slave notify.");
}

