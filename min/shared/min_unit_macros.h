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
#include <test_module_api.h>

/* ------------------------------------------------------------------------- */
/* CONSTANTS */

/* ------------------------------------------------------------------------- */
/* MACROS */
#define MIN_LOG min_info

#define MIN_TESTDEFINE(__test_name__) \
__test_case_index__++; \
if (__action__ == GET_CASES) { \
      ENTRY(*__cases__,#__test_name__,NULL); \
} else if (__action__ == RUN_CASE && __test_case_index__ == __id__)


#define MIN_SETUP if (__action__ == RUN_SETUP)

#define MIN_TEARDOWN if (__action__ == RUN_TEARDOWN)


/******************************************************************************
 * Assert Macros
 *****************************************************************************/
/** used by other ASSERT macros.  */
#define __MIN_ASSERT_SHARED(__assertion__, __aMessage__) \
if(!__assertion__) { \
		min_debug(__FUNCTION__, __LINE__,__FILE__, __aMessage__);\
	        RESULT(__result__,TP_FAILED,__aMessage__);\
}
/** used by other ASSERT macros.  */
#define __MIN_ASSERT_SHARED_DESC(__assertion__, __aMessage__, __aDesc__) \
if(!__assertion__) { \
		min_debug(__FUNCTION__, __LINE__,__FILE__, __aMessage__);\
	        RESULT2(__result__,TP_FAILED,__aMessage__, __aDesc__);\
}

/** assert fails if the arguments are not equal  */
#define MIN_ASSERT_EQUALS(aExpected, aActual) \
	__MIN_ASSERT_SHARED(((aExpected) == (aActual)) , \
			      "AssertEquals failed");

/** assert fails if the arguments are not equal, with desc  */
#define MIN_ASSERT_EQUALS_DESC(aExpected, aActual, aDesc) \
	__MIN_ASSERT_SHARED_DESC(((aExpected) == (aActual)) , \
			      "AssertEquals failed", aDesc);


/** assert fails if the arguments are equal  */
#define MIN_ASSERT_NOT_EQUALS(aExpected, aActual) \
	__MIN_ASSERT_SHARED(((aExpected) != (aActual)) , \
                              "AssertNotEquals failed");

/** assert fails if the arguments are equalm with desc  */
#define MIN_ASSERT_NOT_EQUALS_DESC(aExpected, aActual, aDesc) \
	__MIN_ASSERT_SHARED_DESC(((aExpected) != (aActual)) , \
                              "AssertNotEquals failed", aDesc);


/** assert fails if aPtr is not NULL  */
#define MIN_ASSERT_NULL(aPtr) \
        __MIN_ASSERT_SHARED(((aPtr) == NULL), "AssertNull Failed");

/** assert fails if aPtr is not NULL, with desc  */
#define MIN_ASSERT_NULL_DESC(aPtr, aDesc) \
        __MIN_ASSERT_SHARED_DESC(((aPtr) == NULL), "AssertNull Failed",\
                                  aDesc);

/** assert fails if aPtr is  NULL */
#define MIN_ASSERT_NOT_NULL(aPtr) \
        __MIN_ASSERT_SHARED(((aPtr) != NULL), "AssertNotNull Failed");


/** assert fails if aPtr is  NULL, with desc */
#define MIN_ASSERT_NOT_NULL_DESC(aPtr, aDesc) \
        __MIN_ASSERT_SHARED_DESC(((aPtr) != NULL),\
                                 "AssertNotNull Failed", aDesc);

/** assert fails if pointers are not the same */
#define MIN_ASSERT_SAME(aExpectedPtr, aActualPtr) \
        __MIN_ASSERT_SHARED(((aExpectedPtr) == (aActualPtr)),\
  "AssertSame Failed");

/** assert fails if pointers are not the same, with desc */
#define MIN_ASSERT_SAME_DESC(aExpectedPtr, aActualPtr, aDesc) \
        __MIN_ASSERT_SHARED(((aExpectedPtr) == (aActualPtr)),\
  "AssertSame Failed", aDesc);

/** assert fails if pointers are the same */
#define MIN_ASSERT_NOT_SAME(aExpectedPtr, aActualPtr) \
      __MIN_ASSERT_SHARED(((aExpectedPtr) != (aActualPtr)),\
                            "AssertNotSame Failed");

/** assert fails if pointers are the same, with desc */

#define MIN_ASSERT_NOT_SAME_DESC(aExpectedPtr, aActualPtr, aDesc) \
      __MIN_ASSERT_SHARED_DESC(((aExpectedPtr) != (aActualPtr)), \
                               "AssertNotSame Failed", aDesc);

/** assert fails if aCondition is not TRUE */
#define MIN_ASSERT_TRUE(aCondition) \
        __MIN_ASSERT_SHARED((aCondition), "AssertTrue Failed" );

/** assert fails if aCondition is not TRUE, with description */
#define MIN_ASSERT_TRUE_DESC(aCondition, aDesc) \
        __MIN_ASSERT_SHARED_DESC((aCondition), "AssertTrue Failed", aDesc);

/** assert fails if aCondition is not FALSE */
#define MIN_ASSERT_FALSE(aCondition) \
        __MIN_ASSERT_SHARED(!(aCondition), "AssertFalse Failed");

/** assert fails if aCondition is not FALSE, with description */
#define MIN_ASSERT_FALSE_DESC(aCondition, aDesc) \
        __MIN_ASSERT_SHARED_DESC(!(aCondition), "AssertFalse Failed", aDesc);


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
