/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Marko Hyyppä
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
 *  @file       data_common.h
 *  @version    0.1
 *  @brief      This file contains header file of the internal definitions
 */

#ifndef DATA_COMMON_H
#define DATA_COMMON_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <min_common.h>

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */

/** BOOL type definition value macros
*/
#define TRUE    1
#define FALSE   0

/* ------------------------------------------------------------------------- */
/* DATA TYPES */

/** Boolean variable type definition as like "bool"
*/
typedef int     BOOL;

/** Title string type definition for Test Case title name string
*/
typedef char    title_string_t[MaxTestCaseName];

/** Result Description string type definition for Test Result data use
*/
typedef char    result_description_t[MaxTestResultDescription];

/** Printout string type definition for Test Result data use
*/
typedef char    test_printout_t[MaxUsrMessage];

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* None */


/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
#endif                          /* DATA_COMMON_H */
/* End of file */
