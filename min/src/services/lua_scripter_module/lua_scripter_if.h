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
 *  @file       lua_scripter_if.h
 *  @version    0.1
 *  @brief      This file contains header file for the lua scripter
 */

#ifndef LUA_SCRIPTER_IF_H
#define LUA_SCRIPTER_IF_H

/* -------------------------------------------------------------------------- */
/* INCLUDES */
/* -------------------------------------------------------------------------- */
#include <stdlib.h>
#include <stdio.h>

#include <min_common.h>        /* common MIN stuff */
#include <min_ipc_mechanism.h> /* IPC stuff */
#include <test_module_api.h>    /* TestCaseResult */
#include <min_system_logger.h> /* MIN_DEBUG1 */
#include <min_test_event_if.h> /* Event system */
#include <dlfcn.h>              /* dlopen/dlclose/dlsym */
#include <dllist.h>             /* DLList/DLListIterator */
#include <min_text.h>               /* Text interface */
#include <lauxlib.h>            /* Lua auxiliary stuff */
#include <lstate.h>             /* lua_State */
#include <lualib.h>             /* Lua library operations */
/* -------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* -------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* -------------------------------------------------------------------------- */
/* DATA TYPES */
/** Pointer to get function from Lua Test Module. */
typedef int (*ptr2get)( DLList** );
/** Pointer to get function from Lua. */
typedef int (*ptr2luatc)( lua_State* );
/* -------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* -------------------------------------------------------------------------- */
/* STRUCTURES */
/* -------------------------------------------------------------------------- */
/** Describes test function which is provided by the end user. */
typedef struct
{
        char            name_[256];     /**< Test function name */
        ptr2luatc       test_;          /**< Pointer to test function */
        unsigned int    id_;            /**< Id of the test function */
        char            desc_[512];     /**< Test function description */
} TCInfo;
/* -------------------------------------------------------------------------- */
/** Describes slave */
typedef struct
{
        char name_[128];        /**< Slave name */
} SlaveInfo;
/* -------------------------------------------------------------------------- */
/** Describes test module, structure to be exported to lua */
typedef struct
{
        void *dllhnd_;          /**< handler to the dll. */
        char name_[128];        /**< test library file name. */
} TMInfo;
/* -------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* -------------------------------------------------------------------------- */
int tm_run_test_case( unsigned int      id
                    , const char     *  scriptfile
                    , TestCaseResult *  result );
/* -------------------------------------------------------------------------- */
int tm_get_test_cases( const char * scriptfile, DLList ** cases );
/* -------------------------------------------------------------------------- */
#endif /* LUA_SCRIPTER_IF_H */

/* End of file */

