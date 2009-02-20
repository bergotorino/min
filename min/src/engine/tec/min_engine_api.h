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
 *  @file       min_engine_api.h
 *  @version    0.1
 *  @brief      
 */

#ifndef MIN_ENGINE_API_H
#define MIN_ENGINE_API_H

/* ----------------------------------------------------------------------------
 * INCLUDES 
 */
/* ----------------------------------------------------------------------------
 * CONSTANTS
 */
/* ----------------------------------------------------------------------------
 * GLOBAL VARIABLES
 */
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

/** Callbacks from plugin called by Engine */
typedef struct  {
        void (*case_result) (long test_run_id, int result, char *desc);
        void (*new_module) (char *modulename, unsigned module_id);
        void (*no_module) (char *modulename);
        void (*module_ready) (unsigned module_id);
        void (*new_case) (unsigned module_id, unsigned case_id, 
			  char *casetitle);
        void (*case_started) (unsigned module_id, unsigned case_id, 
			      long test_run_id);
        void (*case_paused) (long test_run_id);
        void (*case_resumed) (long test_run_id);
        void (*module_prints) (long test_run_id, char *message);
} eapiIn_t;

/** Callbacks to functions from engine called by plugin */
typedef struct {
        void (*add_test_module) (char *modulepath);
        void (*add_test_case_file) (unsigned moduleid, char *testcasefile);
        void (*start_case) (unsigned moduleid, unsigned caseid);
        void (*pause_case) (long test_run_id);
        void (*resume_case) (long test_run_id);
        void (*abort_case) (unsigned moduleid, unsigned caseid);
        void (*fatal_error) (char *what, char *error_string);
} eapiOut_t;

/* ----------------------------------------------------------------------------
 * FUNCTION PROTOTYPES
 */
void eapi_init (eapiIn_t *in, eapiOut_t *out);

/* -------------------------------------------------------------------------*/

#endif                          /* MIN_ENGINE_API_H */

/* End of file */
