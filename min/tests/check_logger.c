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
 *  @file       check_logger.c
 *  @version    0.1
 *  @brief      This file contains implementation of tests for Logger Mechanism
 */

/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <check.h>
#include "min_logger.h"
#include "check_logger.tests"

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
extern int      min_logger_file_output_tests ();
extern int      min_logger_tests ();

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
/* None */

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
/* None */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */

int main (int argc, char *argv[])
{
        int             number_failed = 0;
        Suite          *s = logger_suite ();
        SRunner        *sr = srunner_create (s);

        srunner_run_all (sr, CK_NORMAL);
        number_failed += srunner_ntests_failed (sr);
        srunner_free (sr);

        s = min_logger_file_output_suite ();
        sr = srunner_create (s);

        srunner_run_all (sr, CK_NORMAL);
        number_failed += srunner_ntests_failed (sr);
        srunner_free (sr);

        s = min_logger_suite ();
        sr = srunner_create (s);
        srunner_run_all (sr, CK_NORMAL);
        number_failed += srunner_ntests_failed (sr);
        srunner_free (sr);

        return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}



/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* End of file */
