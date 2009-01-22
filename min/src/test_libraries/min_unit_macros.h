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
 *  @file       min_unit_macros.h
 *  @version    0.1
 *  @brief      This file contains macro definitions used in MINUnit
 */

#ifndef MIN_UNIT_MACROS_H
#define MIN_UNIT_MACROS_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include "../tmc/test_module_api/test_module_api.h"

/* ------------------------------------------------------------------------- */
/* CONSTANTS */

/* ------------------------------------------------------------------------- */
/* MACROS */
#define MIN_LOG min_info

#define MIN_TESTDEFINE( __test_name__ ) \
__test_case_index__++; \
if (__action__ == GET_CASES) { \
      ENTRY(*__cases__,#__test_name__,NULL); \
} else if (__action__ == RUN_CASE && __test_case_index__ == __id__ )

#define MIN_SETUP if (__action__ == RUN_SETUP)

#define MIN_TEARDOWN if (__action__ == RUN_TEARDOWN)


/******************************************************************************
 * Assert Macros
 *****************************************************************************/
/** used by other ASSERT macros.  */
#define __MIN_ASSERT_SHARED( __assertion__, __aMessage__ ) \
if(!__assertion__) { \
		min_debug(__FUNCTION__, __LINE__, __FILE__, __aMessage__);\
	        RESULT(__result__,TP_FAILED,__aMessage__);\
}
/** assert fails if the arguments are not equal  */
#define MIN_ASSERT_EQUALS( aExpected, aActual ) \
	__MIN_ASSERT_SHARED( ( (aExpected) == (aActual) ) , \
			      "AssertEquals failed" );

/** assert fails if the arguments are equal  */
#define MIN_ASSERT_NOT_EQUALS( aExpected, aActual ) \
	__MIN_ASSERT_SHARED( ( (aExpected) != (aActual) ) , \
 "AssertNotEquals failed" );

/** assert fails if aPtr is not NULL  */
#define MIN_ASSERT_NULL( aPtr) \
        __MIN_ASSERT_SHARED( ( ( aPtr ) == NULL), "AssertNull Failed" );

/** assert fails if aPtr is  NULL */
#define MIN_ASSERT_NOT_NULL( aPtr ) \
        __MIN_ASSERT_SHARED( ( ( aPtr ) != NULL), "AssertNotNull Failed" );

/** assert fails if pointers are not the same */
#define MIN_ASSERT_SAME( aExpectedPtr, aActualPtr ) \
        __MIN_ASSERT_SHARED( ( (aExpectedPtr) == (aActualPtr) ),\
  "AssertSame Failed" );

/** assert fails if pointers are the same */
#define MIN_ASSERT_NOT_SAME( aExpectedPtr, aActualPtr) \
      __MIN_ASSERT_SHARED( ( (aExpectedPtr) != (aActualPtr) ),\
  "AssertNotSame Failed" );

/** assert fails if aCondition is not TRUE */
#define MIN_ASSERT_TRUE( aCondition ) \
        __MIN_ASSERT_SHARED( ( aCondition ), "AssertTrue Failed"  );

/** assert fails if aCondition is not FALSE */
#define MIN_ASSERT_FALSE( aCondition ) \
        __MIN_ASSERT_SHARED( !(aCondition), "AssertFalse Failed" );



/* ------------------------------------------------------------------------- */
/* DATA TYPES */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */

/* ------------------------------------------------------------------------- */
/* Structures */

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */


/* ------------------------------------------------------------------------- */

#endif                          /* MIN_UNIT_MACROS_H */

/* End of file */
