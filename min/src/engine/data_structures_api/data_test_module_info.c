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
 *  @file       data_test_module_info.c
 *  @version    0.1
 *  @brief      This file contains implementation of Test Module Info API
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */

#include <string.h>
#include <pthread.h>

#include <min_common.h>
#include <data_test_module_info.h>
#include <data_test_result.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */

/** Test Module Info thread spesicific mutex data variable.
Initial value is PTHREAD_MUTEX_INITIALIZER.
(Note scope of variable and mutex are the same.)
 */
pthread_mutex_t TM_MUTEX = PTHREAD_MUTEX_INITIALIZER;

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
static test_module_id_t current_id = 1000;

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */

/** Local comparison function for same process ID value searching.
This function is used with DLLIST function dl_list_find().
*/
LOCAL int       compare_pid (const void *data1, const void *data2);

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* None */

/* ======================== FUNCTIONS ====================================== */

/** Adds Test Module Info data item to linked list
 *  @param list_handle pointer to linked list of Test Modules Info data
 *  @param tm_data pointer to Test Module Info data structure.
 *  @return pointer to Test Module Info data item,
 *          or returns INITPTR if operation failed.  
 *
 *  Possible errors:
 *  INITPTR if Test Module Info adding to list failed.
 */
DLListIterator tm_add (DLList * list_handle, test_module_info_s * tm_data)
{
        pthread_mutex_lock (&TM_MUTEX);

        DLListIterator  dllist_item = INITPTR;

        if (list_handle != INITPTR) {
                dl_list_add (list_handle, (void *)tm_data);
                dllist_item = dl_list_tail (list_handle);
        }

        pthread_mutex_unlock (&TM_MUTEX);

        return dllist_item;
}

/** Searches for test module by id from the given list
 *  @param list_handle pointer to linked list of Test Modules Info data
 *  @param id search key
 *  @return pointer to Test Module Info data item,
 *          or returns INITPTR if operation failed.  
 *
 */

DLListIterator  tm_find_by_module_id (DLList * list_handle, 
                                      test_module_id_t id)
{
        pthread_mutex_lock (&TM_MUTEX);
        DLListIterator  it;
        
        for (it = dl_list_head (list_handle); it != INITPTR;
             it = dl_list_next(it)) {
                if (((test_module_info_s *)dl_list_data(it))->module_id_ ==
                    id) {
                        pthread_mutex_unlock (&TM_MUTEX);
                        return it;
                }
        }

        pthread_mutex_unlock (&TM_MUTEX);
        
        return INITPTR;

}


/* ------------------------------------------------------------------------- */

/** Creates new Test Module Info data structure
 *  @param tm_filename contains Test Module Info filename string.
 *  @param cfg_filename_list pointer to config filename string linked list.
 *  @return pointer to Test Module Info data item,
 *          or returns INITPTR if operation failed.  
 *
 *  Possible errors:
 *  INITPTR if Test Module Info adding to list failed.
 */
test_module_info_s *tm_create (filename_t tm_filename,
                               DLList * cfg_filename_list,
                               test_module_id_t id)
{
        pthread_mutex_lock (&TM_MUTEX);

        test_module_info_s *test_module = NEW (test_module_info_s);

        if ((test_module != NULL) 
            && (strlen (tm_filename) < MaxFileName + 1)) {
                STRCPY (test_module->module_filename_, tm_filename,
                        strlen (tm_filename) + 1);
                test_module->cfg_filename_list_ = cfg_filename_list;
                test_module->test_case_list_ = INITPTR;
                test_module->test_summary_ = NULL;
                if (id == 0) {
                        test_module->module_id_ = current_id;
                        current_id ++;
                } else 
                        test_module->module_id_ = id;

        } else {
                /* Test Module Info data structure creation failed */
                test_module = INITPTR;
        }

        pthread_mutex_unlock (&TM_MUTEX);

        return test_module;
}

/* ------------------------------------------------------------------------- */

/** Removes Test Module Info data item from linked list where this exists
 *  @param tm_data_item pointer to Test Module Info data item.
 */
void tm_remove (DLListIterator tm_data_item)
{
        pthread_mutex_lock (&TM_MUTEX);

        if ((tm_data_item != INITPTR) && (tm_data_item != NULL))
                dl_list_remove_it (tm_data_item);

        pthread_mutex_unlock (&TM_MUTEX);
}

/* ------------------------------------------------------------------------- */

/** Deletes Test Module Info data structure and freeing memory allocation of
 *  used data
 *  @param test_module pointer to Test Module Info data structure.
 */
void tm_delete (test_module_info_s * test_module)
{
        pthread_mutex_lock (&TM_MUTEX);

        if (test_module != NULL) {

                if (test_module->cfg_filename_list_ != INITPTR) {
                        dl_list_free_data (&test_module->cfg_filename_list_);
                        dl_list_free (&test_module->cfg_filename_list_);
                }
                if (test_module->test_case_list_ != INITPTR)
                        dl_list_free (&test_module->test_case_list_);

                if (test_module->test_summary_)
                        DELETE (test_module->test_summary_);

                DELETE (test_module);
        }

        pthread_mutex_unlock (&TM_MUTEX);
}

/* ------------------------------------------------------------------------- */

/** Gets Test Module Info process ID value
 *  @param item_ptr pointer to Test Module Info data item.
 *  @return process ID value.
 *
 *  Possible errors:
 *  None.
 */
long tm_get_pid (DLListIterator item_ptr)
{
        pthread_mutex_lock (&TM_MUTEX);

        test_module_info_s *tm_data = dl_list_data (item_ptr);

        pthread_mutex_unlock (&TM_MUTEX);

        return tm_data->process_id_;
}

/* ------------------------------------------------------------------------- */

/** Gets test module id
 *  @param item_ptr pointer to Test Module Info data item.
 *  @return id
 *
 *  Possible errors:
 *  None.
 */
test_module_id_t tm_get_module_id (DLListIterator item_tm_data)
{
        pthread_mutex_lock (&TM_MUTEX);

        test_module_info_s *tm_data = dl_list_data (item_tm_data);

        pthread_mutex_unlock (&TM_MUTEX);

        return tm_data->module_id_;

}

/* ------------------------------------------------------------------------- */

/** Gets Test Case list of given Test Module Info data item
 *  @param item_tm_data pointer to Test Module Info data item.
 *  @return pointer to Test Case linked list.
 *
 *  Possible errors:
 *  None.
 */
DLList         *tm_get_tclist (DLListIterator item_tm_data)
{
        pthread_mutex_lock (&TM_MUTEX);

        test_module_info_s *tm_data = dl_list_data (item_tm_data);

        pthread_mutex_unlock (&TM_MUTEX);

        return tm_data->test_case_list_;
}

/* ------------------------------------------------------------------------- */

/** Gets Test Case status value
 *  @param tm_data_item pointer to Test Module Info data item.
 *  @return status value, or error if Test Module Info not available.
 *
 *  Possible errors:
 *  -1 if Test Module Info data not exists
 */
int tm_get_status (DLListIterator tm_data_item)
{
        pthread_mutex_lock (&TM_MUTEX);

        test_module_info_s *test_module;
        int             tm_status;

        test_module = (test_module_info_s *) dl_list_data (tm_data_item);

        if ((test_module != INITPTR) && (test_module != NULL)) {
                tm_status = test_module->status_;
        } else {
                /* Test Module Status value not available */
                tm_status = -1;
        }

        /*
           if (tm_data_item != INITPTR) {
           test_module = (test_module_info_s*)dl_list_data(tm_data_item);
           if (test_module)
           tm_status = test_module->status_; 
           else
           tm_status = -1;
           }
         */

        pthread_mutex_unlock (&TM_MUTEX);

        return tm_status;
}

/* ------------------------------------------------------------------------- */

/** Gets Test Module Info data item pointer according to process ID value
 *  @param tm_list pointer to Test Module Info data linked list.
 *  @param pid process ID value. 
 *  @return pointer to found Test Module Info data item,
 *          or INITPTR if wanted data item not exists.
 *
 *  Possible errors:
 *  INITPTR if Test Module Info data item not exists
 */
DLListIterator tm_get_ptr_by_pid (DLList * tm_list, long pid)
{
        pthread_mutex_lock (&TM_MUTEX);

        DLListIterator  tm_data_item = INITPTR;

        tm_data_item = dl_list_find (dl_list_head (tm_list),
                                     dl_list_tail (tm_list), compare_pid,
                                     &pid);

        pthread_mutex_unlock (&TM_MUTEX);

        return tm_data_item;
}

/* ------------------------------------------------------------------------- */

/** Compares process ID values between two Test Module Info data item with
 *  use of DLLIST function dl_list_find()
 *  @param data1 void* pointer to first data.
 *  @param data2 void* pointer to second data.
 *  @return returns integer result value of comparision,
 *          or returns error value if be compared data is not available.     
 *
 *  Possible errors:
 *  -2 if given data is not available.
 */
LOCAL int compare_pid (const void *data1, const void *data2)
{
        /* If compared data pointers are invalid, return error code -2 */
        int             result = -2;
        long            pid1, pid2;

        if ((data1 != NULL) && (data2 != NULL)) {
                pid1 = ((test_module_info_s *) data1)->process_id_;
                pid2 = *((long *)data2);

                if (pid1 > pid2)
                        result = 1;
                else if (pid1 == pid2)
                        result = 0;
                else
                        result = -1;
        }

        return result;
}

/* ------------------------------------------------------------------------- */

/** Gets pointer to Test Module Info config filenames linked list 
 *  @param tm_data_item pointer to Test Module Info data item.
 *  @return return pointer to linked list,
 *          or if given data item not available then returns INITPTR.
 *          
 *  Possible errors:
 *  INITPTR if Test Module Info data item not available.
 */
DLList         *tm_get_cfg_filenames (DLListIterator tm_data_item)
{
        pthread_mutex_lock (&TM_MUTEX);

        test_module_info_s *test_module = INITPTR;
        DLList         *cfg_filename_list = INITPTR;

        if (tm_data_item != INITPTR) {
                test_module = (test_module_info_s *) tm_data_item->data_;
                if (test_module) {
                        cfg_filename_list = test_module->cfg_filename_list_;
                }
        }

        pthread_mutex_unlock (&TM_MUTEX);

        return cfg_filename_list;
}

/* ------------------------------------------------------------------------- */

/** Copy filename string of Test Module Info data item to given filename
 *  string
 *  @param tm_data_item pointer to Test Module Info data item.
 *  @param tm_filename filename string where wanted filename be copied. 
*/
void tm_get_module_filename (DLListIterator tm_data_item,
                             filename_t tm_filename)
{
        pthread_mutex_lock (&TM_MUTEX);

        test_module_info_s *test_module = INITPTR;

        if (tm_data_item != INITPTR) {
                test_module = (test_module_info_s *) tm_data_item->data_;
                if (test_module) {
                        STRCPY (tm_filename, test_module->module_filename_,
                                strlen (test_module->module_filename_) + 1);
                }
        }

        pthread_mutex_unlock (&TM_MUTEX);
}

/* ------------------------------------------------------------------------- */

/** Gets pointer to Test Summary data structure of given Test Module Info
 *  data item
 *  @param tm_data_item pointer to Test Module Info data item. 
 *  @return returns pointer Test Summary data structure,
 *          if Test Module Info data not available returns INITPTR.
 *
 *  Possible errors:
 *  INITPTR if Test Module Info data not available.
 */
test_summary_s *tm_get_test_summary_data (DLListIterator tm_data_item)
{
        pthread_mutex_lock (&TM_MUTEX);

        test_summary_s *test_summary = INITPTR;
        test_module_info_s *test_module = INITPTR;

        if (tm_data_item != INITPTR) {
                test_module = (test_module_info_s *) tm_data_item->data_;
                if (test_module)
                        test_summary = test_module->test_summary_;
        }

        pthread_mutex_unlock (&TM_MUTEX);

        return test_summary;
}

/* ------------------------------------------------------------------------- */

/** Gets Test Summary detail data by given data structure member enumeration
 *  @param tm_data_item pointer to Test Module Info data item.
 *  @param ts_type data structure member type. 
 *  @return positive integer value of wanted detail data.
 *          If Test Summary data not available returns error code.
 *
 *  Possible errors:
 *  -1 If Test Summary data not available.
 *  -2 If data structure member type is invalid.
 */
int tm_get_test_summary_detail (DLListIterator tm_data_item, int ts_type)
{
        pthread_mutex_lock (&TM_MUTEX);

        test_module_info_s *test_module = INITPTR;
        test_summary_s *test_summary = INITPTR;
        int             ts_count = -1;  /* Test Summary data not available */

        if (tm_data_item != INITPTR) {
                test_module = (test_module_info_s *) tm_data_item->data_;

                if ((test_module != INITPTR) && (test_module != NULL)) {
                        test_summary = test_module->test_summary_;

                        if ((test_summary != INITPTR)
                            && (test_summary != NULL)) {

                                switch (ts_type) {

                                case TEST_RESULT_PASSED:
                                        ts_count = test_summary->passed_;
                                        break;

                                case TEST_RESULT_FAILED:
                                        ts_count = test_summary->failed_;
                                        break;

                                case TEST_RESULT_CRASHED:
                                        ts_count = test_summary->crashed_;
                                        break;

                                case TEST_RESULT_ABORTED:
                                        ts_count = test_summary->aborted_;
                                        break;

                                case TEST_RESULT_TIMEOUT:
                                        ts_count = test_summary->timeout_;
                                        break;

                                default:
                                        /* Invalid Test Summary type */
                                        ts_count = -2;
                                        break;
                                }
                        }
                }
        }

        pthread_mutex_unlock (&TM_MUTEX);

        return ts_count;
}

/* ------------------------------------------------------------------------- */

/** Sets process ID value of given Test Module Info data item
 *  @param item_ptr pointer to Test Module Info data item.
 *  @param tm_pid new process ID value.
 */
void tm_set_pid (DLListIterator item_ptr, long tm_pid)
{
        pthread_mutex_lock (&TM_MUTEX);

        test_module_info_s *tm_data_item =
            (test_module_info_s *) dl_list_data (item_ptr);

        if (tm_data_item != INITPTR)
                tm_data_item->process_id_ = tm_pid;

        pthread_mutex_unlock (&TM_MUTEX);
}

/* ------------------------------------------------------------------------- */

/** Sets pointer to Test Case linked list for given Test Module Info
 *  data item
 *  @param item_tm_data pointer to Test Module Info data item. 
 *  @param tcs_list pointer to Test Case linked list.
 */
void tm_set_tclist (DLListIterator item_tm_data, DLList * tcs_list)
{
        pthread_mutex_lock (&TM_MUTEX);

        test_module_info_s *tm_data = dl_list_data (item_tm_data);
        tm_data->test_case_list_ = tcs_list;

        pthread_mutex_unlock (&TM_MUTEX);
}

/* ------------------------------------------------------------------------- */

/** Sets status value of given Test Module Info data item
 *  @param tm_data_item pointer to Test Module Info data item.
 *  @param status new status value.
 */
void tm_set_status (DLListIterator tm_data_item, int status)
{
        pthread_mutex_lock (&TM_MUTEX);

        test_module_info_s *test_module =
            (test_module_info_s *) dl_list_data (tm_data_item);

        if ((test_module != INITPTR) && (test_module != NULL)) {
                test_module->status_ = status;
        }

        /*
           test_module_info_s* test_module = INITPTR;

           if (tm_data_item != INITPTR) {
           test_module = (test_module_info_s*)dl_list_data(tm_data_item);
           if (test_module)
           test_module->status_ = status;
           }
         */

        pthread_mutex_unlock (&TM_MUTEX);
}

/* ------------------------------------------------------------------------- */

/** Sets pointer to config filename linked list for Test Module Info
 *  data item 
 *  @param tm_data_item pointer to Test Module Info data item. 
 *  @param cfg_filename_list pointer linked list of config filenames.
 */
void tm_set_cfg_filenames (DLListIterator tm_data_item,
                           DLList * cfg_filename_list)
{
        pthread_mutex_lock (&TM_MUTEX);

        test_module_info_s *test_module = NULL;

        if (tm_data_item) {
                test_module = (test_module_info_s *) tm_data_item->data_;
                if (test_module) {
                        test_module->cfg_filename_list_ = cfg_filename_list;
                }
        }

        pthread_mutex_unlock (&TM_MUTEX);
}

/* ------------------------------------------------------------------------- */

/** Creates new Test Summary data structure for given Test Moddule Info
 *  data item
 *  @param tm_data_item pointer to Test Module Info data item.
 *  @return returns integer value 1 if data structure created successfully,
 *          or returns error code if operation failed.
 *
 *  Possible errors:
 *  -1 if Test Summary creation failed.
 */
int tm_create_test_summary (DLListIterator tm_data_item)
{
        pthread_mutex_lock (&TM_MUTEX);

        /* If Test Summary creation failed then returns error code -1 */
        int             result = -1;
        test_module_info_s *test_module = INITPTR;
        test_summary_s *test_summary = INITPTR;

        if (tm_data_item != INITPTR) {
                test_module = (test_module_info_s *) tm_data_item->data_;

                if (test_module) {
                        test_summary = NEW (test_summary_s);

                        if (test_summary) {
                                test_module->test_summary_ = test_summary;
                                result = 1;
                        }
                }
        }

        pthread_mutex_unlock (&TM_MUTEX);

        return result;
}

/* ------------------------------------------------------------------------- */

/** Sets pointer to Test Summary data structure for given Test Module Info
 *  data item
 *  @param tm_data_item pointer to Test Module Info data item.
 *  @param test_summary pointer to Test Summary data structure. 
*/
void tm_set_test_summary_data (DLListIterator tm_data_item,
                               test_summary_s * test_summary)
{
        pthread_mutex_lock (&TM_MUTEX);

        test_module_info_s *test_module = INITPTR;

        if (tm_data_item != INITPTR) {
                test_module = (test_module_info_s *) tm_data_item->data_;
                if (test_module)
                        test_module->test_summary_ = test_summary;
        }

        pthread_mutex_unlock (&TM_MUTEX);
}

/* ------------------------------------------------------------------------- */

/** Sets selected Test Summary data detail for given Test Module Info
 *  data item
 *  @param tm_data_item pointer to Test Module Info data item.
 *  @param ts_type selected Test Summary data structure data member type.
 *  @param ts_type_count new count value for selected data structure member.
 */
void tm_set_test_summary_detail (DLListIterator tm_data_item,
                                 int ts_type, int ts_type_count)
{
        pthread_mutex_lock (&TM_MUTEX);

        test_module_info_s *test_module = INITPTR;
        test_summary_s *test_summary = INITPTR;

        if (tm_data_item != INITPTR) {
                test_module = (test_module_info_s *) tm_data_item->data_;

                if ((test_module != INITPTR) && (test_module != NULL)) {
                        test_summary =
                            (test_summary_s *) test_module->test_summary_;

                        if ((test_summary != INITPTR)
                            && (test_module != NULL)) {

                                switch (ts_type) {

                                case TEST_RESULT_PASSED:
                                        test_summary->passed_ = ts_type_count;
                                        break;

                                case TEST_RESULT_FAILED:
                                        test_summary->failed_ = ts_type_count;
                                        break;

                                case TEST_RESULT_CRASHED:
                                        test_summary->crashed_ =
                                            ts_type_count;
                                        break;

                                case TEST_RESULT_ABORTED:
                                        test_summary->aborted_ =
                                            ts_type_count;
                                        break;

                                case TEST_RESULT_TIMEOUT:
                                        test_summary->timeout_ =
                                            ts_type_count;
                                        break;

                                default:
                                        /* None */
                                        break;
                                }
                        }
                }
        }

        pthread_mutex_unlock (&TM_MUTEX);
}

/* ------------------------------------------------------------------------- */

/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
/* None */

/* ========================================================================= */
/* End of file */
