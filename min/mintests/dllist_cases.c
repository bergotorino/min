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
#include "equal.h"

/**
 * GLOBAL VARIABLES SECTION
 */
#ifdef TEST_VAR_DECLARATIONS
        /**
	 * Example of variable common for some test cases
	 */
int             a, b, i, k, new;
int             retval;
int             aaa[3];
int            *pom, *value;
DLListIterator  it;
DLList         *l;
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
        l = dl_list_create ();
        it = DLListNULLIterator;
        retval = -1;
        new = 55;
        k = 0;
        pom = INITPTR;
        aaa[0] = 5;
        aaa[1] = 7;
        aaa[2] = 98;

}

/**
 * MIN_TEARDOWN defines activities needed after every test case
 */
MIN_TEARDOWN {
        if (l != INITPTR)
                dl_list_free (&l);
}

/**
 * MIN_TESTDEFINE defines a test case
 *
 */

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE (test_dllist_create)
{
        MIN_ASSERT_NOT_EQUALS (l, INITPTR);
        MIN_ASSERT_EQUALS (dl_list_size (l), 0);
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE (test_dllist_free)
{
        dl_list_free (&l);
        MIN_ASSERT_EQUALS (l, INITPTR);
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE (test_dllist_add_item)
{
        MIN_ASSERT_NOT_EQUALS (l, INITPTR);
        a = 5;
        retval = dl_list_add (l, &a);
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        MIN_ASSERT_EQUALS (dl_list_size (l), 1);
        dl_list_free (&l);
        retval = dl_list_add (INITPTR, &a);
        MIN_ASSERT_EQUALS (retval, -1);
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE (test_dllist_add_item_at_1)
{
        a = 1;
        dl_list_free (&l);
        retval = dl_list_add_at (INITPTR, &a, 0);
        MIN_ASSERT_EQUALS (retval, -1);
        retval = dl_list_add_at (INITPTR, &a, 99);
        MIN_ASSERT_EQUALS (retval, -1);
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE (test_dllist_add_item_at_2)
{
        MIN_ASSERT_NOT_EQUALS (l, INITPTR);

        retval = dl_list_add_at (l, &aaa[0], 0);
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        MIN_ASSERT_EQUALS (dl_list_size (l), 1);

        retval = dl_list_add_at (l, &aaa[1], 1);
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        MIN_ASSERT_EQUALS (dl_list_size (l), 2);

        retval = dl_list_add_at (l, &aaa[2], 2);
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        MIN_ASSERT_EQUALS (dl_list_size (l), 3);

        for (k = 0; k < dl_list_size (l); k++) {
                pom = (int *)dl_list_data (dl_list_at (l, k));
                MIN_ASSERT_NOT_EQUALS (pom, INITPTR);
                MIN_ASSERT_EQUALS (*pom, aaa[k]);
        }
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE (test_dllist_add_item_at_3)
{
        MIN_ASSERT_NOT_EQUALS (l, INITPTR);

        retval = dl_list_add_at (l, &aaa[0], 0);
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        MIN_ASSERT_EQUALS (dl_list_size (l), 1);

        retval = dl_list_add_at (l, &aaa[1], 0);
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        MIN_ASSERT_EQUALS (dl_list_size (l), 2);

        retval = dl_list_add_at (l, &aaa[2], 0);
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        MIN_ASSERT_EQUALS (dl_list_size (l), 3);

        for (k = 0; k < dl_list_size (l); k++) {
                pom = (int *)dl_list_data (dl_list_at (l, k));
                MIN_ASSERT_NOT_EQUALS (pom, INITPTR);
                MIN_ASSERT_EQUALS (*pom, aaa[2 - k]);
        }
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE (test_dllist_add_item_at_4)
{
        MIN_ASSERT_NOT_EQUALS (l, INITPTR);

        retval = dl_list_add_at (l, &aaa[0], 1);
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        MIN_ASSERT_EQUALS (dl_list_size (l), 1);

        retval = dl_list_add_at (l, &aaa[1], 1);
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        MIN_ASSERT_EQUALS (dl_list_size (l), 2);

        retval = dl_list_add_at (l, &aaa[2], 1);
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        MIN_ASSERT_EQUALS (dl_list_size (l), 3);

        pom = (int *)dl_list_data (dl_list_at (l, 0));
        MIN_ASSERT_NOT_EQUALS (pom, INITPTR);
        MIN_ASSERT_EQUALS (*pom, aaa[0]);

        pom = (int *)dl_list_data (dl_list_at (l, 1));
        MIN_ASSERT_NOT_EQUALS (pom, INITPTR);
        MIN_ASSERT_EQUALS (*pom, aaa[2]);

        pom = (int *)dl_list_data (dl_list_at (l, 2));
        MIN_ASSERT_NOT_EQUALS (pom, INITPTR);
        MIN_ASSERT_EQUALS (*pom, aaa[1]);

}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE (test_dllist_remove_item)
{
        a = 8;
        b = 9;
        retval = -1;
        MIN_ASSERT_NOT_EQUALS (l, INITPTR);
        dl_list_add (l, &a);
        dl_list_add (l, &b);
        retval = dl_list_remove_it (dl_list_find (dl_list_head (l)
                                                  , dl_list_tail (l)
                                                  , equal, &a));
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        MIN_ASSERT_EQUALS (dl_list_size (l), 1);
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE (test_dllist_add_multiple_item)
{
        MIN_ASSERT_NOT_EQUALS (l, INITPTR);
        for (i = 0; i < 3; i++) {
                retval = dl_list_add (l, &aaa[i]);
                MIN_ASSERT_NOT_EQUALS (retval, -1);
        }
        for (k = 0; k < dl_list_size (l); k++) {
                pom = (int *)dl_list_data (dl_list_at (l, k));
                MIN_ASSERT_NOT_EQUALS (pom, INITPTR);
                MIN_ASSERT_EQUALS (*pom, aaa[k]);
        }
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE (test_dllist_two_lists)
{
        DLList         *ll = dl_list_create ();
        MIN_ASSERT_NOT_EQUALS (l, INITPTR);
        MIN_ASSERT_NOT_EQUALS (ll, INITPTR);
        retval = dl_list_add (l, &a);
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        MIN_ASSERT_NOT_EQUALS (dl_list_size (l), dl_list_size (ll));
        dl_list_free (&ll);
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE (test_dllist_alter_item_by_iterator)
{
        MIN_ASSERT_NOT_EQUALS (l, INITPTR);
        for (i = 0; i < 3; i++) {
                retval = dl_list_add (l, &aaa[i]);
                MIN_ASSERT_NOT_EQUALS_DESC (retval, -1, "Adding failed.");
        }
        retval = dl_list_alter_it (dl_list_head (l), &aaa[2]);
        MIN_ASSERT_NOT_EQUALS_DESC (retval, -1, "dl_list_alter_it failed");
        value = (int *)dl_list_data (dl_list_head (l));
        MIN_ASSERT_EQUALS_DESC (*value, aaa[2], "Alter error");
        retval = dl_list_alter_it (dl_list_head (l), INITPTR);
        MIN_ASSERT_NOT_EQUALS_DESC (retval, -1, "dl_list_alter_it failed when value NULL.");
        dl_list_free (&l);
        retval = dl_list_alter_it (INITPTR, &aaa[2]);
        MIN_ASSERT_EQUALS_DESC (retval, -1, "dl_list_alter_it executed when list is NULL.");
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE (test_dllist_alter_item_by_data)
{
        MIN_ASSERT_NOT_EQUALS (l, INITPTR);
        for (i = 0; i < 3; i++) {
                retval = dl_list_add (l, &aaa[i]);
                MIN_ASSERT_NOT_EQUALS (retval, -1);
        }
        pom = dl_list_data (dl_list_head (l));
        *(int *)pom = aaa[2];
        value = (int *)dl_list_data (dl_list_head (l));
        MIN_ASSERT_EQUALS (*value, aaa[2]);
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE (test_dllist_next_functionality)
{
        it = DLListNULLIterator;
        MIN_ASSERT_NOT_EQUALS (l, INITPTR);
        for (i = 0; i < 3; i++) {
                retval = dl_list_add (l, &aaa[i]);
                MIN_ASSERT_NOT_EQUALS (retval, -1);
        }
        it = dl_list_next (dl_list_head (l));
        value = (int *)dl_list_data (it);
        MIN_ASSERT_EQUALS (*value, aaa[1]);
        dl_list_free (&l);
        it = dl_list_next (INITPTR);
        MIN_ASSERT_EQUALS (it, DLListNULLIterator);
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE (test_dllist_prev_functionality)
{
        MIN_ASSERT_NOT_EQUALS (l, INITPTR);
        for (i = 0; i < 3; i++) {
                retval = dl_list_add (l, &aaa[i]);
                MIN_ASSERT_NOT_EQUALS (retval, -1);
        }
        it = dl_list_prev (dl_list_tail (l));
        value = (int *)dl_list_data (it);
        MIN_ASSERT_EQUALS (*value, aaa[1]);
        dl_list_free (&l);
        it = dl_list_prev (INITPTR);
        MIN_ASSERT_EQUALS (it, DLListNULLIterator);
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE (test_dllist_head)
{
        MIN_ASSERT_NOT_EQUALS (l, INITPTR);
        for (i = 0; i < 3; i++) {
                retval = dl_list_add (l, &aaa[i]);
                MIN_ASSERT_NOT_EQUALS (retval, -1);
        }
        it = dl_list_head (l);
        MIN_ASSERT_NOT_EQUALS (it, DLListNULLIterator);
        value = (int *)dl_list_data (it);
        MIN_ASSERT_EQUALS (*value, aaa[0]);
        dl_list_free (&l);
        it = dl_list_head (INITPTR);
        MIN_ASSERT_EQUALS (it, DLListNULLIterator);
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE (test_dllist_tail)
{
        MIN_ASSERT_NOT_EQUALS (l, INITPTR);
        for (i = 0; i < 3; i++) {
                retval = dl_list_add (l, &aaa[i]);
                MIN_ASSERT_NOT_EQUALS (retval, -1);
        }
        it = dl_list_tail (l);
        MIN_ASSERT_NOT_EQUALS (it, DLListNULLIterator);
        value = (int *)dl_list_data (it);
        MIN_ASSERT_EQUALS (*value, aaa[2]);
        dl_list_free (&l);
        it = dl_list_tail (INITPTR);
        MIN_ASSERT_EQUALS (it, DLListNULLIterator);
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE (test_dllist_size)
{
        MIN_ASSERT_NOT_EQUALS (l, INITPTR);
        MIN_ASSERT_EQUALS (dl_list_size (l), 0);
        retval = dl_list_add (l, &aaa[0]);
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        MIN_ASSERT_EQUALS (dl_list_size (l), 1);
        retval = dl_list_add (l, &aaa[1]);
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        MIN_ASSERT_EQUALS (dl_list_size (l), 2);
        retval = dl_list_add (l, &aaa[2]);
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        MIN_ASSERT_EQUALS (dl_list_size (l), 3);
        it = dl_list_tail (l);
        retval = dl_list_remove_it (it);
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        MIN_ASSERT_EQUALS (dl_list_size (l), 2);
        dl_list_free (&l);
        retval = dl_list_size (INITPTR);
        MIN_ASSERT_EQUALS (retval, -1);
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE (test_dllist_remove)
{
        MIN_ASSERT_NOT_EQUALS (l, INITPTR);
        MIN_ASSERT_EQUALS (dl_list_size (l), 0);
        for (i = 0; i < 3; i++) {
                retval = dl_list_add (l, &aaa[i]);
                MIN_ASSERT_NOT_EQUALS (retval, -1);
        }
        it = dl_list_tail (l);
        MIN_ASSERT_NOT_EQUALS (it, DLListNULLIterator);
        retval = dl_list_remove_it (it);
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        it = dl_list_head (l);
        MIN_ASSERT_NOT_EQUALS (it, DLListNULLIterator);
        retval = dl_list_remove_it (it);
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        retval = dl_list_add (l, &aaa[0]);
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        it = dl_list_head (l);
        MIN_ASSERT_NOT_EQUALS (it, DLListNULLIterator);
        retval = dl_list_remove_it (it);
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        it = dl_list_head (l);
        MIN_ASSERT_NOT_EQUALS (it, DLListNULLIterator);
        retval = dl_list_remove_it (it);
        MIN_ASSERT_NOT_EQUALS (retval, -1);
        it = dl_list_head (l);
        MIN_ASSERT_EQUALS (it, DLListNULLIterator);
        retval = dl_list_remove_it (it);
        MIN_ASSERT_EQUALS (retval, -1);
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE (test_dllist_data_at)
{
        MIN_ASSERT_NOT_EQUALS (l, INITPTR);
        MIN_ASSERT_EQUALS (dl_list_size (l), 0);
        for (i = 0; i < 3; i++) {
                retval = dl_list_add (l, &aaa[i]);
                MIN_ASSERT_NOT_EQUALS (retval, -1);
        }
        it = dl_list_at (l, 0);
        MIN_ASSERT_NOT_EQUALS (it, DLListNULLIterator);
        value = (int *)dl_list_data (it);
        MIN_ASSERT_EQUALS (*value, aaa[0]);
        it = dl_list_at (l, 1);
        MIN_ASSERT_NOT_EQUALS (it, DLListNULLIterator);
        value = (int *)dl_list_data (it);
        MIN_ASSERT_EQUALS (*value, aaa[1]);
        it = dl_list_at (l, 2);
        MIN_ASSERT_NOT_EQUALS (it, DLListNULLIterator);
        value = (int *)dl_list_data (it);
        MIN_ASSERT_EQUALS (*value, aaa[2]);
        it = dl_list_at (l, 3);
        MIN_ASSERT_EQUALS (it, DLListNULLIterator);
        dl_list_free (&l);
        value = (int *)dl_list_data (INITPTR);
        MIN_ASSERT_EQUALS (value, DLListNULLIterator);
}

/* ------------------------------------------------------------------------- */
MIN_TESTDEFINE (test_dllist_find)
{
        MIN_ASSERT_NOT_EQUALS (l, INITPTR);
        for (i = 0; i < 3; i++) {
                retval = dl_list_add (l, &aaa[i]);
                MIN_ASSERT_NOT_EQUALS (retval, -1);
        }
        it = dl_list_find (dl_list_head (l)
                           , dl_list_tail (l)
                           , equal, &aaa[2]);
        MIN_ASSERT_NOT_EQUALS (it, DLListNULLIterator);
        value = (int *)dl_list_data (it);
        MIN_ASSERT_EQUALS (*value, aaa[2]);

        it = dl_list_find (dl_list_head (l)
                           , dl_list_tail (l)
                           , equal, &aaa[1]);
        MIN_ASSERT_NOT_EQUALS (it, DLListNULLIterator);
        value = (int *)dl_list_data (it);
        MIN_ASSERT_EQUALS (*value, aaa[1]);

        it = dl_list_find (dl_list_head (l)
                           , dl_list_tail (l)
                           , equal, &aaa[0]);
        MIN_ASSERT_NOT_EQUALS (it, DLListNULLIterator);
        value = (int *)dl_list_data (it);
        MIN_ASSERT_EQUALS (*value, aaa[0]);

        i = 17;
        it = dl_list_find (dl_list_head (l)
                           , dl_list_tail (l)
                           , equal, &i);
        MIN_ASSERT_EQUALS (it, DLListNULLIterator);

        it = dl_list_find (DLListNULLIterator, dl_list_tail (l)
                           , equal, &aaa[2]);
        MIN_ASSERT_EQUALS (it, DLListNULLIterator);

        it = dl_list_find (dl_list_head (l)
                           , DLListNULLIterator, equal, &aaa[2]);
        MIN_ASSERT_EQUALS (it, DLListNULLIterator);

        it = dl_list_find (DLListNULLIterator, DLListNULLIterator, equal,
                           &aaa[2]);
        MIN_ASSERT_EQUALS (it, DLListNULLIterator);

        it = dl_list_find (dl_list_head (l)
                           , dl_list_tail (l)
                           , (ptr2compare) INITPTR, &aaa[2]);
        MIN_ASSERT_EQUALS (it, DLListNULLIterator);

        it = dl_list_find (dl_list_tail (l)
                           , dl_list_head (l)
                           , (ptr2compare) INITPTR, &aaa[2]);
        MIN_ASSERT_EQUALS (it, DLListNULLIterator);

        it = dl_list_find (dl_list_head (l)
                           , dl_list_head (l)
                           , (ptr2compare) INITPTR, &aaa[2]);
        MIN_ASSERT_EQUALS (it, DLListNULLIterator);

        it = dl_list_find (dl_list_tail (l)
                           , dl_list_tail (l)
                           , (ptr2compare) INITPTR, &aaa[2]);
        MIN_ASSERT_EQUALS (it, DLListNULLIterator);
}
/* ------------------------------------------------------------------------- */

#endif
/**
 * END OF TEST CASES SECTION
 */
