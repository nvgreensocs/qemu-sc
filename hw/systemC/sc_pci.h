/*
 * SC_PCI
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

#ifndef SC_PCI_H
#define SC_PCI_H

#include "hw/hw.h"
#include "hw/pci/pci.h"
#include "sc_pci_common.h"

#define TYPE_SC_PCI "sc-pci"
#define SC_PCI(obj) OBJECT_CHECK(SCPCIState, (obj), TYPE_SC_PCI)
#define SC_PCI_GET_CLASS(obj) \
        OBJECT_GET_CLASS(SCPCIClass, (obj), TYPE_SC_PCI)
#define SC_PCI_CLASS(klass) \
        OBJECT_CLASS_CHECK(SCPCIClass, (klass), TYPE_SC_PCI)

typedef struct SCPCIState {
    PCIDevice parent_obj;
} SCPCIState;

#endif /* SC_PCI_H */
