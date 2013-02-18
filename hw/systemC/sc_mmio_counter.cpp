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

SCMMIOInfo SCMMIOCounter::deviceInfo = {"sc-mmio-counter", 0x100};

SCMMIOCounter::SCMMIOCounter(uint64_t baseAddress):
    /*
     * We must give the name of the device registered by QEMU.
     */
    SCMMIODevice("sc-mmio-counter", "sc-mmio-counter",
                 &SCMMIOCounter::deviceInfo, baseAddress)
{
    DBG("sc-mmio-counter created.");

    /*
     * Init the variable.
     */
    counter = 0x55;
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
        if (offset == 0x00)
        {
            readValue = counter;
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
            break;
            case 0x01:
                counter--;
            break;
            case 0x02:
                counter = 0;
            break;
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
