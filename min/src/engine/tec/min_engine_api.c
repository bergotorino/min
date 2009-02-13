/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Sampo Saaristo
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
 *  @file       min_engine_api.c
 *  @version    0.1
 *  @brief      This file contains implementation of test execution 
 *              controller functionality
 */

/* ----------------------------------------------------------------------------
 * INCLUDE FILES
 */
#include <dllist.h>
#include <tec.h>
#include <min_logger.h>
#include <data_api.h>
#include <min_engine_api.h>

/* ----------------------------------------------------------------------------
 * GLOBAL VARIABLES
 */
DLList *modules;
eapiIn_t *in;

/* ----------------------------------------------------------------------------
 * EXTERNAL DATA STRUCTURES
 */


/* ---------------------------------------------------------------------------
 * EXTERNAL FUNCTION PROTOTYPES
 */
/* None */

/* ----------------------------------------------------------------------------
 * CONSTANTS
 */
/* None */

/* ----------------------------------------------------------------------------
 * MACROS
 */
/* None */


/* ----------------------------------------------------------------------------
 * LOCAL CONSTANTS AND MACROS
 */
/* None */

/* ----------------------------------------------------------------------------
 * MODULE DATA STRUCTURES
 */
/* None */

/* ----------------------------------------------------------------------------
 * LOCAL FUNCTION PROTOTYPES
 */

/* -------------------------------------------------------------------------
 * FORWARD DECLARATIONS
 */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */

/* ======================== FUNCTIONS ====================================== */

void eapi_add_test_module (char *modulepath)
{
        test_module_info_s *modinfo = INITPTR;

        modinfo = tm_create (modulepath, INITPTR, 0);
        if (tm_add (modules, modinfo) != INITPTR) {
                if (in->new_module) {
                        in->new_module (modulepath, modinfo->module_id_);
                }
        } else {
                MIN_WARN ("failed to add module");
                if (in->no_module) {
                        in->no_module (modulepath);
                }
                
        }
                
        return;
}

void eapi_add_test_case_file (unsigned module_id, char *testcasefile)
{
        DLListIterator it;
        test_module_info_s *modinfo;
        char *p;

        it = tm_find_by_module_id (modules, module_id);

        if (it == INITPTR) {
                MIN_WARN ("No module with id %u found", module_id);
                return;
        }
        modinfo = dl_list_data (it);
        if (testcasefile != NULL) {
                p =  NEW2 (char, strlen (testcasefile) + 1);
                STRCPY (p, testcasefile, strlen (testcasefile) + 1);
                dl_list_add (modinfo->cfg_filename_list_, p);
        } else {
                /* we have all the test case files for the module,
                   so we can add the module into engine */
                ec_add_module (modinfo->module_filename_, 
                               modinfo->cfg_filename_list_,
                               modinfo->module_id_);
                
        } 
}


void eapi_init (eapiIn_t *inp, eapiOut_t *out)
{
        
        in = inp;

        modules = dl_list_create();        
        out->add_test_module = eapi_add_test_module;
        out->add_test_case_file = eapi_add_test_case_file;
        out->run_test = NULL;
        out->fatal_error = NULL;

}

/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */


/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "min_engine_api.tests"
#endif                          /* MIN_UNIT_TEST */

/* End of file */
