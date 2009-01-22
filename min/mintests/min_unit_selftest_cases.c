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
int             uno, one, two, dos;
void           *null_ptr, *ptr, *ptrsame, *ptrnotsame;
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
        uno = one = 1;
        dos = two = 2;
        null_ptr = NULL;
        ptr = &one;
        ptrsame = &one;
        ptrnotsame = &two;
}

/**
 * MIN_TEARDOWN defines activities needed after every test case
 */
MIN_TEARDOWN {
        ;
}

/**
 * MIN_TESTDEFINE defines a test case
 *
 */
MIN_TESTDEFINE (assertequals_fail)
{

        MIN_ASSERT_EQUALS (uno, dos);
}

MIN_TESTDEFINE (assertequals_ok)
{

        MIN_ASSERT_EQUALS (uno, one);
}

MIN_TESTDEFINE (assertnotequals_fail)
{

        MIN_ASSERT_NOT_EQUALS (two, dos);
}

MIN_TESTDEFINE (assertnotequals_ok)
{

        MIN_ASSERT_NOT_EQUALS (uno, two);
}

MIN_TESTDEFINE (assertnull_fails)
{

        MIN_ASSERT_NULL (ptr);
}

MIN_TESTDEFINE (assertnull_ok)
{

        MIN_ASSERT_NULL (null_ptr);
}

MIN_TESTDEFINE (assertnotnull_fails)
{

        MIN_ASSERT_NOT_NULL (null_ptr);
}

MIN_TESTDEFINE (assertnotnull_ok)
{

        MIN_ASSERT_NOT_NULL (ptr);
}

MIN_TESTDEFINE (assertsame_fails)
{

        MIN_ASSERT_SAME (ptr, ptrnotsame);
}

MIN_TESTDEFINE (assertsame_ok)
{

        MIN_ASSERT_SAME (ptr, ptrsame);
}

MIN_TESTDEFINE (assertnotsame_fails)
{

        MIN_ASSERT_NOT_SAME (ptr, ptrsame);
}

MIN_TESTDEFINE (assertnotsame_ok)
{

        MIN_ASSERT_NOT_SAME (ptr, ptrnotsame);
}

MIN_TESTDEFINE (asserttrue_fails)
{
        MIN_ASSERT_TRUE (one == 2);
}

MIN_TESTDEFINE (asserttrue_ok)
{
        MIN_ASSERT_TRUE (one == 1);
}

MIN_TESTDEFINE (assertfals_fails)
{
        MIN_ASSERT_FALSE (one == 1);
}

MIN_TESTDEFINE (assertfalse_ok)
{
        MIN_ASSERT_FALSE (one == 2);
}

#endif
/**
 * END OF TEST CASES SECTION
 */
