/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Robert Galkowski
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
 *  @file       mintfwif.h
 *  @version    0.1
 *  @brief      This file contains header file of the mintfwif
 */

#ifndef MINTFWIF_H
#define MINTFWIF_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#if 0
#include <tec.h>
#include <tec_rcp_handling.h>
#include <min_common.h>
#endif
/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/** Test case complete callback function */
typedef void    (*min_case_complete_func) (int run_id, int execution_result,
					   int test_result, char *desc);
/** Test case print callback function */
typedef void    (*min_case_print_func) (int run_id, char *text);
/** External controller message sending function */
typedef void    (*min_extif_message_cb_) (char *message, int length);

/** Container for callbacks */
typedef struct {
        min_case_complete_func complete_callback_;
        min_case_print_func print_callback_;
        min_extif_message_cb_ send_extif_msg_;
} tfwif_callbacks_s;

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */
/* DLList         *user_selected_cases; */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */

/** Structure for test caser data storage*/
typedef struct {
        /**test case id, unique in module scope*/
        int             case_id_;
        /**title of test case - NULL terminated string*/
        char            case_name_[256];
} min_case;
/* ------------------------------------------------------------------------- */

/** Structure for storage of test module data*/
typedef struct {
        /** Module name - null terminated string */
        char            module_name_[128];
        /** Number of test cases */
        unsigned int    num_test_cases_;
        /** Array of min_case structures - collection of test cases
        in module*/
        min_case      *test_cases_;
} module_info;

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
int             min_if_open (min_case_complete_func complete_cb,
                             min_case_print_func print_cb,
                             min_extif_message_cb_ extifsend_cb,
                             char *engine_ini, char *envp[]);
/* ------------------------------------------------------------------------- */
int             min_if_close ();
/* ------------------------------------------------------------------------- */
int             min_if_get_cases (module_info ** modules);
/* ------------------------------------------------------------------------- */
int             min_if_exec_case (char *module, unsigned int id);
/* ------------------------------------------------------------------------- */
int             min_if_cancel_case (unsigned int runtime_id);
/* ------------------------------------------------------------------------- */
int             min_if_pause_case (unsigned int runtime_id);
/* ------------------------------------------------------------------------- */
int             min_if_resume_case (unsigned int runtime_id);
/* ------------------------------------------------------------------------- */
int             min_if_message_received (char *message, int length);
/* ------------------------------------------------------------------------- */
int             min_if_set_device_id (int device_id);
/* ------------------------------------------------------------------------- */
int             min_if_module_add (char *module_name, char *conf_name);
/* ------------------------------------------------------------------------- */
#endif                          /* MINTFWIF_H */
/* End of file */
