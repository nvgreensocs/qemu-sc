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
    std::cout << "sc_wrapper.cpp.cc: error: " << str << std::endl;  \
}                                                                   \
while(0)

#include "sc_wrapper.h"
#include "sc_pci_device.h"
#include "sc_mmio_device.h"
#include <iostream>
#include <string>
#include <systemc>

/*
 * Greensocs library.
 */
#include "greenrouter/genericRouter.h"
#include "greenrouter/protocol/SimpleBus/simpleBusProtocol.h"
#include "greenrouter/scheduler/fixedPriorityScheduler.h"

/*
 * TLM-2.0 quantumkeeper.
 */
#include "tlm_utils/tlm_quantumkeeper.h"

extern "C" {
    /*
     * Implemented in vl.c: it is qemu entry_point.
     */
    int qemu_main(int argc, char **argv, char **envp);
}

/*
 * This is the program entry point when CONFIG_SYSTEM_WRAPPER is defined.
 * It must do configuration and then call qemu_main(..).
 */
int sc_main(int argc, char **argv)
{
    SCPCIDevice *test_pci;
    SCMMIODevice *test_mmio;
    std::cout<<std::endl;
    DBG("sc_wrapper started.");
    /*
     * XXX: What must be done here:
     *  -create qemu_device from systemC device.
     *  -create qemu platform.
     */
    test_pci = new SCPCIDevice("sc-pci-toto");
    test_mmio = new SCMMIODevice("sc-mmio-toto");
    test_pci->registerQEMUDevice();
    test_mmio->registerQEMUDevice();
    qemu_main(argc, argv, NULL);
    return 0;
}
