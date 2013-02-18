/*
 * SC_COMMON
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

/*
 * These are common prototypes for systemc devices, and qemu backend.
 */

#ifndef SC_COMMON_H
#define SC_COMMON_H

/*
 * Defined in sc_wrapper.cpp.
 */
void sc_write(hwaddr addr, uint32_t value, uint32_t size);
uint32_t sc_read(hwaddr addr, uint32_t size);

#endif /* SC_COMMON_H */
