/*
 * SYSTEMC PLATFORM
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

#include "sc_platform.h"
#include "sc_mmio_counter.h"
#include "sc_pci_counter.h"
#include "sc_wrapper.h"
#include <iostream>
#include <string>
#include <systemc>

/*
 * Greenrouter.
 */
#include "greenrouter/genericRouter.h"
#include "greenrouter/protocol/SimpleBus/simpleBusProtocol.h"
#include "greenrouter/scheduler/fixedPriorityScheduler.h"
#include "greenrouter/scheduler/dynamicPriorityScheduler.h"

/*
 * TLM-2.0 quantumkeeper.
 */
#include "tlm_utils/tlm_quantumkeeper.h"

/*
 * This is the program entry point when CONFIG_SYSTEM_WRAPPER is defined.
 * It must do configuration and then call qemu_main(..).
 */
int sc_main(int argc, char **argv)
{
        
    std::cout << std::endl;
    /*
     * What must be done here:
     *  -create register QEMU device.
     */

    /*
     * Create the devices.
     * This create the type, and not the real device.
     * It allows instantiating the device inside QEMU.
     */
    SCPCICounter::registerQEMUDevice();
    SCMMIOCounter::registerQEMUDevice();

    /*
     * Start QEMU, sc_platform_init will be called after the machine init.
     */
    qemu_main(argc, argv, NULL);

    /*
     * Starting simulation.
     */
    DBG("starting simulation.");
    sc_core::sc_start();
    return 0;
}

/*
 * This is the SystemC platform.
 * It is used to create device into QEMU.
 * And bind the device into SystemC world.
 */
void sc_platform_init(void)
{
    SCWrapper *pciWrapper = NULL;
    //SCPCICounter *counterDevice = NULL;
    SCMMIOCounter *anotherCounterDevice[2] = {NULL};
    DBG("SystemC platform init.");

    /*
     * Greenrouter related:
     * Protocol, Scheduler and Router are created and linked together.
     */
    gs::gp::SimpleBusProtocol<32> *pciProtocol = 
                           new gs::gp::SimpleBusProtocol<32>("pciProtocol", 10);
    gs::gp::fixedPriorityScheduler *pciScheduler =
                           new gs::gp::fixedPriorityScheduler("pciScheduler");
    gs::gp::GenericRouter<32> *pciRouter =
                           new gs::gp::GenericRouter<32>("pciRouter");

    pciRouter->protocol_port(*pciProtocol);
    pciProtocol->router_port(*pciRouter);
    pciProtocol->scheduler_port(*pciScheduler);

    /*
     * Create the wrapper: it's the PCI port from QEMU, the R/W come from it.
     */
    pciWrapper = new SCWrapper("pciWrapper", 1);

    /*
     * Create the devices.
     */
    //counterDevice = new SCPCICounter();
    anotherCounterDevice[0] = new SCMMIOCounter(0x1c180000);
    anotherCounterDevice[1] = new SCMMIOCounter(0x1c180100);

    /*
     * The PCI bus.
     */
    pciWrapper->pciPort(pciRouter->target_socket);
    //counterDevice->connect(pciRouter);
    anotherCounterDevice[0]->connect(pciRouter);
    anotherCounterDevice[1]->connect(pciRouter);

    /*
     * Direct Access to Memory (from device).
     */
    //counterDevice->mem_init_port(pciWrapper->memPort);
}
