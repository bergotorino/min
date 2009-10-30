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
 *  @file       logger_tests_cases.c
 *  @version    0.1
 *  @brief      This file contains implementation of tests for Logger Mechanism
 */

/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <min_logger.h>
#include <min_logger_output.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* ============================== TESTS ==================================== */
/* ------------------------------------------------------------------------- */
/**
 * TEST CASES SECTION
 */
#ifdef TEST_CASES
MIN_TESTDEFINE(test_logger_open)
{
     int log_open_retval;

     log_open_retval = min_log_open ("min_logger_test", 1);
     MIN_ASSERT_EQUALS_DESC (log_open_retval, 0, "failed to open log");
     min_log_close();
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_logger_open_inv_dlevel)
{
     int log_open_retval;

     log_open_retval = min_log_open ("min_logger_test", 4);
     MIN_ASSERT_EQUALS_DESC (log_open_retval, 0, "should react to invalid debug level");
     min_log_close();
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_logger_close)
{
     int log_close_retval;

     log_close_retval = min_log_close ();
     MIN_ASSERT_EQUALS_DESC (log_close_retval, 0, "failed to close log");
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_logger_log)
{
	int log_retval;
	
	log_retval = min_info ("min test");
	MIN_ASSERT_EQUALS_DESC (log_retval, 0, "failed to log");
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_logger_log_null_arg)
{
	int log_retval;
	
	log_retval = min_err (NULL);
	MIN_ASSERT_EQUALS_DESC (log_retval, 0, "failed to log");
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_logger_warn)
{
	int log_retval;
	
	log_retval = min_warn ("TEST warning");
	MIN_ASSERT_EQUALS_DESC (log_retval, 0, "failed to log");
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_debug)
{
	int log_retval = 0;
	
	log_retval =  min_debug (__FUNCTION__, __LINE__, __FILE__,
				 "testing debug");
	
	MIN_ASSERT_EQUALS_DESC (log_retval, 0, "failed to log");
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_so_create)
{
        MinLoggerSyslogOutput* sxso = INITPTR;
	
        sxso = so_create( NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
        
        MIN_ASSERT_NOT_EQUALS_DESC( sxso, INITPTR
				    , "Syslog Output: sxso == INITPTR" );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_so_destroy)
{
        MinLoggerSyslogOutput* sxso = INITPTR;
	
	sxso = so_create( NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
	
	so_destroy((struct output_typeinfo_t**)&sxso);
	
        MIN_ASSERT_EQUALS_DESC( sxso, INITPTR,
				"SysLog Output: sxso != INITPTR after destruction" );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_so_destroy2)
{
        MinLoggerSyslogOutput* sxso = INITPTR;
	
        sxso = so_create( NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0 );       
        so_destroy((struct output_typeinfo_t**)&sxso);
	
        MIN_ASSERT_EQUALS_DESC( sxso, INITPTR
				, "SysLog Output: sxso != INITPTR after destruction" );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_so_write)
{
        MinLogger* mnl = INITPTR;
        TSChar data[] = "LOGGER UNIT TESTING: Syslog Output message";
	
        
	mnl = mnl_create ("/var/log",
			  "syslog",
			  ESTxt,
			  ESSyslog,
			  0,
			  1,
			  1,
			  1,
			  1,
			  0,
			  0, 
			  0);
	
        MIN_ASSERT_NOT_EQUALS_DESC( mnl, INITPTR
				    , "Syslog Output: mnl == INITPTR" );
	
        mnl_log( mnl, ESNoStyle, data );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_stdo_create)
{
        MinLoggerStdOutput* stdo = INITPTR;
	
        stdo = stdo_create( NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
        
        MIN_ASSERT_NOT_EQUALS_DESC( stdo, INITPTR
				    , "Std Output: stdo == INITPTR" );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_stdo_destroy)
{
        MinLoggerStdOutput* stdo = INITPTR;
        stdo_destroy((struct output_typeinfo_t**)&stdo);

        MIN_ASSERT_EQUALS_DESC( stdo, INITPTR
                   , "Std Output: stdo != INITPTR after destruction" );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_stdo_destroy2)
{
        MinLoggerStdOutput* stdo = INITPTR;

        stdo = stdo_create( NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0 );       
        stdo_destroy((struct output_typeinfo_t**)&stdo);
 
        MIN_ASSERT_EQUALS_DESC( stdo, INITPTR
                   , "Std Output: stdo != INITPTR after destruction" );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_stdo_write)
{
        MinLogger* mnl = INITPTR;
        TSChar data[] = " LOGGER UNIT TESTING: Std Output message";

	mnl = mnl_create( "/tmp/"
			  , "foo"
			  , ESTxt
			  , ESStdout
			  , ESTrue
			  , ESTrue
			  , ESTrue
			  , ESFalse
			  , ESFalse
			  , ESFalse
			  , 1000
			  , ESFalse );

	
        MIN_ASSERT_NOT_EQUALS_DESC( mnl, INITPTR
				    , "Syslog Output: mnl == INITPTR" );
	
        mnl_log( mnl, ESNoStyle, data );
}	 

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_stde_create)
{
        MinLoggerStdOutput* stde = INITPTR;
	
        stde = stde_create( NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
        
        MIN_ASSERT_NOT_EQUALS_DESC( stde, INITPTR
				    , "Std Output: stde == INITPTR" );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_stde_destroy)
{
        MinLoggerStdOutput* stde = INITPTR;
        stde_destroy((struct output_typeinfo_t**)&stde);

        MIN_ASSERT_EQUALS_DESC( stde, INITPTR
                   , "Std Output: stde != INITPTR after destruction" );
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_stde_write)
{
        MinLogger* mnl = INITPTR;
        TSChar data[] = " LOGGER UNIT TESTING: Stderr Output message";

	mnl = mnl_create ("/tmp/",
			  "minlog",
			  ESTxt,
			  ESStderr,
			  0,
			  1,
			  1,
			  1,
			  1,
			  0,
			  0, 
			  0);
			 

        MIN_ASSERT_NOT_EQUALS_DESC( mnl, INITPTR
				    , "stderr Output: mnl == INITPTR" );

        mnl_log( mnl, ESNoStyle, data );
}
#endif /* TEST_CASES */
/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* End of file */
