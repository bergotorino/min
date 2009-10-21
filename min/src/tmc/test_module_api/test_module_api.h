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
 *  @file       test_module_api.h
 *  @version    0.1
 *  @brief      This file contains header file of the Test Module API
 */

#ifndef TEST_MODULE_API_H
#define TEST_MODULE_API_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <dllist.h>
#include <min_ipc_mechanism.h>
#include <tmc.h>
#include <tmc_common.h>

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* ------------------------------------------------------------------------- */
#ifdef ENTRY
#undef ENTRY
#endif

#ifdef ENTRY2
#undef ENTRY2
#endif

#ifdef TL
#undef TL
#endif

#ifdef T1L
#undef T1L
#endif

#ifdef T2L
#undef T2L
#endif

#ifdef T3L
#undef T3L
#endif

#ifdef T4L
#undef T4L
#endif

#ifdef T5L
#undef T5L
#endif
/* ------------------------------------------------------------------------- */
/** Macro that makes adding test cases easier.
 *  @param _l_ pointer to list on which new test case will be stored
 *  @param _n_ is the name of the test case
 *  @param _f_ is the name of test function
 */
#define ENTRY(_l_, _n_,_f_)                                             \
        do {                                                            \
        TestCaseInfo* tc = (TestCaseInfo*)malloc(sizeof(TestCaseInfo)); \
        if( tc == NULL ) break;                                         \
	if (strlen (_n_) > MaxTestCaseName - 1)                         \
		strcpy (tc->name_, "<too long test case name>");        \
	else								\
		strcpy (tc->name_, _n_);				\
        tc->test_ = _f_;                                                \
        tc->id_   = dl_list_size(_l_)+1;                                \
        dl_list_add( _l_, (void*)tc );                                  \
        } while(0)
/* ------------------------------------------------------------------------- */
/** Macro that makes adding a new test case easier
 *  @param _l_ pointer to list on which new test case will be stored
 *  @param _n_ test case name
 *  @param _id_ test case id.
 *
 *  NOTE: The test case id must be > 0 and unique in the scope of config file.
 */
#define ENTRY2( _l_, _n_, _id_ )                                        \
        do {                                                            \
        TestCaseInfo* tc = NEW(TestCaseInfo);                           \
        if( tc == NULL ) break;                                         \
	if (strlen (_n_) > MaxTestCaseName - 1)                         \
		strcpy (tc->name_, "<too long test case name>");        \
	else								\
		strcpy (tc->name_, _n_);				\
        tc->test_ = (ptr2test)INITPTR;                                  \
        tc->id_   = _id_;                                               \
        dl_list_add( _l_, (void*)tc );                                  \
        } while(0);
/* ------------------------------------------------------------------------- */
/** Macro that makes reporting the test case result eastier
 *  @param _tcr_ is the pointer to the TestCaseResult structure
 *  @param _r_ is the test result
 *  @param _d_ is the description of the test case result
 */
#define RESULT(_tcr_,_r_,_d_)                                           \
        do {                                                            \
        _tcr_->result_ = _r_;                                           \
	if (strlen (_d_) > MaxTestResultDescription - 1)		\
		strcpy (_tcr_->desc_, "<too long result description>"); \
	else								\
		strcpy(_tcr_->desc_,_d_);				\
        return 0;                                                       \
        } while(0);
/* ------------------------------------------------------------------------- */
#define RESULT2(_tcr_,_r_,_d_,...)                                         \
        do {                                                               \
		char tmp[MaxTestResultDescription];			   \
		char* hack = &_d_[0];					   \
		if(strlen(hack)>(MaxTestResultDescription-1)) {		   \
			hack[MaxTestResultDescription-1] = '\0';	   \
		}							   \
		snprintf(tmp,MaxTestResultDescription-1,hack,__VA_ARGS__); \
		_tcr_->result_ = _r_;					   \
		STRCPY(_tcr_->desc_,tmp,MaxTestResultDescription);	   \
		return 0;						   \
        } while(0);
/* ------------------------------------------------------------------------- */
/** Macro for veryfy boolean value.
 *  @param _istrue_ [in] value which is checked against value of 1.
 *
 *  NOTE: In C language there is no bool type, we assume: true > 0, false=0
 *
 *  Example for use:
 *  - TL( Open() == KErrNone );
 *  - TL( VerifyString1() == VerifyString2() );
 *  - TL( 6 <= 8 );
 */
#define TL( _istrue_ ) do {                                             \
        if( (!(_istrue_)) ) {                                           \
        tm_macro_error( (0)                                             \
                      , __FILE__                                        \
                      , __FUNCTION__                                    \
                      , __LINE__                                        \
                      , _istrue_                                        \
                      , 0                                               \
                      , 0                                               \
                      , 0                                               \
                      , 0                                               \
                      , 0);                                             \
        RESULT(tcr,TP_LEAVE,"Error occured in component under test");   \
        }} while(0);
/* ------------------------------------------------------------------------- */
/** Macro for verify integer value from the expected 1 value.
 *  @param _result_ [in] result value
 *  @param _expected1_ [in] expected result value
 *
 *  Example:
 *  - T1L( Open(), EPERM );
 */
#define T1L( _result_, _expected1_ ) do {                               \
        if( ( (_result_) != (_expected1_) ) ) {                         \
        tm_macro_error( (1)                                             \
                      , __FILE__                                        \
                      , __FUNCTION__                                    \
                      , __LINE__                                        \
                      , _result_                                        \
                      , _expected1_                                     \
                      , 0                                               \
                      , 0                                               \
                      , 0                                               \
                      , 0);                                             \
        }} while(0);
/* ------------------------------------------------------------------------- */
/** Macro for verify integer value from the expected 2 values.
 *  @param _result_ [in] result value
 *  @param _expected1_ [in] expected result value
 *  @param _expected2_ [in] expected result value
 *
 *  Example:
 *  - T1L( Open(), EPERM, ENOENT );
 */
#define T2L( _result_, _expected1_, _expected2_ ) do {                  \
        if( ( (_result_) != (_expected1_) &&                            \
              (_result_) != (_expected2_) ) ) {                         \
        tm_macro_error( (2)                                             \
                      , __FILE__                                        \
                      , __FUNCTION__                                    \
                      , __LINE__                                        \
                      , _result_                                        \
                      , _expected1_                                     \
                      , _expected2_                                     \
                      , 0                                               \
                      , 0                                               \
                      , 0);                                             \
        }} while(0);
/* ------------------------------------------------------------------------- */
/** Macro for verify integer value from the expected 2 values.
 *  @param _result_ [in] result value
 *  @param _expected1_ [in] expected result value
 *  @param _expected2_ [in] expected result value
 *  @param _expected3_ [in] expected result value
 *
 *  Example:
 *  - T1L( Open(), EPERM, ENOENT, EACCES );
 */
#define T3L( _result_, _expected1_, _expected2_, _expected3_ ) do {     \
        if( ( (_result_) != (_expected1_) &&                            \
              (_result_) != (_expected2_) &&                            \
              (_result_) != (_expected3_) ) ) {                         \
        tm_macro_error( (3)                                             \
                      , __FILE__                                        \
                      , __FUNCTION__                                    \
                      , __LINE__                                        \
                      , _result_                                        \
                      , _expected1_                                     \
                      , _expected2_                                     \
                      , _expected3_                                     \
                      , 0                                               \
                      , 0);                                             \
        }} while(0);
/* ------------------------------------------------------------------------- */
/** Macro for verify integer value from the expected 2 values.
 *  @param _result_ [in] result value
 *  @param _expected1_ [in] expected result value
 *  @param _expected2_ [in] expected result value
 *  @param _expected3_ [in] expected result value
 *  @param _expected4_ [in] expected result value
 *
 *  Example:
 *  - T1L( Open(), EPERM, ENOENT, EACCES, EBUSY );
 */
#define T4L( _result_, _expected1_, _expected2_, _expected3_, _expected4_ ) do {\
        if( ( (_result_) != (_expected1_) &&                            \
              (_result_) != (_expected2_) &&                            \
              (_result_) != (_expected3_) &&                            \
              (_result_) != (_expected4_) ) ) {                         \
        tm_macro_error( (4)                                             \
                      , __FILE__                                        \
                      , __FUNCTION__                                    \
                      , __LINE__                                        \
                      , _result_                                        \
                      , _expected1_                                     \
                      , _expected2_                                     \
                      , _expected3_                                     \
                      , _expected4_                                     \
                      , 0);                                             \
        }} while(0);
/* ------------------------------------------------------------------------- */
/** Macro for verify integer value from the expected 2 values.
 *  @param _result_ [in] result value
 *  @param _expected1_ [in] expected result value
 *  @param _expected2_ [in] expected result value
 *  @param _expected3_ [in] expected result value
 *  @param _expected4_ [in] expected result value
 *  @param _expected5_ [in] expected result value
 *
 *  Example:
 *  - T1L( Open(), EPERM, ENOENT, EACCES, EBUSY, ENOSPC );
 */
#define T5L( _result_, _expected1_, _expected2_, _expected3_, _expected4_, _expected5_ ) do {\
        if( ( (_result_) != (_expected1_) &&                            \
              (_result_) != (_expected2_) &&                            \
              (_result_) != (_expected3_) &&                            \
              (_result_) != (_expected4_) &&                            \
              (_result_) != (_expected5_) ) ) {                         \
        tm_macro_error( (5)                                             \
                      , __FILE__                                        \
                      , __FUNCTION__                                    \
                      , __LINE__                                        \
                      , _result_                                        \
                      , _expected1_                                     \
                      , _expected2_                                     \
                      , _expected3_                                     \
                      , _expected4_                                     \
                      , _expected5_);                                   \
        }} while(0);
/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/** Retrieves the list of available test cases from within the test library.
 *  @param cfg_file [in] the name of the config file.
 *  @param cases [out] the list of test cases.
 *  @return error code, or 0 in case of success.
 */
int             tm_get_test_cases (const char *cfg_file, DLList ** cases);
/* ------------------------------------------------------------------------- */
/** Executes a test case.
 *  @param id [in] test case id number (>0).
 *  @param cfg_file [in] the name of the config file.
 *  @param result [out] test result.
 *  @return error code or 0 in case of success.
 */
int             tm_run_test_case (unsigned int id, const char *cfg_file,
                                  TestCaseResult * result);
/* ------------------------------------------------------------------------- */
/** Function called once when library (test module) is loaded by test module
 * controller. It is not mandatory to implement it.
 * @param none
 * @return none
 */
void            tm_initialize ();
/* ------------------------------------------------------------------------- */
/** Function called once when library (test module) is closed by test module
 * controller. It is not mandatory to implement it.
 * @param none
 * @return none
 */
void            tm_finalize ();
/* ------------------------------------------------------------------------- */
int             compare_id (const void *a, const void *b);
/* ------------------------------------------------------------------------- */
int             tm_printf (int priority, char *desc, char *format, ...);
/* ------------------------------------------------------------------------- */
int             tm_print_err (char *format, ...);
/* ------------------------------------------------------------------------- */
void            tm_macro_error (unsigned int macro_type, const char *file,
                                const char *function, unsigned int line,
                                int result, int expected1, int expected2,
                                int expected3, int expected4, int expected5);
/* ------------------------------------------------------------------------- */
char           *tm_get_caller_name (void);
/* ------------------------------------------------------------------------- */

#endif                          /* TEST_MODULE_API_H */
/* End of file */
