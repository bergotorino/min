/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Robert Galkowski
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
 *  @file       tec.h
 *  @version    0.1
 *  @brief      This file contains header file of the tec.c. 
 *              It holds declaration of functions provided for User Interface
 */

#ifndef TEC_H
#define TEC_H


/* ----------------------------------------------------------------------------
 * INCLUDES 
 */
#include <pthread.h>            //threads creation and handling
#include <min_ipc_mechanism.h>
#include <min_logger.h>
#include <min_logger_output.h>
#include <min_common.h>
#include <min_test_event_if.h>
#include <min_parser.h>
#include <tec_rcp_handling.h>
#include <min_engine_api.h>
#include <dllist.h>
/* ----------------------------------------------------------------------------
 * CONSTANTS
 */

typedef struct {
        filename_t      tmc_app_path_;
        long            engine_pid_;
        DLList         *search_dirs;
        int             operation_mode_;
        int             sh_mem_id_;     /* must be kept, so that it is possible
                                         * to destroy the segment 
                                         * during cleanup */
} ec_settings_s;

typedef struct {
        /** id of host that sent the data*/
        int             sender_id_;
        /** name of requested/sent value*/
        char            data_name_[128];
        /** data received from external controller*/
        char            data_[128];
        /** adress of test process that requested data, 
         * 0 if data not requested yet*/
        long            requested_by_;
} received_data;


/* ----------------------------------------------------------------------------
 * GLOBAL VARIABLES
 * This chapter contains declaration of global variables that have to be 
 * available for entities other than test execution controller, e.g UI
 */

DLList         *instantiated_modules;   /*list of running test modules */
DLList         *selected_cases; /*list of test cases selected for execution */
DLList         *results;        /*list of result structures for executed cases.
                                 * Used for generating reports and presenting 
                                 * data to user */
char           *UiMessage;      /*Message text to be dispalyed on UI in 
                                 * case of fault */
int             mq_id;          /*message queue id */
ec_settings_s   ec_settings;    /*structure containing global settings */
int             own_id;
/* ----------------------------------------------------------------------------
 * MACROS
 */

/* ----------------------------------------------------------------------------
 * DATA TYPES
 */
/* ---------------------------------------------------------------------------
 * FORWARD DECLARATIONS
 */

/* ----------------------------------------------------------------------------
 * Structures
 */
/* ----------------------------------------------------------------------------
 * FUNCTION PROTOTYPES
 */
void            ec_min_init (char *envp_[], int operation_mode);
int             ec_start_modules ();
DLListIterator  ec_select_case (DLListIterator work_case_item, int group_id);
void            log_summary ();
int             log_summary_stdout ();
int             ec_exec_case (DLListIterator work_case_item);
int             ec_exec_test_case (DLListIterator work_case_item);
int             ec_pause_test_case (DLListIterator work_case_item);
int             ec_resume_test_case (DLListIterator work_case_item);
int             ec_abort_test_case (DLListIterator work_case_item);
int             ec_run_cases_seq (DLList * work_cases_list);
int             ec_run_cases_par (DLList * work_cases_list);
int             ec_set_write (DLList * cases_list);
void            ec_set_read (DLList * set_cases_list, char *setname);
int             ec_run_set_seq (DLList * work_cases_list);
int             ec_run_set_par (DLList * work_cases_list);
int             ec_read_settings (char *engine_ini);
void            ec_cleanup ();
/*write test set with default name */
int             ec_set_write (DLList * cases_list);
/*write set with given name */
int             ec_set_write_file (DLList * cases_list, char *filename);
/*add module at any time */
int             ec_add_module (TSChar * mod_name, DLList * testcase_files,
                               unsigned id);
/*create path to test set from current date/hour */
char           *create_path ();
/*search for file in defined search paths */
int             ec_search_lib (char *mod_name);
/* reads config files and do reports new modules (uses eapi) */
int ec_configure ();
/* -------------------------------------------------------------------------*/

#endif                          /* TEC_H */

/* End of file */
