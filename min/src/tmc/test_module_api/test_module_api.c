/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Konrad Marek Zapalowicz
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
 *  @file       test_module_api.c
 *  @version    0.1
 *  @brief      This file contains implementation of Test Module API
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <test_module_api.h>
#include <min_test_event_if.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* None */

/* ======================== FUNCTIONS ====================================== */

/* ------------------------------------------------------------------------- */
/** Checks if test case has a matching id
 *  @param a pointer to TestCaseInfo
 *  @param b pointer to id 
 *  @return 0 if id's match -1 otherwise
 */
int compare_id (const void *a, const void *b)
{
        TestCaseInfo   *tci = (TestCaseInfo *) a;
        int            *id = (int *)b;

        if (tci->id_ == *id)
                return 0;
        else
                return -1;
}

/* ------------------------------------------------------------------------- */
/** Gives the ability to print messages to the UI.
 *  @param priority message importance.
 *  @param desc short description.
 *  @param format message format.
 *  @param ... format parameters.
 *  @return 0, or -1 in case of failure.
 *
 *  NOTE: see: man 2 printf
 */
int tm_printf (int priority, char *desc, char *format, ...)
{
        int             retval = -1;
        int             mqid = -1;
        va_list         ap;
        MsgBuffer       buff;
        char            tmp_msg[MaxUsrMessage];

        if (strlen (desc) > (MaxUsrMessage - 1))
                desc[MaxUsrMessage - 1] = '\0';
        if (strlen (format) > (MaxUsrMessage - 1))
                format[MaxUsrMessage - 1] = '\0';

        va_start (ap, format);
	vsnprintf (tmp_msg, MaxUsrMessage, format, ap);

        buff.receiver_ = getppid ();
        buff.sender_ = getpid ();
        buff.type_ = MSG_USR;
        buff.param_ = priority;
        STRCPY (buff.desc_, desc, MaxUsrMessage);
        STRCPY (buff.message_, tmp_msg, MaxUsrMessage);

        mqid = mq_open_queue ('a');

        if (mqid != -1) {
                retval = 0;
                retval = mq_send_message (mqid, &buff);
        }

        va_end (ap);
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Prints error messages 
 *  @param format message format.
 *  @param ... format parameters.
 *  @return 0, or -1 in case of failure.
 *
 *  NOTE: see: man 2 printf
 */
int tm_print_err (char *format, ...)
{
        int             retval = -1;
        int             mqid = -1;
        va_list         ap;
        MsgBuffer       buff;
        char            tmp_msg[MaxUsrMessage];

        if (strlen (format) > (MaxUsrMessage - 1))
                format[MaxUsrMessage - 1] = '\0';

        va_start (ap, format);
	vsnprintf (tmp_msg, MaxUsrMessage, format, ap);

        buff.receiver_ = getppid ();
        buff.sender_ = getpid ();
        buff.type_ = MSG_USR;
        buff.param_ = 0;
        strcpy (buff.desc_, "__error_console__");
        STRCPY (buff.message_, tmp_msg, MaxUsrMessage);

        mqid = mq_open_queue ('a');

        if (mqid != -1) {
                retval = 0;
                retval = mq_send_message (mqid, &buff);
        }

        va_end (ap);
        return retval;
}


/* ------------------------------------------------------------------------- */
/** Logs the error produced by using one of TL macros.
 *  @param macro_type the type of macro that has been used.
 *  @param file name of the file in which error has occured.
 *  @param function name of the function in which error has occured.
 *  @param line number of the line in which error has occured.
 *  @param result the result that occurred.
 *  @param expectedN expeced value of the result
 */
void tm_macro_error (unsigned int macro_type,
                     const char *file,
                     const char *function,
                     unsigned int line,
                     int result,
                     int expected1,
                     int expected2,
                     int expected3, int expected4, int expected5)
{

        switch (macro_type) {
        case 0:                /* TL */
                MIN_INFO ("FAIL: MIN TF's macro. FILE[%s], "
                             "FUNCTION[%s], LINE[%d]", file, function, line);
                break;

        case 1:                /* T1L */
                MIN_INFO ("FAIL: MIN TF's macro. RECEIVED[%d], "
                             "EXPECTED[%d], FILE[%s], FUNCTION[%s], LINE[%d]",
                             result, expected1, file, function, line);
                break;

        case 2:                /* T2L */
                MIN_INFO ("FAIL: MIN TF's macro. RECEIVED[%d], "
                             "EXPECTED[%d], EXPECTED[%d], FILE[%s], "
                             "FUNCTION[%s], LINE[%d]",
                             result, expected1, expected2,
                             file, function, line);
                break;

        case 3:                /* T3L */
                MIN_INFO ("FAIL: MIN TF's macro. RECEIVED[%d], "
                             "EXPECTED[%d], EXPECTED[%d], EXPECTED[%d], "
                             "FILE[%s], FUNCTION[%s], LINE[%d]",
                             result, expected1, expected2, expected3,
                             file, function, line);
                break;

        case 4:                /* T4L */
                MIN_INFO ("FAIL: MIN TF's macro. RECEIVED[%d], "
                             "EXPECTED[%d], EXPECTED[%d], EXPECTED[%d], "
                             "EXPECTED[%d], FILE[%s], FUNCTION[%s], LINE[%d]",
                             result, expected1, expected2, expected3,
                             expected4, file, function, line);
                break;

        case 5:                /* T5L */
                MIN_INFO ("FAIL: MIN TF's macro. RECEIVED[%d], "
                             "EXPECTED[%d], EXPECTED[%d], EXPECTED[%d], "
                             "EXPECTED[%d], EXPECTED[%d], FILE[%s], "
                             "FUNCTION[%s], LINE[%d]",
                             result, expected1, expected2, expected3,
                             expected4, expected5, file, function, line);
                break;
        }
}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include <check.h>
#include "test_module_api.tests"
#endif                          /* MIN_UNIT_TEST */
/* ------------------------------------------------------------------------- */
/* End of file */
