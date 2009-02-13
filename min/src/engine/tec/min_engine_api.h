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
typedef struct  {
        void (*report_result) (unsigned moduleid, unsigned caseid, char *desc);
        void (*report_case_status) (unsinged moduleid, unsigned caseid, stat);
        void (*module_prints) (unsigned moduleid, unsigned caseid, 
                               char *message);
        void (*new_module) (char *modulename, unsigned moduleid);
        void (*no_module) (char *modulename);
        void (*new_case) (unsigned moduleid, unsigned caseid, char *casetitle);
} eapiIn_t;

typedef struct {
        void (*add_test_module) (char *modulepath);
        void (*add_test_case_file) (unsigned moduleid, char *testcasefile);
        void (*run_test) (unsigned moduleid, unsigned caseid);
} eapiOut_t;

/* ----------------------------------------------------------------------------
 * FUNCTION PROTOTYPES
 */
void eapi_init (eapiIn_t *in, eapiOut_t *out);

/* -------------------------------------------------------------------------*/

#endif                          /* MIN_ENGINE_API_H */

/* End of file */
