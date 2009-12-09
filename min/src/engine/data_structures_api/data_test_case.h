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
 *  @file       data_test_case.h
 *  @version    0.1
 *  @brief      This file contains header file of the Test Case API
 */

#ifndef DATA_TEST_CASE_H
#define DATA_TEST_CASE_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */

#include <data_common.h>
#include <data_test_result.h>
#include <dllist.h>

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */

/** Test Case status value enumarations
 */
typedef enum {
        TEST_CASE_IDLE,
        TEST_CASE_ONGOING,
        TEST_CASE_PAUSED,
        TEST_CASE_TERMINATED,
        TEST_CASE_ALL
} test_case_status_t;

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */

/** Test Case data structure
*/
typedef struct {
    /** Test Case ID */
        int             tc_id_;
    /** Test Case run time ID */
        long            tc_run_id_;
    /** Test Case Id shared outside the engine */
        long            tc_ext_id_;
    /** Test Case group ID */
        int             tc_group_id_;
    /** Pointer to Test Module Info which includes this Test Case */
        DLListIterator  tm_data_item_;
    /** Test Case config filename string */
        filename_t      tc_cfg_filename_;
    /** Test Case title name string */
        title_string_t  title_;
    /** Test Case status value of running state */
        int             status_;
    /** Test Case priority value */
        int             priority_;
    /** Pointer to Test Result data of Test Case */
        DLList         *test_result_list_;
    /** Flag stating if this case should be started with debugger */
        int             debug_case_;
} test_case_s;

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
DLListIterator  tc_add (DLList * list_handle, test_case_s * tc_data);
/* ------------------------------------------------------------------------- */
test_case_s    *tc_create (DLListIterator tm_data_item,
                           filename_t tc_cfg_filename,
                           title_string_t tc_title);
/* ------------------------------------------------------------------------- */
DLListIterator  tc_find_by_id (DLList * list_handle,
			       int test_id);
/* ------------------------------------------------------------------------- */
void            tc_remove (DLListIterator tc_data_item);
/* ------------------------------------------------------------------------- */
void            tc_delete (test_case_s * test_case);
/* ------------------------------------------------------------------------- */
int             tc_get_id (DLListIterator tc_data_item);
/* ------------------------------------------------------------------------- */
int             tc_get_ext_id (DLListIterator tc_data_item);
/* ------------------------------------------------------------------------- */
long            tc_get_run_id (DLListIterator tc_data_item);
/* ------------------------------------------------------------------------- */
int             tc_get_group_id (DLListIterator tc_data_item);
/* ------------------------------------------------------------------------- */
int             tc_get_title (DLListIterator tc_data_item,
                              title_string_t title_string);
/* ------------------------------------------------------------------------- */
void            tc_get_cfg_filename (DLListIterator tc_data_item,
                                     filename_t tc_cfg_filename);
/* ------------------------------------------------------------------------- */
int             tc_get_priority (DLListIterator tc_data_item);
/* ------------------------------------------------------------------------- */
DLList         *tc_get_tr_list (DLListIterator tc_data_item);
/* ------------------------------------------------------------------------- */
int             tc_get_status (DLListIterator tc_data_item);
/* ------------------------------------------------------------------------- */
DLListIterator  tc_get_test_module_ptr (DLListIterator tc_data_item);
/* ------------------------------------------------------------------------- */
void            tc_set_id (DLListIterator tc_data_item, int test_case_id);
/* ------------------------------------------------------------------------- */
void            tc_set_run_id (DLListIterator tc_data_item, long run_id);
/* ------------------------------------------------------------------------- */
void            tc_set_group_id (DLListIterator tc_data_item, int group_id);
/* ------------------------------------------------------------------------- */
void            tc_set_status (DLListIterator item_data, int status);
/* ------------------------------------------------------------------------- */
void            tc_set_cfg_filename (DLListIterator tc_data_item,
                                     filename_t tc_cfg_filename);
/* ------------------------------------------------------------------------- */
void            tc_set_priority (DLListIterator tc_data_item, int priority);
/* ------------------------------------------------------------------------- */
void            tc_set_tr_list (DLListIterator tc_data_item,
                                DLList * test_result_list);
/* ------------------------------------------------------------------------- */
void            tc_set_test_module_ptr (DLListIterator tc_data_item,
                                        DLListIterator tm_data_item);
/* ------------------------------------------------------------------------- */
#endif                          /* DATA_TEST_CASE_H */
/* End of file */
