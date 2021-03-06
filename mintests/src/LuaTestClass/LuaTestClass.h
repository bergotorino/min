/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Konrad Marek Zapalowicz
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
 *  @file       LuaTestClass.h
 *  @version    0.1
 *  @brief      This file contains header file of the LuaTestClass 
 *              test module of MIN Test Framework.
 */

#ifndef LUATESTCLASS_H
#define LUATESTCLASS_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <test_module_api.h>
#include <min_parser.h>
#include <min_logger.h>
#include <min_test_event_if.h>

#include "lstate.h"

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* ------------------------------------------------------------------------- */
#ifdef ENTRYTC
#undef ENTRYTC
#endif
/* ------------------------------------------------------------------------- */
/** Macro that makes adding test cases easier.
 *  @param _l_ pointer to list on which new test case will be stored
 *  @param _n_ is the name of the test case
 *  @param _f_ is the name of test function
 */
#define ENTRYTC(_l_, _n_,_f_)                                           \
        do {                                                            \
        TestCaseInfoTC* tc = (TestCaseInfoTC*)malloc(sizeof(TestCaseInfoTC));\
        if( tc == NULL ) break;                                         \
	if (strlen (_n_) > MaxTestCaseName - 1)                         \
		strcpy (tc->name_, "<too long method name>");		\
	else								\
		strcpy (tc->name_,_n_);					\
        tc->test_ = _f_;                                                \
        tc->id_   = dl_list_size(_l_)+1;                                \
        dl_list_add( _l_, (void*)tc );                                  \
        } while(0)
/* ------------------------------------------------------------------------- */
#define RESULT_NUMBER(_val_)    					\
	do {								\
	lua_pushnumber(L,_val_);					\
	} while(0)
/* ------------------------------------------------------------------------- */
#define RESULT_STRING(_val_)    					\
	do {								\
	lua_pushstring(L,_val_);					\
	} while(0)
/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* ------------------------------------------------------------------------- */
/** Usefull type definitions. */
typedef struct _TestCaseInfoTC TestCaseInfoTC;
/* ------------------------------------------------------------------------- */
/** Pointer to the test case function that can be called from Lua. */
typedef int (*ptr2luatc)( lua_State * );
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* ------------------------------------------------------------------------- */
/** Detailed info about a single Test Case. 
 *  Used only in Test Class!
 */
struct _TestCaseInfoTC
{
        /** Test Case name which must be unique in the scope of test module! */
        char name_[MaxTestCaseName];
        /** pointer to test function. */
        ptr2luatc test_;
        /** id of the test case */
        unsigned int id_;
};
/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
int ts_run_method( MinItemParser* mip );
/* ------------------------------------------------------------------------- */
int ts_get_test_cases( DLList** list );
/* ------------------------------------------------------------------------- */
#endif /* LUATESTCLASS_H */
/* End of file */

