/*
 * SC PLATFORM
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

#ifndef SC_PLATFORM_H
#define SC_PLATFORM_H

#define DEBUG

#ifdef DEBUG
#define DBG(str)                                                    \
do                                                                  \
{                                                                   \
    std::cout << "sc_platform.cpp: debug: " << str << std::endl;    \
}                                                                   \
while(0)
#else
#define DBG(str) do { } while (0)
#endif

#define WRN(str)                                                    \
do                                                                  \
{                                                                   \
    std::cout << "sc_platform.cpp: warning: " << str << std::endl;  \
}                                                                   \
while(0)

#define ERR(str)                                                    \
do                                                                  \
{                                                                   \
    std::cout << "sc_platform.cpp.cc: error: " << str << std::endl; \
}                                                                   \
while(0)

extern "C"
{
    /*
     * Implemented in vl.c: it is qemu entry_point.
     */
    int qemu_main(int argc, char **argv, char **envp);
    /*
     * Called in vl.c after machine creation.
     */
    void sc_platform_init(void);
}

#endif /* SC_PLATFORM_H */
