/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Konrad Marek Zapalowicz
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
 *  @file       TestClass.c
 *  @version    0.1
 *  @brief      This file contains implementation of the TestClass
 *              test module of MIN Test Framework.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include "TestClass.h"

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
extern char *module_date;
extern char *module_time;
/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
TTestModuleType module_type     = ETestClass;
unsigned int    module_version  = 200830;
/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
/** Used for finding callname.  */
int             _look4callname (const void *a, const void *b);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
int _look4callname (const void *a, const void *b)
{
        TestCaseInfoTC *tci = (TestCaseInfoTC *) a;
        return strcmp (tci->name_, (char *)b);
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
int ts_run_method (MinItemParser * item)
{
        DLList         *l;
        DLListIterator  it;
        int             retval;
        char           *callname;

        l = dl_list_create ();
        callname = INITPTR;

        /*
         * Take callname  
         */
        retval = mip_get_next_string (item, &callname);
        if (retval != ENOERR) {
                retval = -1;
                goto EXIT;
        }

        /*
         * Obtain list of test cases 
         */
        ts_get_test_cases (&l);

        /*
         * Look for test with name set to callname 
         */
        it = dl_list_find (dl_list_head (l)
                           , dl_list_tail (l)
                           , _look4callname, callname);

        if (it == DLListNULLIterator) {
                retval = -1;
                DELETE (callname);
                it = dl_list_head (l);
                while (it != DLListNULLIterator) {
                        free (dl_list_data (it));
                        dl_list_remove_it (it);
                        it = dl_list_head (l);
                }
                dl_list_free (&l);
                goto EXIT;
        }

        /*
         * Call function 
         */
        retval = ((TestCaseInfoTC *) dl_list_data (it))->test_ (item);

        /*
         * Cleanup 
         */
        DELETE (callname);
        it = dl_list_head (l);
        while (it != DLListNULLIterator) {
                free (dl_list_data (it));
                dl_list_remove_it (it);
                it = dl_list_head (l);
        }
        dl_list_free (&l);
      EXIT:
        return retval;
}
/* ------------------------------------------------------------------------- */
unsigned int get_module_type()
{ return module_type; }
/* ------------------------------------------------------------------------- */
unsigned int get_module_version()
{ return module_version; }
/* ------------------------------------------------------------------------- */
char* get_module_date()
{ return module_date; }
/* ------------------------------------------------------------------------- */
char* get_module_time()
{ return module_time; }
/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
