/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Marko Hyyppä
 * Contact e-mail: DG.MIN-Support@nokia.com
 * 
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License as published by the Free 
 * Software Foundation, version 2 of the License. 
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  Public License for
 * more details. You should have received a copy of the GNU General Public 
 * License along with this program. If not,  see 
 * <http://www.gnu.org/licenses/>.
 */


/**
 *  @file       scripter_dsa.h
 *  @version    0.1
 *  @brief      This file contains header file of the Scripter DSA
 */

#ifndef SCRIPTER_DSA_H
#define SCRIPTER_DSA_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <errno.h>
#include "min_common.h"
#include "dllist.h"

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
typedef struct _ScripterDataItem ScripterDataItem;

/** DLL type value enumerations
 */
typedef enum {
        EDLLTypeClass,
        EDLLTypeNormal
} TDLLType;

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/** Scripter Data Item structure.
 */
struct _ScripterDataItem {
        /** Pointer to DLL name string */
        TSChar         *DLL_name_;
        /** Pointer to class name string */
        TSChar         *Class_name_;
        /** Enumeration value of DLL type */
        TDLLType        DLL_type_;
        /** Linked list of symbol name string data */
        DLList         *symbol_list_;
};

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
DLListIterator  scripter_dsa_add (DLList * list_handle,
                                  ScripterDataItem * scripter_data);
/* ------------------------------------------------------------------------- */
ScripterDataItem *scripter_dsa_create (TSChar * DLL_name, TSChar * Class_name,
                                       TDLLType DLL_type);
/* ------------------------------------------------------------------------- */
void            scripter_dsa_free (ScripterDataItem ** scripter_data);
/* ------------------------------------------------------------------------- */
int             scripter_dsa_remove (DLListIterator scr_data_item);
/* ------------------------------------------------------------------------- */
int             scripter_dsa_add_symbol (DLListIterator scr_data_item,
                                         TSChar * symbol_name);
/* ------------------------------------------------------------------------- */

#endif                          /* SCRIPTER_DSA_H */
/* End of file */
