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
 *  @file       scripter_dsa.c
 *  @version    0.1
 *  @brief      This file contains implementation of the Scripter DSA
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include "scripter_dsa.h"

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* None */

/* ======================== FUNCTIONS ====================================== */

/** Adds Scripter Data Item to linked list of scripter data structures.
 *  @param list_handle pointer to linked list of Scripter Data Item.
 *  @param scripter_data pointer to Scripter Data Item structure.
 *  @return pointer to Scripter Data Item structure,
 *          or returns INITPTR if operation failed.  
 *
 *  Possible errors:
 *  INITPTR if Scripter Data Item adding to list failed.
 */
DLListIterator scripter_dsa_add (DLList * list_handle,
                                 ScripterDataItem * scripter_data)
{
        DLListIterator  dllist_item = INITPTR;

        if (list_handle != INITPTR) {
                dl_list_add (list_handle, (void *)scripter_data);
                dllist_item = dl_list_tail (list_handle);
        }

        return dllist_item;
}

/* ------------------------------------------------------------------------- */
/** Creates new Scripter Data Item structure
 *  @param DLL_name pointer to DLL name string.
 *  @param DLL_type enumeration value of DLL type.
 *  @return pointer to Scripter Data Item structure,
 *          or returns INITPTR if operation failed.  
 *
 *  Possible errors:
 *  INITPTR if Scripter Data Item creating to list failed.
 */
ScripterDataItem *scripter_dsa_create (TSChar * DLL_name, TSChar * Class_name,
                                       TDLLType DLL_type)
{
        ScripterDataItem *scripter_data = NEW (ScripterDataItem);
        TSBool          complete = ESFalse;
        int             len = 0;
        int             lenc = 0;

        if ((scripter_data != NULL) && (DLL_name != INITPTR)) {

                len = strlen (DLL_name) + 1;
                scripter_data->DLL_name_ = NEW2 (TSChar, len);
                lenc = strlen (Class_name) + 1;
                scripter_data->Class_name_ = NEW2 (TSChar, lenc);

                scripter_data->symbol_list_ = dl_list_create ();

                if ((scripter_data->DLL_name_ != NULL) &&
                    (scripter_data->symbol_list_ != INITPTR)) {
                        STRCPY (scripter_data->DLL_name_, DLL_name, len);
                        STRCPY (scripter_data->Class_name_, Class_name, lenc);
                        scripter_data->DLL_type_ = DLL_type;
                        complete = ESTrue;
                }
        }

        /* Something went incorrectly and creation failed */
        if (complete == ESFalse)
                scripter_data = INITPTR;

        return scripter_data;
}

/* ------------------------------------------------------------------------- */
/** Frees the memory allocation of Scripter Data Item structure.
 *  @param scripter_data reference pointer to Scripter Data Item.
 *
 *  Possible errors:
 *  If returns INITPTR value in scripter_data then operation failed.
 */
void scripter_dsa_free (ScripterDataItem ** scripter_data)
{
        DLListIterator it;
        char *symbol;

        if (*scripter_data != INITPTR) {
                DELETE ((*scripter_data)->DLL_name_);
                DELETE ((*scripter_data)->Class_name_);
                if ((*scripter_data)->symbol_list_ != INITPTR) {
                        for (it = dl_list_head ((*scripter_data)->symbol_list_);
                             it != DLListNULLIterator;
                             it = dl_list_next (it)) {
                                symbol = dl_list_data (it);
                                DELETE (symbol);
                        }
                        dl_list_free (&((*scripter_data)->symbol_list_));
                        

                }

                DELETE (*scripter_data);
                if (*scripter_data == NULL)
                        *scripter_data = INITPTR;
        }
}

/* ------------------------------------------------------------------------- */
/** Removes Scripter Data Item structure from linked list.
 *  @param scr_data_item pointer to list item in linked list.
 *  @return returns 0 error value is removing completed successfully,
 *          or returns error value if operation failed.  
 *
 *  Possible errors:
 *  -1 If removing operation failed.
 */
int scripter_dsa_remove (DLListIterator scr_data_item)
{
        int             retval = 0;
        ScripterDataItem *data;
        if (scr_data_item != INITPTR) {
                data = dl_list_data (scr_data_item);
                scripter_dsa_free (&data);
                retval = dl_list_remove_it (scr_data_item);
        } else {
                errno = EINVAL;
                retval = -1;
        }

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Adds symbol name data to Scripter Data Item structure.
 *  @param scr_data_item pointer to list item in linked list.
 *  @param symbol_name Pointer symbol name string.
 *  @return returns 0 error value when adding completed successfully,
 *          or returns error value if operation failed.  
 *
 *  Possible errors:
 *  -1 If adding operation failed.
 */
int scripter_dsa_add_symbol (DLListIterator scr_data_item,
                             TSChar * symbol_name)
{
        ScripterDataItem *scripter_data = INITPTR;
        int             retval = 0;

        if (scr_data_item != INITPTR) {
                scripter_data =
                    (ScripterDataItem *) dl_list_data (scr_data_item);

                if (scripter_data != INITPTR) {
                        retval = dl_list_add (scripter_data->symbol_list_,
                                              (void *)symbol_name);
                }
        } else {
                errno = EINVAL;
                retval = -1;
        }

        return retval;
}

/* ------------------------------------------------------------------------- */

/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "scripter_dsa.tests"
#endif                          /* MIN_UNIT_TEST */

/* End of file */
