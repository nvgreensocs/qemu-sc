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

#include "sc_mmio_counter.h"

#define DEBUG

#ifdef DEBUG
#define DBG(str)                                                       \
do                                                                     \
{                                                                      \
    std::cout << "sc_mmio_counter.cpp: debug: " << str << std::endl;   \
}                                                                      \
while(0)
#else
#define DBG(str) do { } while (0)
#endif

#define WRN(str)                                                       \
do                                                                     \
{                                                                      \
    std::cout << "sc_mmio_counter.cpp: warning: " << str << std::endl;  \
}                                                                      \
while(0)

#define ERR(str)                                                       \
do                                                                     \
{                                                                      \
    std::cout << "sc_mmio_counter.cpp.cc: error: " << str << std::endl; \
}                                                                      \
while(0)

/*
 * This information must be provided for registrating the device into QEMU.
 * The SCMMIOInfo structure is defined in sc_mmio_comon.h
 */
SCMMIOInfo SCMMIOCounter::deviceInfo = {"sc-mmio-counter", 0x100};

SCMMIOCounter::SCMMIOCounter(uint64_t baseAddress, qemu_irq IRQ):

    /*
     * We must give the name of the device registered by QEMU.
     */
    SCMMIODevice("sc-mmio-counter", "sc-mmio-counter",
                 &SCMMIOCounter::deviceInfo, baseAddress, IRQ, 4),
    /*
     * Init the gs_param.
     */
    counter("counter", (unsigned char)(0xFC)),
    irqRegister("irq register", (unsigned char)(0x00))
{
    DBG("sc-mmio-counter created.");

    /*
     * GreenReg.
     */
    r.create_register("Reg0",
                      "Increment or get value",
                      0x00,
                      gs::reg::STANDARD_REG
                      | gs::reg::SINGLE_IO
                      | gs::reg::SINGLE_BUFFER
                      | gs::reg::FULL_WIDTH,
                      0x00,
                      0x00,
                      8,
                      0x00);
    r.create_register("Reg1",
                      "Decrement",
                      0x01,
                      gs::reg::STANDARD_REG
                      | gs::reg::SINGLE_IO
                      | gs::reg::SINGLE_BUFFER
                      | gs::reg::FULL_WIDTH,
                      0x00,
                      0x00,
                      8,
                      0x00);
    r.create_register("Reg2",
                      "Reset",
                      0x02,
                      gs::reg::STANDARD_REG
                      | gs::reg::SINGLE_IO
                      | gs::reg::SINGLE_BUFFER
                      | gs::reg::FULL_WIDTH,
                      0x00,
                      0x00,
                      8,
                      0x00);
    r.create_register("Reg3",
                      "Interrupt register",
                      0x03,
                      gs::reg::STANDARD_REG
                      | gs::reg::SINGLE_IO
                      | gs::reg::SINGLE_BUFFER
                      | gs::reg::FULL_WIDTH,
                      0x00,
                      0x00,
                      8,
                      0x00);
}

SCMMIOCounter::~SCMMIOCounter()
{
    GC_UNREGISTER_CALLBACKS();
}

/*
 * Called by GreenReg at the end of elaboration.
 */
void SCMMIOCounter::end_of_elaboration()
{
    /*
     * Define the callback for each register.
     */
    GR_FUNCTION(SCMMIOCounter, incrementCounter);
    GR_SENSITIVE(r[0x00].add_rule(gs::reg::PRE_WRITE,
                                   "increment counter",
                                   gs::reg::NOTIFY));
    GR_FUNCTION(SCMMIOCounter, preReadCounter);
    GR_SENSITIVE(r[0x00].add_rule(gs::reg::PRE_READ,
                                   "read counter",
                                   gs::reg::NOTIFY));
    GR_FUNCTION(SCMMIOCounter, decrementCounter);
    GR_SENSITIVE(r[0x01].add_rule(gs::reg::POST_WRITE,
                                   "decrement counter",
                                   gs::reg::NOTIFY));
    GR_FUNCTION(SCMMIOCounter, resetCounter);
    GR_SENSITIVE(r[0x02].add_rule(gs::reg::POST_WRITE,
                                   "reset counter",
                                   gs::reg::NOTIFY));
    GR_FUNCTION(SCMMIOCounter, clearIRQ);
    GR_SENSITIVE(r[0x03].add_rule(gs::reg::POST_WRITE,
                                   "clear interrupt register",
                                   gs::reg::NOTIFY));
}

void SCMMIOCounter::registerQEMUDevice()
{
    /*
     * not many to do:
     * devices must be registered before QEMU start.
     */
    devicesToBeRegistered.push_back(deviceInfo);
}

/*
 * IPModeling.
 */

void SCMMIOCounter::incrementCounter()
{
    /*
     * We don't care what is written to this register..
     */
    counter = counter + 1;

    if (counter == 0x00)
    {
        /*
         * When counter pass from 0xFF to 0x00.
         * Throw an interrupt.
         */
        irqRegister |= 0x01;
        updateIRQ();
    }
}

void SCMMIOCounter::preReadCounter()
{
    /*
     * Fill the buffer with the counter value.
     */
    r[0x00] = counter;
}

void SCMMIOCounter::decrementCounter()
{
    counter -= 1;

    if (counter == 0xFF)
    {
        /*
         * When counter pass from 0x00 to 0xFF.
         * Throw an interrupt.
         */
        irqRegister |= 0x02;
        updateIRQ();
    }
}

void SCMMIOCounter::resetCounter()
{
    /*
     * Reset the IP.
     */
    counter = 0;
    irqRegister = 0;
    updateIRQ();
}

void SCMMIOCounter::clearIRQ()
{
    /*
     * Clearing the interruption.
     */
    irqRegister = 0x00;
    updateIRQ();
}

void SCMMIOCounter::updateIRQ()
{
    bool irqData = false;
    bool ackRequirement = false;
    sc_core::sc_time time = sc_core::SC_ZERO_TIME;
    gs_generic_signal::gs_generic_signal_payload* payload;

    /*
     * If there is a flag set, the level is high.
     */
    if (irqRegister != 0x00)
    {
        irqData = true;
    }
    payload = irq_socket.get_transaction();
    payload->set_data_ptr((unsigned char*) &irqData);
    payload->set_ack_requirement(ackRequirement);
    irq_socket.validate_extension<IRQ>(*payload);
    irq_socket->b_transport(*payload, time);
    irq_socket.release_transaction(payload);    
}
