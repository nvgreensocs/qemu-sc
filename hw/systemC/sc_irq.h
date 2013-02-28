/*
 * sc_irq.h
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
 * This is needed for interruption.
 */
#include "greensignalsocket/green_signal.h"

#ifndef SC_IRQ_H
#define SC_IRQ_H

GS_GUARD_ONLY_EXTENSION(IRQ);

extern "C"
{
    /*
     * From irq.h.
     */
    typedef struct IRQState *qemu_irq;
    void qemu_set_irq(qemu_irq irq, int level);
}

#endif /* SC_IRQ_H */
