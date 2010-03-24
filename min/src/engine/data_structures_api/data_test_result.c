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
 *  @file       data_test_result.c
 *  @version    0.1
 *  @brief      This file contains implementation of Test Result API
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */

#include <string.h>
#include <pthread.h>
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

/** Test Result thread spesicific mutex data variable.
Initial value is PTHREAD_MUTEX_INITIALIZER.
(Note scope of variable and mutex are the same)
 */
LOCAL pthread_mutex_t TR_MUTEX = PTHREAD_MUTEX_INITIALIZER;

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
/* ------------------------------------------------------------------------- */
/** Adds Test Result data item to linked list
 *  @param list_handle pointer to linked list of Test Result data
 *  @param test_result pointer to Test Result data structure.
 *  @return pointer to Test Result data item,
 *          or returns INITPTR if operation failed.  
 *
 *  Possible errors:
 *  INITPTR if Test Result data item adding to list failed.
 */
DLListIterator tr_add (DLList * list_handle, test_result_s * test_result)
{
        pthread_mutex_lock (&TR_MUTEX);

        DLListIterator  dllist_item = INITPTR;

        /* Add Test Result data item to given linked list */
        if (list_handle != INITPTR) {
                dl_list_add (list_handle, (void *)test_result);
                dllist_item = dl_list_tail (list_handle);
        }

        pthread_mutex_unlock (&TR_MUTEX);

        return dllist_item;
}
/* ------------------------------------------------------------------------- */
/** Creates Test Result data structure
 *  @param tc_data_item pointer to Test Case data item which processed this
 *                      Test Result data.
 *  @return pointer to Test Result data structure,
 *          or returns INITPTR if operation failed.  
 *
 *  Possible errors:
 *  INITPTR if Test Result data structure creation failed.
 */
test_result_s  *tr_create_data_item (DLListIterator tc_data_item)
{
        pthread_mutex_lock (&TR_MUTEX);

        test_result_s  *test_result = INITPTR;

        test_result_s  *data = NEW (test_result_s);

        if (data) {
                test_result = data;

                /* Test Result given data */
                test_result->tc_data_item_ = tc_data_item;

                /* Test Result basic initialization data */
                test_result->start_time_ = 0;
                test_result->end_time_ = 0;
                test_result->result_type_ = TEST_RESULT_NOT_RUN;
                test_result->result_code_ = 0;
                test_result->printouts_list_ = INITPTR;
        }

        pthread_mutex_unlock (&TR_MUTEX);

        return test_result;
}
/* ------------------------------------------------------------------------- */
/** Removes given Test Result data item from linked list.
 *  @param tr_data_item pointer to given Test Result data item.
 */
void tr_remove (DLListIterator tr_data_item)
{
        /* Mutex locking not used because next function including also it */
        tr_remove_printouts_list (tr_data_item);

        pthread_mutex_lock (&TR_MUTEX);

        dl_list_remove_it (tr_data_item);

        pthread_mutex_unlock (&TR_MUTEX);
}
/* ------------------------------------------------------------------------- */
/** Gets result description string from  given Test Result data item.
 *  @param tr_data_item pointer to given Test Result data item.
 */
void tr_get_result_description (DLListIterator tr_data_item,
                                result_description_t result_desc)
{
        pthread_mutex_lock (&TR_MUTEX);

        test_result_s  *test_result =
            (test_result_s *) dl_list_data (tr_data_item);

        if ((test_result != INITPTR) && (test_result != NULL)) {
                STRCPY (result_desc, test_result->result_description_,
                        strlen (test_result->result_description_) + 1);
        }

        pthread_mutex_unlock (&TR_MUTEX);
}
/* ------------------------------------------------------------------------- */
/** Gets result type value from given Test Result Data item
 *  @param tr_data_item pointer to given Test Result data item.
 *  @return positive integer result type value,
 *          or returns -1 if operation failed.  
 *
 *  Possible errors:
 *  -1 if Test Result data item not available.
 */
int tr_get_result_type (DLListIterator tr_data_item)
{
        pthread_mutex_lock (&TR_MUTEX);

        /* If result type not available then returns error code -1 */
        int             result_type = -1;

        test_result_s  *test_result =
            (test_result_s *) dl_list_data (tr_data_item);

        if ((test_result != INITPTR) && (test_result != NULL)) {
                result_type = test_result->result_type_;
        }

        pthread_mutex_unlock (&TR_MUTEX);

        return result_type;
}
/* ------------------------------------------------------------------------- */
/** Gets result code value from given Test Result Data item
 *  @param tr_data_item pointer to given Test Result data item.
 *  @return positive integer result code value,
 *          or returns -1 if operation failed.  
 *
 *  Possible errors:
 *  -1 if Test Result data item not available.
 */
int tr_get_result_code (DLListIterator tr_data_item)
{
        pthread_mutex_lock (&TR_MUTEX);

        /* If result code not available then returns error code -1 */
        int             result_code = -1;

        test_result_s  *test_result =
            (test_result_s *) dl_list_data (tr_data_item);
        if ((test_result != INITPTR) && (test_result != NULL)) {
                result_code = test_result->result_code_;
        }

        pthread_mutex_unlock (&TR_MUTEX);

        return result_code;
}
/* ------------------------------------------------------------------------- */
/** Gets start time value from given Test Result Data item
 *  @param tr_data_item pointer to given Test Result data item.
 *  @return long type start time value.
 *
 *  Possible errors:
 *  None.
 */
long tr_get_start_time (DLListIterator tr_data_item)
{
        pthread_mutex_lock (&TR_MUTEX);

        /* Default start time value is 0 */
        long            start_time = 0;

        test_result_s  *test_result =
            (test_result_s *) dl_list_data (tr_data_item);;
        if ((test_result != INITPTR) && (test_result != NULL)) {
                start_time = test_result->start_time_;
        }

        pthread_mutex_unlock (&TR_MUTEX);

        return start_time;
}
/* ------------------------------------------------------------------------- */
/** Gets end time value from given Test Result Data item
 *  @param tr_data_item pointer to given Test Result data item.
 *  @return long type end time value.
 *
 *  Possible errors:
 *  None.
 */
long tr_get_end_time (DLListIterator tr_data_item)
{
        pthread_mutex_lock (&TR_MUTEX);

        /* Default end time value is 0 */
        long            end_time = 0;

        test_result_s  *test_result =
            (test_result_s *) dl_list_data (tr_data_item);

        if ((test_result != INITPTR) && (test_result != NULL)) {
                end_time = test_result->end_time_;
        }

        pthread_mutex_unlock (&TR_MUTEX);

        return end_time;
}
/* ------------------------------------------------------------------------- */
/** Gets pointer to linked list of printouts list
 *  @param tr_data_item pointer to Test Result data item.
 *  @return returns pointer to linked list of printouts list data,
 *          or returns INITPTR if operation failed.  
 *
 *  Possible errors:
 *  INITPTR if Test Result data item not available.
 */
DLList         *tr_get_priontouts_list (DLListIterator tr_data_item)
{
        pthread_mutex_lock (&TR_MUTEX);

        DLList         *printouts_list = INITPTR;
        test_result_s  *test_result =
            (test_result_s *) dl_list_data (tr_data_item);

        if ((test_result != INITPTR) && (test_result != NULL)) {
                printouts_list = test_result->printouts_list_;
        }

        pthread_mutex_unlock (&TR_MUTEX);

        return printouts_list;
}
/* ------------------------------------------------------------------------- */
/** Sets new result description string text for given Test Result data item
 *  @param tr_data_item pointer to given Test Result data item.
 *  @param result_desc new result description string text. 
 */
void tr_set_result_description (DLListIterator tr_data_item,
                                result_description_t result_desc)
{
        pthread_mutex_lock (&TR_MUTEX);

        size_t          str_size = 0;
        test_result_s  *test_result =
            (test_result_s *) dl_list_data (tr_data_item);

        str_size = strlen (result_desc) + 1;

        if ((str_size <= MaxTestResultDescription) &&
            (test_result != INITPTR) && (test_result != NULL)) {
                STRCPY (test_result->result_description_, result_desc,
                        str_size);
        }

        pthread_mutex_unlock (&TR_MUTEX);
}
/* ------------------------------------------------------------------------- */
/** Sets result type value for given Test Result data item
 *  @param tr_data_item pointer to given Test Result data item.
 */
void tr_set_result_type (DLListIterator tr_data_item, int result_type)
{
        pthread_mutex_lock (&TR_MUTEX);

        test_result_s  *test_result =
            (test_result_s *) dl_list_data (tr_data_item);

        if ((test_result != INITPTR) && (test_result != NULL)) {
                test_result->result_type_ = result_type;
        }

        pthread_mutex_unlock (&TR_MUTEX);
}
/* ------------------------------------------------------------------------- */
/** Sets result code for given Test Result data item.
 *  @param tr_data_item pointer to given Test Result data item.
 */
void tr_set_result_code (DLListIterator tr_data_item, int result_code)
{
        pthread_mutex_lock (&TR_MUTEX);

        test_result_s  *test_result =
            (test_result_s *) dl_list_data (tr_data_item);

        if ((test_result != INITPTR) && (test_result != NULL)) {
                test_result->result_code_ = result_code;
        }

        pthread_mutex_unlock (&TR_MUTEX);
}
/* ------------------------------------------------------------------------- */
/** Sets start time for given Test Result data item
 *  @param tr_data_item pointer to given Test Result data item.
 */
void tr_set_start_time (DLListIterator tr_data_item, long start_time)
{
        pthread_mutex_lock (&TR_MUTEX);

        test_result_s  *test_result =
            (test_result_s *) dl_list_data (tr_data_item);

        if ((test_result != INITPTR) && (test_result != NULL)) {
                test_result->start_time_ = start_time;
        }

        pthread_mutex_unlock (&TR_MUTEX);
}
/* ------------------------------------------------------------------------- */
/** Sets end time for given Test Result data item
 *  @param tr_data_item pointer to given Test Result data item.
 */
void tr_set_end_time (DLListIterator tr_data_item, long end_time)
{
        pthread_mutex_lock (&TR_MUTEX);

        test_result_s  *test_result =
            (test_result_s *) dl_list_data (tr_data_item);

        if ((test_result != INITPTR) && (test_result != NULL)) {
                test_result->end_time_ = end_time;
        }

        pthread_mutex_unlock (&TR_MUTEX);
}
/* ------------------------------------------------------------------------- */
/** Sets pointer to linked list of printouts list for Test Result data item
 *  @param tr_data_item pointer to given Test Result data item.
 */
void tr_set_priontouts_list (DLListIterator tr_data_item,
                             DLList * printouts_list)
{
        pthread_mutex_lock (&TR_MUTEX);

        test_result_s  *test_result =
            (test_result_s *) dl_list_data (tr_data_item);

        if ((test_result != INITPTR) && (test_result != NULL)) {
                test_result->printouts_list_ = printouts_list;
        }

        pthread_mutex_unlock (&TR_MUTEX);
}
/* ------------------------------------------------------------------------- */
/** Creates Test Result prinout data structure
 *  @param tr_data_item pointer to Test Result data item.
 *  @return returns pointer to printout data structure,
 *          or returns NULL if operation failed.  
 *
 *  Possible errors:
 *  NULL if not possible create and allocate memory for printout data.
 */
test_result_printout_s *tr_create_printout (int priority, char *printout)
{
        pthread_mutex_lock (&TR_MUTEX);

        test_result_printout_s *tr_printout = NEW (test_result_printout_s);
        int             len = 0;

        if (tr_printout != NULL) {
                tr_printout->priority_ = priority;
                tr_printout->printout_ = NULL;

                if (printout != NULL) {
                        len = strlen (printout);
                        if (len > 0) {
                                tr_printout->printout_ =
                                    NEW2 (char, (len + 1));
                                STRCPY (tr_printout->printout_, printout,
                                        (len + 1));
                        }
                }
        }

        pthread_mutex_unlock (&TR_MUTEX);

        return tr_printout;
}
/* ------------------------------------------------------------------------- */
/** Removes printouts data structure and freeing used memery allocation
 *  @param tr_data_item pointer to given Test Result data item.
 */
void tr_remove_printout (test_result_printout_s * tr_printout)
{
        pthread_mutex_lock (&TR_MUTEX);

        if (tr_printout != NULL) {
                if (tr_printout->printout_ != NULL)
                        DELETE (tr_printout->printout_);
                DELETE (tr_printout);
        }

        pthread_mutex_unlock (&TR_MUTEX);
}
/* ------------------------------------------------------------------------- */
/** Removes linked list of printouts data from given Test Result data item
 *  @param tr_data_item pointer to given Test Result data item.
 */
void tr_remove_printouts_list (DLListIterator tr_data_item)
{
        pthread_mutex_lock (&TR_MUTEX);

        test_result_s  *test_result =
            (test_result_s *) dl_list_data (tr_data_item);
        DLList         *prt_list = INITPTR;
        DLListIterator  prt_data_item = INITPTR;
        test_result_printout_s *prt_data = INITPTR;

        if (test_result != INITPTR) {
                prt_list = test_result->printouts_list_;
                if (prt_list != INITPTR) {
                        prt_data_item = dl_list_head (prt_list);
                        while (prt_data_item != INITPTR) {
                                prt_data =
                                    (test_result_printout_s *)
                                    dl_list_data (prt_data_item);
                                if (prt_data->printout_ != INITPTR) {
                                        DELETE (prt_data->printout_);
                                }
                                DELETE (prt_data);
                                prt_data_item = dl_list_next (prt_data_item);
                        }
                }

                dl_list_free (&prt_list);
                test_result->printouts_list_ = INITPTR;
        }

        pthread_mutex_unlock (&TR_MUTEX);
}

/* ------------------------------------------------------------------------- */

/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
/* None */

/* ========================================================================= */
/* End of file */
