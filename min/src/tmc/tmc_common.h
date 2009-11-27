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
 *  @file       tmc_common.h
 *  @version    0.1
 *  @brief      This file contains header file of the common definitions of TMC
 */

#ifndef TMC_COMMON_H
#define TMC_COMMON_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <min_common.h>
#include <dllist.h>
#include <min_logger.h>

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* ------------------------------------------------------------------------- */
/** Usefull type definitions { */
typedef struct _TestCaseInfo TestCaseInfo;
typedef struct _TestCaseResult TestCaseResult;
typedef struct _TestProcessInfo TestProcessInfo;
typedef struct _AsyncOpFlags AsyncOpFlags;
/**}*/
/* ------------------------------------------------------------------------- */
/** Pointer to the user provided get_test_cases function */
typedef int     (*ptr2case) (const char *, DLList **);
/* ------------------------------------------------------------------------- */
/** Pointer to the user provided run_case function */
typedef int     (*ptr2run) (unsigned int, const char *, TestCaseResult *);
/* ------------------------------------------------------------------------- */
/** Pointer to the test case function. */
typedef int     (*ptr2test) (TestCaseResult * tcr);
/* ------------------------------------------------------------------------- */
/** Status of the Test Process */
typedef enum {
        TP_NONE = 1,            /**< No test is runing */
        TP_RUNNING = 2,         /**< Test is runing    */
        TP_PAUSED = 3,          /**< Test is paused    */
        TP_ENDED = 4,           /**< Test is ended     */
        TP_TIMEOUT = 5,         /**< Test is timeouted */
        TP_ABORTED = 6,         /**< Test has been aborted */
        TP_CANCELED = 7         /**< Test has beed canceled */
} MINTPStatus;
/* ------------------------------------------------------------------------- */
/** Test Process exit reasons */
typedef enum {
        TP_EXIT_SUCCESS = 0,    /**< Test Process exits gracefully */
        TP_EXIT_FAILURE = -1    /**< Test Process sigsegvs */
} TPExitCode;
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/** Detailed info about a single Test Case. */
struct _TestCaseInfo {
        /** Test Case name which must be unique in the scope of test module! */
        char            name_[MaxTestCaseName];
        /** pointer to test function. */
        ptr2test        test_;
        /** id of the test case */
        unsigned int    id_;
	/** Test Case Description */
	char            desc_[MaxUsrMessage];
};
/* ------------------------------------------------------------------------- */
/** Result of a single test case. */
struct _TestCaseResult {
        /** Result of the test case */
        int             result_;
        /** Short description of the result */
        char            desc_[MaxTestResultDescription];
};
/* ------------------------------------------------------------------------- */
/** Detailed information about ongoing test case. */
struct _TestProcessInfo {
        /** The PID of the Test Process */
        long            pid_;
        /** Status of the Test Process */
        MINTPStatus    status_;
};
/* ------------------------------------------------------------------------- */
/** Structure containng flags to control asynchronous operations*/
struct _AsyncOpFlags {
        /** testcase from other module started from 
        python case is ongoing*/
        TSBool parallel_test_ongoing_;
        /** wait for answer from external controller*/
        TSBool extif_response_;
        /** list of ids of testcases running remotely*/
        DLList* remote_cases_list_;
};
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
#endif                          /* TMC_COMMON_H */
/* End of file */
