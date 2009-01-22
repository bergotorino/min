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
 *  @file       check_minunit.c
 *  @version    0.1
 *  @brief      This file contains implementation of ...
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <stdlib.h>
#include <stdio.h>
#include <dllist.h>
#include <test_module_api.h>

#include <check.h>
#include "check_minunit.tests"
/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
extern int      tm_get_test_cases (const char *cfg_file, DLList ** cases);
extern int      tm_run_test_case (unsigned int id, const char *cfg_file,
                                  TestCaseResult * result);

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
int main (int argc, char *argv[])
{
        int             number_failed;
        Suite          *s = minunit_suite ();
        SRunner        *sr = srunner_create (s);
        srunner_run_all (sr, CK_NORMAL);
        number_failed = srunner_ntests_failed (sr);
        srunner_free (sr);
        return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


/* ======================== FUNCTIONS ====================================== */
/* None */

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */


/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */


/* End of file */
