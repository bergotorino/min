/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Sampo Saaristo
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
 *  @file       settins_tests_cases.c
 *  @version    0.1
 *  @brief      This file contains tests for MIN Settings Utility
 */

/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include <min_settings.h>
#include "settings_tests_utils.h"
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
#ifdef TEST_VAR_DECLARATIONS
SettingsSection *ss = INITPTR;
#endif /* TEST_VAR_DECLARATIONS */

#ifdef TEST_CASES
/* ------------------------------------------------------------------------- */
MIN_SETUP {
	settings_init ();

	if (new_section ("Test", 
			 INITPTR,
			 test_settings_parse, 
			 test_settings_init,
			 test_settings_clean,
			 sizeof (test_settings)) == 0)
		ss = settings_get_section ("Test");
}
/* ------------------------------------------------------------------------- */
MIN_TEARDOWN {
	settings_destroy_section ("Test");
}
/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_new_section_inv_param)
{
	
	
	MIN_ASSERT_NOT_EQUALS (0, new_section (NULL, INITPTR, 
					       NULL,NULL, NULL, 0));
}
/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_new_section)
{
	MIN_ASSERT_NOT_EQUALS (ss, INITPTR);
}
/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_section_read_inv_param)
{
	int ret;

	MIN_ASSERT_NOT_EQUALS (ss, INITPTR);
	
	ret = settings_read (ss, NULL);
	MIN_ASSERT_NOT_EQUALS (ret, 0);

	ret = settings_read (ss, INITPTR);
	MIN_ASSERT_NOT_EQUALS (ret, 0);
}
/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_section_read)
{
	int ret;
	MinParser *mp = INITPTR;
	
	MIN_ASSERT_NOT_EQUALS (ss, INITPTR);
        
	
        mp = mp_create ("/usr/share/min-ci-tests",
			"test.cfg",
			ENoComments );

	MIN_ASSERT_NOT_EQUALS (mp, INITPTR);

	ret = settings_read (ss, mp);

	MIN_ASSERT_EQUALS (ret, 0);
}
/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_section_get)
{
	int  ret;
	MinParser *msp = INITPTR;
	char path[4096];
	test_settings *ts;
	
	MIN_ASSERT_NOT_EQUALS (ss, INITPTR);
        
	sprintf(path, "%s", getenv("PWD"));
	
        msp = mp_create( "/usr/share/min-ci-tests"
			 , "test.cfg"
			 , ENoComments );
	
	MIN_ASSERT_NOT_EQUALS (msp, INITPTR);
	
	ret = settings_read (ss, msp);
	MIN_ASSERT_EQUALS (ret, 0);
	
	ts = (test_settings *)settings_get ("Test");
	MIN_ASSERT_NOT_EQUALS (ts, INITPTR);
	
	MIN_ASSERT_EQUALS (ts->foo, 1);
	MIN_ASSERT_EQUALS (dl_list_size (ts->bar_list), 1);
}
/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE(test_section_get_twice)
{
	int  ret;
	MinParser *msp = INITPTR;
	char path[4096];
	test_settings *ts;

	MIN_ASSERT_NOT_EQUALS (ss, INITPTR);
        
	sprintf(path, "%s", getenv("PWD"));
	
        msp = mp_create( "/usr/share/min-ci-tests"
			 , "test.cfg"
			 , ENoComments );
	
	MIN_ASSERT_NOT_EQUALS (msp, INITPTR);
	
	ret = settings_read (ss, msp);
	MIN_ASSERT_EQUALS (ret, 0);

	ts = (test_settings *)settings_get ("Test");
	MIN_ASSERT_NOT_EQUALS (ts, INITPTR);
	
	MIN_ASSERT_EQUALS (ts->foo, 1);
	MIN_ASSERT_EQUALS (dl_list_size (ts->bar_list), 1);

	ret = settings_read (ss, msp);
	MIN_ASSERT_EQUALS (ret, 0);

	ts = (test_settings *)settings_get ("Test");
	MIN_ASSERT_NOT_EQUALS (ts, INITPTR);
	
	MIN_ASSERT_EQUALS (ts->foo, 0);
	MIN_ASSERT_EQUALS (dl_list_size (ts->bar_list), 2);
}

/* ------------------------------------------------------------------------- */

#endif /* TEST_CASES */
/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* End of file */
