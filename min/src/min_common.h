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
 *  @file       min_common.h
 *  @version    0.1
 *  @brief      This file contains header file of the Common defs of MIN
 */

#ifndef MIN_COMMON_H
#define MIN_COMMON_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <stdlib.h>
#include <string.h>

#ifdef _UNICODE
#include <wchar.h>
#endif
/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/** No error definition */
#define ENOERR 0
/** MIN version */
#define MIN_VERSION_STR "min2009w10"
/* ------------------------------------------------------------------------- */
/** Max length of the Test Result description.
 */
#define MaxTestResultDescription        128
/* ------------------------------------------------------------------------- */
/** Max lenght of the Test Case name.
 */
#define MaxTestCaseName                 256
/* ------------------------------------------------------------------------- */
/** Max length of the file name, which includes path.
 *  Designed for:
 *  - cfg files
 *  - test libraries
 */
#define MaxFileName                     512
/* ------------------------------------------------------------------------- */
/** Max length of the user message parts.
 */
#define MaxUsrMessage                   512
/* ------------------------------------------------------------------------- */
/** Initial value that should be used for all pointers. Introduced to make
 *  debugging easier.
 */
#define INITPTR         ((void *)0xDEADBEEF)
/* ------------------------------------------------------------------------- */
/** We use it to indicate that sth is used only in the scope of the file
    it is introduced.
  */
#define LOCAL           static
/* ------------------------------------------------------------------------- */
/** Copies one string to another in safe way.
 *  @param d destination string
 *  @param s source string
 *  @param n max length of the destination string
 *
 *  Copies whole source string if it is shorter than max size of destination,
 *  otherwise copies n chcracter from the source string.
 */
#define STRCPY(d,s,n)   strncpy(d,s,(strlen(s)>(n-1))?n:strlen(s)+1);
/* ------------------------------------------------------------------------- */
/** Wraps around malloc to make it simpler to use.
 *  @param _object_ type of variable to be allocated
 */
#define NEW(_object_)   (_object_*)malloc(sizeof(_object_));
/* ------------------------------------------------------------------------- */
/** Wraps around malloc to make it simpler to use. This version of macro 
 * facilitates allocation for array of objects.
 *  @param _object_ type of variable to be allocated
 *  @param _size_ amount of memmory to be alocated
 */
#define NEW2(_object_,_size_)   (_object_*)malloc(sizeof(_object_)*(_size_))
/* ------------------------------------------------------------------------- */
/** Wraps around free to make it simpler to use.
 *  @param _var_ pointer to be deallocated
 */
#define DELETE(_var_)   free(_var_);_var_=INITPTR;
/* ------------------------------------------------------------------------- */
#ifndef MAX
#define MAX(a,b)        (((a)>(b))?(a):(b))
#endif /*MAX*/
#define MAX4(a,b,c,d)   MAX(MAX((a),(b)),MAX((c),(d)))
/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* ------------------------------------------------------------------------- */
#ifdef _UNICODE
typedef wchar_t TSChar;
#else
typedef char    TSChar;
#endif                          /* _UNICODE */
/* ------------------------------------------------------------------------- */
/** General filename string type definition for file input/output operations
*/
typedef char    filename_t[MaxFileName];

/* ------------------------------------------------------------------------- */
/** Test Process result */
typedef enum {
        TP_CRASHED = -2,        /**< Test Process has crashed */
        TP_TIMEOUTED = -1,      /**< Test Process has timeouted */
        TP_PASSED = 0,          /**< Test Process has passed */
        TP_FAILED = 1,          /**< Test Process has failed */
        TP_NC = 2,              /**< Test Process has been not completed */
        /** Test Process has exited because an error occured in the
         *  component under test
         */
        TP_LEAVE = 3
} TPResult;
/* ------------------------------------------------------------------------- */
/** Master/Slave commands */
typedef enum {
        EAllocateSlave,         /**< Allocating slave           */
        EFreeSlave,             /**< Freeing slave              */
        ERemoteSlave,           /**< Remote command execution   */
        EResponseSlave,         /**< Response on any cmd        */
        ERemoteSlaveResponse    /**< Response after test end    */
} TMSCommand;
/* ------------------------------------------------------------------------- */
/** Boolean type */
typedef enum {
        ESFalse = 0,  /**< Indicates false */
        ESTrue = 1    /**< Indicates true */
} TSBool;
/* ------------------------------------------------------------------------- */
/* Test module type */
typedef enum {
        EHardcoded      = 0x1,
        ENormal         = 0x2,
        ESUnit          = 0x3,
        ETestClass      = 0x4,
        ELuaTestClass   = 0x5
} TTestModuleType;
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
#endif                          /* MIN_COMMON_H */
/* End of file */
