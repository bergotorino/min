/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Robert Galkowski
 * Contact e-mail: min-support@lists.sourceforge.net
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
 *  @file       check_tec.c
 *  @version    0.1
 *  @brief      test engine testsuites runner
 */

/* ----------------------------------------------------------------------------
 * INCLUDE FILES
 */
#include <stdlib.h>

/* ----------------------------------------------------------------------------
 * EXTERNAL DATA STRUCTURES
 * None
 */

/* --------------------------------------------------------------------------- 
 * EXTERNAL FUNCTION PROTOTYPES
 */

extern int      tec_tests ();
extern int      tec_events_tests ();

int slave_exit;

/* ----------------------------------------------------------------------------
 * CONSTANTS
 */

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
/* None */

/* ------------------------------------------------------------------------- 
 * FORWARD DECLARATIONS 
 */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
int main (int argc, char *argv[], char *envp[])
{
	int number_failed;
	
	number_failed = tec_tests (envp);
	number_failed += tec_events_tests ();

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
