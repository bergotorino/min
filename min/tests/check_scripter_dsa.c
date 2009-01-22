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
 *  @file       check_scripter_dsa.c
 *  @version    0.1
 *  @brief      This file contains main() for MIN Scripter DSA
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <stdlib.h>
#include <scripter_dsa.tests>

/* =============================== TEST MAIN =============================== */

int main (int argc, char *argv[])
{
	int number_failed;

        number_failed = scripter_dsa_tests ();

        return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

/* ========================================================================= */
/* End of file */
