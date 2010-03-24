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
 *  @file       data_test_module_info.h
 *  @version    0.1
 *  @brief      This file contains header file of the Test Module Info API
 */

#ifndef DATA_TEST_MODULE_INFO_H
#define DATA_TEST_MODULE_INFO_H

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
/** unique identifier for the test module */
typedef unsigned int test_module_id_t;

/** Test Module Info status value enumerations
 */
typedef enum {
        TEST_MODULE_NOT_STARTED,
        TEST_MODULE_INITIALIZED,
        TEST_MODULE_INITIALIZED_TEMP,
        TEST_MODULE_TC_SENDING,
        TEST_MODULE_READY,
        TEST_MODULE_BUSY,
        TEST_MODULE_BUSY_TEMP
} test_module_status_t;

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */

/** Test Summary data structure
 */
typedef struct {
        int             passed_;
        int             failed_;
        int             crashed_;
        int             aborted_;
        int             timeout_;
} test_summary_s;

/** Test Module Info data structure
 */
typedef struct {
    /** Module identifier */
        test_module_id_t  module_id_;
    /** Status value of Test Module Info running state */
        test_module_status_t status_;
    /** Filename of Test Module Info */
        filename_t      module_filename_;
    /** Pointer to config filename list of Test Module Info */
        DLList         *cfg_filename_list_;
    /** Test Module Info process ID */
        long            process_id_;
    /** Pointer to Test Case list of Test Module Info */
        DLList         *test_case_list_;
    /** Pointer to Test Summary data */
        test_summary_s *test_summary_;
} test_module_info_s;

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
DLListIterator  tm_add (DLList * list_handle, test_module_info_s * tm_data);
/* ------------------------------------------------------------------------- */
DLListIterator  tm_find_by_module_id (DLList * list_handle, 
                                      test_module_id_t id);
/* ------------------------------------------------------------------------- */
test_module_info_s *tm_create (filename_t tm_filename,
                               DLList * cfg_filename_list,
                               test_module_id_t id);
/* ------------------------------------------------------------------------- */
void            tm_remove (DLListIterator tm_data_item);
/* ------------------------------------------------------------------------- */
void            tm_delete (test_module_info_s * test_module);
/* ------------------------------------------------------------------------- */
long            tm_get_pid (DLListIterator item_tm_data);
/* ------------------------------------------------------------------------- */
DLList         *tm_get_tclist (DLListIterator item_tm_data);
/* ------------------------------------------------------------------------- */
DLListIterator  tm_get_ptr_by_pid (DLList * list_handle, long pid);
/* ------------------------------------------------------------------------- */
int             tm_get_status (DLListIterator tm_data_item);
/* ------------------------------------------------------------------------- */
DLList         *tm_get_cfg_filenames (DLListIterator tm_data_item);
/* ------------------------------------------------------------------------- */
void            tm_get_module_filename (DLListIterator tm_data_item,
                                        filename_t tm_filename);
/* ------------------------------------------------------------------------- */
test_summary_s *tm_get_test_summary_data (DLListIterator tm_data_item);
/* ------------------------------------------------------------------------- */
int             tm_get_test_summary_detail (DLListIterator tm_data_item,
					    int ts_type);
/* ------------------------------------------------------------------------- */
test_module_id_t tm_get_module_id (DLListIterator item_tm_data);
/* ------------------------------------------------------------------------- */
int             tm_create_test_summary (DLListIterator tm_data_item);
/* ------------------------------------------------------------------------- */
void            tm_set_pid (DLListIterator item_tm_data, long tm_pid);
/* ------------------------------------------------------------------------- */
void            tm_set_tclist (DLListIterator item_tm_data,
                               DLList * tcs_list);
/* ------------------------------------------------------------------------- */
void            tm_set_status (DLListIterator tm_data_item, int status);
/* ------------------------------------------------------------------------- */
void            tm_set_cfg_filenames (DLListIterator tm_data_item,
                                      DLList * cfg_filename_list);
/* ------------------------------------------------------------------------- */
void            tm_set_test_summary_data (DLListIterator tm_data_item,
                                          test_summary_s * test_summary);
/* ------------------------------------------------------------------------- */
void            tm_set_test_summary_detail (DLListIterator tm_data_item,
                                            int ts_type, int ts_type_count);
/* ------------------------------------------------------------------------- */
#endif                          /* DATA_TEST_MODULE_INFO_H */
/* End of file */
