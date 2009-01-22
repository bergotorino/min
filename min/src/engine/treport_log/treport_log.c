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
 *  @file       treport_log.c
 *  @version    0.1
 *  @brief      This file contains implementation of treport loggin facility
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include <treport_log.h>
#include <data_api.h>
#include <min_text.h>

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
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/** Start time */
static struct timeval inittv;
/** End time */

char timestamp[20];
/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* None */

/* ======================== FUNCTIONS ====================================== */

int min_test_report_begin ()
{
        struct tm       now;
        gettimeofday (&inittv, NULL);
        localtime_r (&(inittv.tv_sec), &now);
        strftime (timestamp, 20, "%Y-%m-%d %H:%M:%S", &now);
        return 0;
}

/* ------------------------------------------------------------------------- */

int min_test_report_begin_set (const char *set_name)
{
        return 0;
}


/* ------------------------------------------------------------------------- */

int min_test_report_result (DLListIterator tc, const char *fname)
{
        DLListIterator  result_it;
        char            title_str[50];
        result_description_t result_desc;
        char           *path_name;
        char           *file_name;
        char           *slash;
        int             retval = 1;

        Text    *result = tx_create(" ");
        tx_c_prepend(result,timestamp);

        result_it = dl_list_head (tc_get_tr_list (tc));
        switch (tr_get_result_type (result_it)) {
        case TEST_RESULT_PASSED:
                tx_c_append(result,"Passed  ");
                retval = 0;
                break;
        case TEST_RESULT_FAILED:
                tx_c_append(result,"FAILED  ");
                break;
        case TEST_RESULT_CRASHED:
                tx_c_append(result,"Crashed ");
                break;
        case TEST_RESULT_ABORTED:
                tx_c_append(result,"Aborted ");
                break;
        case TEST_RESULT_TIMEOUT:
                tx_c_append(result,"Timeout ");
                break;
        case TEST_RESULT_NOT_RUN:
                tx_c_append(result,"N/A     ");
                break;
        default:
                tx_destroy(&result);
                return 1;
        }

        slash = strrchr (fname, '/');
        if (slash == NULL) {
                path_name = (char *)".";
                file_name = (char *)fname;
        } else {
                path_name = strdup (fname);
                path_name[slash - fname] = 0;
                file_name = slash + 1;
        }

        tc_get_title (tc, title_str);
        tx_c_append(result,file_name);
        tx_c_append(result," ");
        tx_c_append(result,title_str);
        tx_c_append(result," ");
        tr_get_result_description (result_it, result_desc);
        tx_c_append(result,result_desc);

        if (slash != NULL) { free (path_name); }

        printf("%s\n",tx_share_buf(result));
        tx_destroy(&result);

        return retval;
}
