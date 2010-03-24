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
 *  @file       data_test_result.h
 *  @version    0.1
 *  @brief      This file contains header file of the Test Result API
 */

#ifndef DATA_TEST_RESULT_H
#define DATA_TEST_RESULT_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */

#include <data_common.h>
#include <dllist.h>

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */

/** Test Result cause type enumarations
 */
typedef enum {
        TEST_RESULT_NOT_RUN,
        TEST_RESULT_PASSED,
        TEST_RESULT_FAILED,
        TEST_RESULT_CRASHED,
        TEST_RESULT_ABORTED,
        TEST_RESULT_TIMEOUT,
        TEST_RESULT_ALL
} test_result_type_t;

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */

/** Test Result printout data structure
*/
typedef struct {
    /** Printout priority */
        int             priority_;
    /** Pointer to printout text string */
        char           *printout_;
} test_result_printout_s;

/** Test Result data structure
*/
typedef struct {
    /** Pointer to Test Case data which processed Test Result */
        DLListIterator  tc_data_item_;
    /** Test Result description string */
        result_description_t result_description_;
    /** Start time of test sequence */
        long            start_time_;
    /** End time of test sequence */
        long            end_time_;
    /** Test Result cause type */
        int             result_type_;
    /** Test Result code (reserved for own reporting of Test Module code) */
        int             result_code_;
    /** Pointer to linked list of Test Result prinouts data */
        DLList         *printouts_list_;
} test_result_s;

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
DLListIterator  tr_add (DLList * list_handle, test_result_s * test_result);
/* ------------------------------------------------------------------------- */
test_result_s  *tr_create_data_item (DLListIterator tc_data_item);
/* ------------------------------------------------------------------------- */
void            tr_remove (DLListIterator tr_data_item);
/* ------------------------------------------------------------------------- */
void            tr_get_result_description (DLListIterator tr_data_item,
                                           result_description_t result_desc);
/* ------------------------------------------------------------------------- */
int             tr_get_result_type (DLListIterator tr_data_item);
/* ------------------------------------------------------------------------- */
int             tr_get_result_code (DLListIterator tr_data_item);
/* ------------------------------------------------------------------------- */
long            tr_get_start_time (DLListIterator tr_data_item);
/* ------------------------------------------------------------------------- */
long            tr_get_end_time (DLListIterator tr_data_item);
/* ------------------------------------------------------------------------- */
DLList         *tr_get_priontouts_list (DLListIterator tr_data_item);
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
void            tr_set_result_description (DLListIterator tr_data_item,
                                           result_description_t result_desc);
/* ------------------------------------------------------------------------- */
void            tr_set_result_type (DLListIterator tr_data_item,
                                    int result_type);
/* ------------------------------------------------------------------------- */
void            tr_set_result_code (DLListIterator tr_data_item,
                                    int result_code);
/* ------------------------------------------------------------------------- */
void            tr_set_start_time (DLListIterator tr_data_item,
                                   long start_time);
/* ------------------------------------------------------------------------- */
void            tr_set_end_time (DLListIterator tr_data_item, long end_time);
/* ------------------------------------------------------------------------- */
void            tr_set_priontouts_list (DLListIterator tr_data_item,
                                        DLList * printouts_list);
/* ------------------------------------------------------------------------- */
test_result_printout_s *tr_create_printout (int priority, char *printout);
/* ------------------------------------------------------------------------- */
void            tr_remove_printout (test_result_printout_s * tr_printout);
/* ------------------------------------------------------------------------- */
void            tr_remove_printouts_list (DLListIterator tr_data_item);
/* ------------------------------------------------------------------------- */
#endif                          /* DATA_TEST_RESULT_H */
/* End of file */
