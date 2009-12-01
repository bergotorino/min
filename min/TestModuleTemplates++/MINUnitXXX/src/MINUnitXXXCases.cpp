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
 * GLOBAL VARIABLES SECTION
 */
#ifdef TEST_VAR_DECLARATIONS
        /**
	 * Example of variable common for some test cases
	 */
char           *XXX_var;

#endif                          /* TEST_VAR_DECLARATIONS */
/**
 * END OF GLOBAL VARIABLES SECTION
 */


/**
 * TEST CASES SECTION
 */
#ifdef TEST_CASES
/**
 * MIN_SETUP defines activities needed before every test case.
 */
MIN_SETUP {
        MIN_DEBUG ("UNIT XXX SETUP\n");
}

/**
 * MIN_TEARDOWN defines activities needed after every test case
 */
MIN_TEARDOWN {
        MIN_DEBUG ("UNIT XXX TEARDOWN\n");
}

/**
 * MIN_TESTDEFINE defines a test case
 *
 */
MIN_TESTDEFINE (XXX_1)
{

        MIN_ASSERT_NOT_EQUALS (0, 1);
}

/*
** This one fails
*/
MIN_TESTDEFINE_DESC (XXX_2, <description for case XXX_2>)
{
        MIN_ASSERT_EQUALS (0, 1);
}

#endif
/**
 * END OF TEST CASES SECTION
 */
