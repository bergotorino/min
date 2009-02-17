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
 *  @file       data_test_case.c
 *  @version    0.1
 *  @brief      This file contains implementation of Test Case API
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */

#include <string.h>
#include <pthread.h>
#include <data_test_case.h>

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

/** Test Case thread spesicific mutex data variable.
Initial value is PTHREAD_MUTEX_INITIALIZER.
(Note scope of variable and mutex are the same.)
*/
LOCAL pthread_mutex_t TC_MUTEX = PTHREAD_MUTEX_INITIALIZER;

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
/* None */

/* ======================== FUNCTIONS ====================================== */

/** Adds new Test Case data structure to linked list DLList
 *  @param list_handle link to list to which a new element is added.
 *  @param tc_data pointer to data that is stored on the list.
 *  @return iterator pointing to added element, or INITPTR if adding
 *          failed.
 *
 *  Possible errors:
 *  - INITPTR if data adding not completed.
 */
DLListIterator tc_add (DLList * list_handle, test_case_s * tc_data)
{
        DLListIterator  dllist_item = INITPTR;

        pthread_mutex_lock (&TC_MUTEX);


        /* Add Test Case data item to given linked list */
        if (list_handle != INITPTR) {
                dl_list_add (list_handle, (void *)tc_data);
                dllist_item = dl_list_tail (list_handle);
        }

        pthread_mutex_unlock (&TC_MUTEX);

        return dllist_item;
}

/* ------------------------------------------------------------------------- */

/** Creates new Test Case data structure
 *  @param tm_data_item pointer to Test Module Info data item.
 *  @param tc_cfg_filename contains filaname string of
 *         Test Case config file.
 *  @param tc_title contains title name string of Test Case. 
 *  @return pointer to Test Case data, or NULL if creation failed.
 *
 *  Possible errors:
 *  - NULL if data creation not completed.
 */
test_case_s    *tc_create (DLListIterator tm_data_item,
                           filename_t tc_cfg_filename,
                           title_string_t tc_title)
{
        pthread_mutex_lock (&TC_MUTEX);

        test_case_s    *tc_data = NEW (test_case_s);

        if ((tc_data != NULL) &&
            (strlen (tc_cfg_filename) < MaxFileName + 1) &&
            (strlen (tc_title) < MaxTestCaseName + 1)) {

                /* Test Case basic initialization data */
                tc_data->tc_id_ = 0;
                tc_data->tc_group_id_ = 0;
                tc_data->status_ = TEST_CASE_IDLE;
                tc_data->priority_ = 0;
                tc_data->test_result_list_ = dl_list_create ();

                /* Test Case given data */
                tc_data->tm_data_item_ = tm_data_item;
                STRCPY (tc_data->title_, tc_title, strlen (tc_title) + 1);
                STRCPY (tc_data->tc_cfg_filename_, tc_cfg_filename,
                        strlen (tc_cfg_filename) + 1);
        } else {
                tc_data = INITPTR;
        }

        pthread_mutex_unlock (&TC_MUTEX);

        return tc_data;
}

/* ------------------------------------------------------------------------- */

/** Searches for test module by id from the given list
 *  @param list_handle pointer to linked list of Test Modules Info data
 *  @param id search key
 *  @return pointer to Test Module Info data item,
 *          or returns INITPTR if operation failed.  
 *
 */
DLListIterator  tc_find_by_case_id (DLList * list_handle, 
				    int test_case_id)
{
        pthread_mutex_lock (&TC_MUTEX);
        DLListIterator  it;
        
        for (it = dl_list_head (list_handle); it != INITPTR;
             it = dl_list_next(it)) {
                if (((test_case_s    *)dl_list_data(it))->tc_id_ ==
                    test_case_id) {
                        pthread_mutex_unlock (&TC_MUTEX);
                        return it;
                }
        }

        pthread_mutex_unlock (&TC_MUTEX);
        
        return INITPTR;

}

/* ------------------------------------------------------------------------- */

/** Removes Test Case data item from linked list where this exists
 *  @param tc_data_item pointer to Test Case data item.
 */
void tc_remove (DLListIterator tc_data_item)
{
        pthread_mutex_lock (&TC_MUTEX);

        if ((tc_data_item != INITPTR) && (tc_data_item != NULL))
                dl_list_remove_it (tc_data_item);

        pthread_mutex_unlock (&TC_MUTEX);
}

/* ------------------------------------------------------------------------- */

/** Deletes Test Case data structure and freeing used memory allocation
 *  @param test_case pointer to Test Case data structure.
 */
void tc_delete (test_case_s * test_case)
{
        DLListIterator  it, next_it;
        test_result_s  *tr;

        pthread_mutex_lock (&TC_MUTEX);


        /* if( (test_case != NULL) && (test_case != INITPTR) ) { */
        it = dl_list_head (test_case->test_result_list_);
        while (it != INITPTR) {
                next_it = dl_list_next (it);
                tr = dl_list_data (it);
                tr_remove (it);
                DELETE (tr);
                it = next_it;
        }
        dl_list_free (&test_case->test_result_list_);
        DELETE (test_case);


        pthread_mutex_unlock (&TC_MUTEX);
}

/* ------------------------------------------------------------------------- */

/** Gets ID value of specific Test Case data item
 *  @param tc_data_item pointer to Test Case data item.
 *  @return positive integer value of ID, or -1 if get ID failed.
 *
 *  Possible errors:
 *  -1 if Test Case data item not exists.
 */
int tc_get_id (DLListIterator tc_data_item)
{
        pthread_mutex_lock (&TC_MUTEX);

        int             tc_id;
        test_case_s    *test_case;

        test_case = (test_case_s *) dl_list_data (tc_data_item);

        if ((test_case != INITPTR) && (test_case != NULL))
                tc_id = test_case->tc_id_;
        else
                tc_id = -1;

        pthread_mutex_unlock (&TC_MUTEX);

        return tc_id;
}

/* ------------------------------------------------------------------------- */

/** Gets group ID value of specific Test Case data item
 *  @param tc_data_item pointer to Test Case data item.
 *  @return positive integer value of group ID,
 *          or -1 if get group ID failed.
 *
 *  Possible errors:
 *  -1 if Test Case data item not exists.
 */
int tc_get_group_id (DLListIterator tc_data_item)
{
        pthread_mutex_lock (&TC_MUTEX);

        int             tc_group_id;
        test_case_s    *test_case;

        test_case = (test_case_s *) dl_list_data (tc_data_item);

        if ((test_case != INITPTR) && (test_case != NULL))
                tc_group_id = test_case->tc_group_id_;
        else
                tc_group_id = -1;

        pthread_mutex_unlock (&TC_MUTEX);

        return tc_group_id;
}

/* ------------------------------------------------------------------------- */

/** Gets Test Case title string and copying it to
 *  given title string parameter
 *  @param tc_data_item pointer to Test Case data item.
 *  @param title_string title string parameter where will be copied title.
 *  @return 1 value if title string copied complete,
 *          or -1 if get title operation failed.
 *
 *  Possible errors:
 *  -1 if Test Case data item not exists.
 */
int tc_get_title (DLListIterator tc_data_item, title_string_t title_string)
{
        pthread_mutex_lock (&TC_MUTEX);

        int             result;
        test_case_s    *test_case =
            (test_case_s *) dl_list_data (tc_data_item);

        if ((test_case != INITPTR) && (test_case != NULL)) {
                STRCPY (title_string, test_case->title_,
                        strlen (test_case->title_) + 1);
                result = 1;
        } else {
                /* Something failed, so will returned error value -1 */
                result = -1;
        }

        pthread_mutex_unlock (&TC_MUTEX);

        return result;
}

/* ------------------------------------------------------------------------- */

/** Gets Test Case status value
 *  @param tc_data_item pointer to Test Case data item.
 *  @return status integer value,
 *          or -1 if get status operation failed.
 *
 *  Possible errors:
 *  -1 if Test Case data item not exists.
 */
int tc_get_status (DLListIterator tc_data_item)
{
        pthread_mutex_lock (&TC_MUTEX);

        /* If Test Case status data not available then error value -1 */
        int             tc_status = -1;
        test_case_s    *test_case =
            (test_case_s *) dl_list_data (tc_data_item);

        if ((test_case != INITPTR) && (test_case != NULL))
                tc_status = test_case->status_;

        pthread_mutex_unlock (&TC_MUTEX);

        return tc_status;
}

/* ------------------------------------------------------------------------- */

/** Gets Test Case config filename string and
 *  copying it to given title string parameter
 *  @param tc_data_item pointer to Test Case data item.
 *  @param tc_cfg_filename filename string where data will be copied
 */
void tc_get_cfg_filename (DLListIterator tc_data_item,
                          filename_t tc_cfg_filename)
{
        pthread_mutex_lock (&TC_MUTEX);

        test_case_s    *test_case =
            (test_case_s *) dl_list_data (tc_data_item);

        if ((test_case != INITPTR) && (test_case != NULL)) {
                STRCPY (tc_cfg_filename, test_case->tc_cfg_filename_,
                        strlen (test_case->tc_cfg_filename_) + 1);
        }

        pthread_mutex_unlock (&TC_MUTEX);
}

/* ------------------------------------------------------------------------- */

/** Gets Test Result list pointer according to Test Case data item
 *  @param tc_data_item pointer to Test Case data item.
 *  @return pointer to Test Result linked list,
 *          or -1 if get status operation failed.
 *
 *  Possible errors:
 *  INITPTR if Test Case data item not exists.
 */
DLList         *tc_get_tr_list (DLListIterator tc_data_item)
{
        pthread_mutex_lock (&TC_MUTEX);

        DLList         *test_result_list = INITPTR;
        test_case_s    *test_case =
            (test_case_s *) dl_list_data (tc_data_item);

        if ((test_case != INITPTR) && (test_case != NULL))
                test_result_list = test_case->test_result_list_;

        pthread_mutex_unlock (&TC_MUTEX);

        return test_result_list;
}

/* ------------------------------------------------------------------------- */

/** Gets Test Result priority value
 *  @param tc_data_item pointer to Test Case data item.
 *  @return positive integer priority value,
 *          or -1 if get priority operation failed.
 *
 *  Possible errors:
 *  -1 if Test Case data item not exists.
 */
int tc_get_priority (DLListIterator tc_data_item)
{
        pthread_mutex_lock (&TC_MUTEX);

        /* If Test Case priority value not available then returns error -1 */
        int             priority = -1;
        test_case_s    *test_case =
            (test_case_s *) dl_list_data (tc_data_item);

        if ((test_case != INITPTR) && (test_case != NULL))
                priority = test_case->priority_;

        pthread_mutex_unlock (&TC_MUTEX);

        return priority;
}

/* ------------------------------------------------------------------------- */

/** Gets Test Module Info data iten pointer according to Test Case data item
 *  @param tc_data_item pointer to Test Case data item.
 *  @return pointer to Test Module Info data item,
 *          or INITPTR if get data pointer operation failed.
 *
 *  Possible errors:
 *  INITPTR if Test Case data item not exists.
 */
DLListIterator tc_get_test_module_ptr (DLListIterator tc_data_item)
{
        pthread_mutex_lock (&TC_MUTEX);

        DLListIterator  tm_data_item = INITPTR;
        test_case_s    *test_case =
            (test_case_s *) dl_list_data (tc_data_item);

        if ((test_case != INITPTR) && (test_case != NULL))
                tm_data_item = test_case->tm_data_item_;

        pthread_mutex_unlock (&TC_MUTEX);

        return tm_data_item;
}

/* ------------------------------------------------------------------------- */

/** Sets Test Case ID value
 *  @param tc_data_item pointer to Test Case data item.
 *  @param test_case_id integer value of Test Case ID.
 */
void tc_set_id (DLListIterator tc_data_item, int test_case_id)
{
        pthread_mutex_lock (&TC_MUTEX);

        test_case_s    *test_case =
            (test_case_s *) dl_list_data (tc_data_item);

        if ((test_case != INITPTR) && (test_case != NULL))
                test_case->tc_id_ = test_case_id;

        pthread_mutex_unlock (&TC_MUTEX);
}

/* ------------------------------------------------------------------------- */

/** Sets Test Case group ID value
 *  @param tc_data_item pointer to Test Case data item.
 *  @param group_id integer value of Test Case group ID.
 */
void tc_set_group_id (DLListIterator tc_data_item, int group_id)
{
        pthread_mutex_lock (&TC_MUTEX);

        test_case_s    *test_case =
            (test_case_s *) dl_list_data (tc_data_item);

        if ((test_case != INITPTR) && (test_case != NULL))
                test_case->tc_group_id_ = group_id;

        pthread_mutex_unlock (&TC_MUTEX);
}

/* ------------------------------------------------------------------------- */

/** Sets Test Case status value
 *  @param tc_data_item pointer to Test Case data item.
 *  @param status integer value of Test Case status.
 */
void tc_set_status (DLListIterator tc_data_item, int status)
{
        pthread_mutex_lock (&TC_MUTEX);

        test_case_s    *test_case =
            (test_case_s *) dl_list_data (tc_data_item);

        if ((test_case != INITPTR) && (test_case != NULL))
                test_case->status_ = status;

        pthread_mutex_unlock (&TC_MUTEX);
}

/* ------------------------------------------------------------------------- */

/** Sets Test Case config filename string
 *  @param tc_data_item pointer to Test Case data item.
 *  @param tc_cfg_filename new Test Case config filename string.
 */
void tc_set_cfg_filename (DLListIterator tc_data_item,
                          filename_t tc_cfg_filename)
{
        pthread_mutex_lock (&TC_MUTEX);

        test_case_s    *test_case =
            (test_case_s *) dl_list_data (tc_data_item);

        if ((test_case != INITPTR) &&
            (test_case != NULL) &&
            (strlen (tc_cfg_filename) < MaxFileName + 1)) {
                STRCPY (test_case->tc_cfg_filename_, tc_cfg_filename,
                        strlen (tc_cfg_filename) + 1);

        }

        pthread_mutex_unlock (&TC_MUTEX);
}

/* ------------------------------------------------------------------------- */

/** Sets Test Case priority value
 *  @param tc_data_item pointer to Test Case data item.
 *  @param priority integer value of Test Case priority.
 */
void tc_set_priority (DLListIterator tc_data_item, int priority)
{
        pthread_mutex_lock (&TC_MUTEX);

        test_case_s    *test_case =
            (test_case_s *) dl_list_data (tc_data_item);

        if ((test_case != INITPTR) && (test_case != NULL))
                test_case->priority_ = priority;

        pthread_mutex_unlock (&TC_MUTEX);
}

/* ------------------------------------------------------------------------- */

/** Sets Test Result list pointer for Test Case data item
 *  @param tc_data_item pointer to Test Case data item.
 *  @param test_result_list pointer to linked list of Test Result list.
 */
void tc_set_tr_list (DLListIterator tc_data_item, DLList * test_result_list)
{
        pthread_mutex_lock (&TC_MUTEX);

        test_case_s    *test_case =
            (test_case_s *) dl_list_data (tc_data_item);

        if ((test_case != INITPTR) && (test_case != NULL))
                test_case->test_result_list_ = test_result_list;

        pthread_mutex_unlock (&TC_MUTEX);
}

/* ------------------------------------------------------------------------- */

/** Sets Test Module Info data item pointer for Test Case data item
 *  @param tc_data_item pointer to Test Case data item.
 *  @param tm_data_item pointer to setting Test Module Info data item.
 */
void tc_set_test_module_ptr (DLListIterator tc_data_item,
                             DLListIterator tm_data_item)
{
        pthread_mutex_lock (&TC_MUTEX);

        test_case_s    *test_case =
            (test_case_s *) dl_list_data (tc_data_item);

        if ((test_case != INITPTR) && (test_case != NULL))
                test_case->tm_data_item_ = tm_data_item;

        pthread_mutex_unlock (&TC_MUTEX);
}

/* ------------------------------------------------------------------------- */

/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
/* None */

/* ========================================================================= */
/* End of file */
