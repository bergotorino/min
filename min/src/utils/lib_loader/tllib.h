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
 *  @file       tllib.h
 *  @version    0.1
 *  @brief      This file contains header file of the MIN test lib loader
 */

#ifndef TLLIB_H
#define TLLIB_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <dlfcn.h>
#include <tmc_common.h>

/* ------------------------------------------------------------------------- */
/* CONSTANTS */

/* ------------------------------------------------------------------------- */
/* MACROS */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
typedef unsigned int (*ptr2internals)();
typedef char* (*ptr2internals2)();
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */

/* ------------------------------------------------------------------------- */
/* Structures */
/** Structure for holding stuff directly connected to test library loading */
typedef struct {
        /** Path of the test library */
	char fname_ [255];
        /** Handler to the test ibrary */
	void *test_library_;
        /** Pointer to tm_get_test_cases() in the test lib */
	ptr2case get_cases_fun_;
        /** Pointer to tm_run_test_case() in the test lib */
	ptr2run run_case_fun_;
        /** Type of the module. */
        char type_[16];
        /** Version of the module. */
        int version_;
        /** Build date */
        char date_[12];
        /** Build time */
        char time_[9];
} test_libl_t;

typedef test_libl_t TestLibraryLoader;
/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
int tl_open  (test_libl_t *tlibl, const char *lib_name);
/* ------------------------------------------------------------------------- */
void* tl_open_tc( const char* file );
/* ------------------------------------------------------------------------- */
int tl_close (test_libl_t *tlibl);
/* ------------------------------------------------------------------------- */
int tl_is_ok (test_libl_t *tlibl);
/* ------------------------------------------------------------------------- */
#endif /* TLLIB_H */
/* End of file */

