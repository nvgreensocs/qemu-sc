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
    std::cout << "sc_mmio_counter.cpp: debug: " << str << std::endl;    \
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
                 &SCMMIOCounter::deviceInfo, baseAddress, IRQ)
{
    DBG("sc-mmio-counter created.");

    /*
     * Init the variable.
     */
    counter = 0xFC;
    irqRegister = 0x00;
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
 * Transaction for GenericSlavePort.
 */
void SCMMIOCounter::b_transact(gs::gp::GenericSlaveAccessHandle t)
{
    /*
     * We don't care about timing.
     */
	IPmodel(_getSlaveAccessHandle(t));
}

void SCMMIOCounter::notify(gs::gp::master_atom& tc)
{
	DBG("Master notify.");
}

void SCMMIOCounter::notify(gs::gp::slave_atom& tc)
{
	DBG("Slave notify.");
}

/*
 * IPModeling.
 */
int SCMMIOCounter::IPmodel(accessHandle t)
{
    /*
     * Data for the transaction.
     */
	gs::GSDataType data;
	data.set(t->getMData());

    /*
     * Actual address of the transaction.
     */
    uint32_t offset = t->getMAddr() - target_port.base_addr;

    /*
     * Return value.
     */
    uint32_t readValue = 0;
    
    if (t->getMCmd() == gs::Generic_MCMD_RD)
    {
        DBG("Attempt to read @" << offset);

        /*
         * Just send the counter value in case of read @0x00.
         */
        switch (offset)
        {
            case 0x00:
                /*
                 * Read the counter value.
                 */
                readValue = counter;
            break;
            case 0x03:
                /*
                 * Read the IRQ Register.
                 */
                readValue = irqRegister;
            break;
            default:
                /*
                 * Should not happen.
                 */
                readValue = 0x00;
            break;
        }
        memcpy(data.getPointer(), &readValue, sizeof(uint8_t));
    }
    else if (t->getMCmd() == gs::Generic_MCMD_WR)
    {
        DBG("Attempt to write @" << offset);

        /*
         * Just increment counter in case of write @0x01, and reset it in
         * case of write @0x02.
         */
        switch (offset)
        {
            case 0x00:
                counter++;
                if (counter == 0x00)
                {
                    /*
                     * When counter pass from 0xFF to 0x00.
                     * Throw an interrupt.
                     */
                    irqRegister |= 0x01;
                    updateIRQ();
                }
            break;
            case 0x01:
                counter--;
                if (counter == 0xFF)
                {
                    /*
                     * When counter pass from 0x00 to 0xFF.
                     * Throw an interrupt.
                     */
                    irqRegister |= 0x02;
                    updateIRQ();
                }
            break;
            case 0x02:
                counter = 0;
                irqRegister = 0x00;
                updateIRQ();
            break;
            case 0x03:
                /*
                 * Clearing the interruption.
                 */
                irqRegister = 0x00;
                updateIRQ();
            default:
                /*
                 * Nothing.
                 */
            break;
        }
    }
    else
    {
        DBG("???");
    }
    return 0;
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
