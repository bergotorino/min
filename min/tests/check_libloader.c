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
 *  @file       check_libloader.c
 *  @version    0.1
 *  @brief      This file contains main() lib loader util tests
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <stdlib.h>
#include <check.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
extern Suite *tllib_suite();

/* =============================== TEST MAIN =============================== */

int main (void)
{
        int             number_failed;
        Suite          *s = tllib_suite ();
        SRunner        *sr = srunner_create (s);
        srunner_set_fork_status (sr,CK_NOFORK);
        srunner_run_all (sr, CK_NORMAL);
        number_failed = srunner_ntests_failed (sr);
        srunner_free (sr);
        return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

/* ========================================================================= */
/* End of file */
