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
     *  -register QEMU devices.
     */

    /*
     * Register the devices.
     * This create the type, and not the real device.
     * It allows instantiating the device inside QEMU.
     */
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
    SCWrapper *wrapper = NULL;
    SCMMIOCounter *counterDevice = NULL;
    DBG("SystemC platform init.");

    /*
     * Greenrouter related:
     * Protocol, Scheduler and Router are created and linked together.
     */
    gs::gp::SimpleBusProtocol<32> *protocol = 
                           new gs::gp::SimpleBusProtocol<32>("protocol", 10);
    gs::gp::fixedPriorityScheduler *scheduler =
                           new gs::gp::fixedPriorityScheduler("scheduler");
    gs::gp::GenericRouter<32> *router =
                           new gs::gp::GenericRouter<32>("router");

    router->protocol_port(*protocol);
    protocol->router_port(*router);
    protocol->scheduler_port(*scheduler);

    /*
     * Create the wrapper: It's the link between SystemC backend and QEMU.
     */
    wrapper = new SCWrapper("wrapper", 100);

    /*
     * Create the devices.
     */
    counterDevice = new SCMMIOCounter(0x1c180000, SCWrapper::getQEMUIRQ(15));

    /*
     * Connect the SystemC bus (R/W from QEMU) to the wrapper.
     */
    wrapper->master_socket(router->target_socket);

    /*
     * Connect the SystemC bus (R/W from QEMU) to the devices.
     */
    counterDevice->connect(router);

    /*
     * Connect the devices IRQ to the wrapper socket.
     */
    wrapper->plugIRQ(counterDevice);

}
