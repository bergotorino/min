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
 *  @file       treport_log.h
 *  @version    0.1
 *  @brief      This file contains header file of the MIN test report logger
 */

#ifndef TEST_REPORT_LOGGER_H
#define TEST_REPORT_LOGGER_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <dllist.h>

/* ------------------------------------------------------------------------- */
/* CONSTANTS */

/* ------------------------------------------------------------------------- */
/* MACROS */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */

/* ------------------------------------------------------------------------- */
/* Structures */

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
int             min_test_report_begin ();
/* ------------------------------------------------------------------------- */
int             min_test_report_begin_set (const char *set_name);
/* ------------------------------------------------------------------------- */
int             min_test_report_result (DLListIterator tc,
                                         const char *fname);

#endif                          /* TEST_REPORT_LOGGER_H */

/* End of file */
