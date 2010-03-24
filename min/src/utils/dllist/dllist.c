/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Konrad Marek Zapalowicz
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
 *  @file       dllist.c
 *  @version    0.1
 *  @brief      This file contains implementation of Doubly Linked List
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <dllist.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */

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

/* ==================== LOCAL FUNCTIONS ==================================== */
/* None */

/* ======================== FUNCTIONS ====================================== */

/* ------------------------------------------------------------------------- */
/** Creates a new doubly linked list.
 *  @return pointer to created list, or NULL along with setting the global
 *          errno variable.
 *
 *  Possible errors:
 *  - ENOMEM not enough memory to create list.
 */
DLList         *dl_list_create ()
{
        DLList         *new_list = (DLList *) malloc (sizeof (DLList));
        if (new_list != NULL) {
                new_list->head_ = INITPTR;
                new_list->tail_ = new_list->head_;
        } else {
                errno = ENOMEM;
                new_list = INITPTR;
        }
        return new_list;
}

/* ------------------------------------------------------------------------- */
/** Frees memory allocated for doubly linked list.
 *  NOTE: Does not free memory allocated for data stored on the list!
 *  @param list_handle adress of the pointer to the doubly linked list.
 */
void dl_list_free (DLList ** list_handle)
{
        DLListItem     *current = INITPTR;
        DLListItem     *next = INITPTR;

        if (*list_handle != INITPTR) {
                current = (*list_handle)->head_;
                next = INITPTR;
                while (current != INITPTR) {
                        next = current->next_;
                        free (current);
                        current = next;
                }
                free (*list_handle);
                *list_handle = INITPTR;
        }
}

/* ------------------------------------------------------------------------- */
/** Frees the data from each item in the list
 *  @param list_handle adress of the pointer to the doubly linked list.
 */
void dl_list_free_data (DLList ** list_handle)
{
        DLListItem     *current = INITPTR;
        DLListItem     *next = INITPTR;

        if (*list_handle != INITPTR) {
                current = (*list_handle)->head_;
                next = INITPTR;
                while (current != INITPTR) {
                        next = current->next_;
                        free (current->data_);
                        current = next;
                }
        }
}

/* ------------------------------------------------------------------------- */
/** Adds a new element to the end of the list.
 *  @param list_handle link to list to which a new element is added.
 *  @param data pointer to data that is stored on the list.
 *  @return 0 in case of success, returns -1 and sets errno in case of failure.
 *
 *  Possible errors:
 *  - EINVAL list handler is invalid
 *  - ENOMEM not enough memory to allocate new element of the list.
 */
int dl_list_add (DLList * list_handle, void *data)
{
        DLListItem     *new_element = INITPTR;
        int             retval = 0;
        if (list_handle != INITPTR) {
                new_element = (DLListItem *) malloc (sizeof (DLListItem));
                if (new_element != NULL) {
                        new_element->data_ = data;
                        new_element->next_ = INITPTR;
                        new_element->prev_ = INITPTR;
                        new_element->list_ = list_handle;

                        if (list_handle->tail_ != INITPTR) {
                                new_element->prev_ = list_handle->tail_;
                                list_handle->tail_->next_ = new_element;
                                list_handle->tail_ = new_element;
                        } else {
                                list_handle->head_ = new_element;
                                list_handle->tail_ = new_element;
                        }
                } else {
                        errno = ENOMEM;
                        retval = -1;
                        new_element = INITPTR;
                }
        } else {
                errno = EINVAL;
                retval = -1;
        }
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Adds a new element, at the given position, to the list.
 *  @param list_handle link to list to which a new element is added.
 *  @param data pointer to data that is stored on the list.
 *  @param pos position on which the element will be added. [0;N-1]
 *  @return 0 in case of success, returns -1 and sets errno in case of failure.
 *
 *  If pos is greater then actual size of the list then the element is
 *  added at the end of the list.
 *
 *  Possible errors:
 *  - EINVAL list handler is invalid
 *  - ENOMEM not enough memory to allocate new element of the list.
 */
int dl_list_add_at (DLList * list_handle, void *data, unsigned int pos)
{
        DLListItem     *new_element = INITPTR;
        DLListItem     *old = INITPTR;
        DLListItem     *prev = INITPTR;
        int             retval = 0;
        int             i = 0;

        if (list_handle != INITPTR) {
                new_element = (DLListItem *) malloc (sizeof (DLListItem));
                if (new_element != NULL) {
                        new_element->data_ = data;
                        new_element->next_ = INITPTR;
                        new_element->prev_ = INITPTR;
                        new_element->list_ = list_handle;

                        /* find element at specified position */
                        old = list_handle->head_;
                        for (i = 0; i < pos; i++) {
                                if (old == INITPTR)
                                        break;
                                if (old->next_ == INITPTR)
                                        break;
                                old = old->next_;
                        }

                        if (i == pos) { /* inside of a list */

                                if (old == INITPTR) {
                                        DELETE (new_element);
                                        dl_list_add (list_handle, data);
                                } else {
                                        prev = old->prev_;
                                        old->prev_ = new_element;
                                        new_element->next_ = old;

                                        if (prev != INITPTR) {
                                                new_element->prev_ = prev;
                                                prev->next_ = new_element;
                                        } else {
                                                list_handle->head_ =
                                                    new_element;
                                        }
                                }
                        } else {        /* outside of a list */
                                DELETE (new_element);
                                dl_list_add (list_handle, data);
                        }

                } else {
                        errno = ENOMEM;
                        retval = -1;
                        new_element = INITPTR;
                }
        } else {
                errno = EINVAL;
                retval = -1;
        }
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Removes element from the list which is specified by iterator pointer.
 *  NOTE: after removal allocated element is freed, 
 *  and input iterator is set to NULL iterator;
 *  @param it iterator pointing at the element which is going to be removed
 *  @return 0 in case of success, returns -1 and sets errno in case of failure.
 *
 *  Possible errors:
 *  - EINVAL input iterator is invalid
 */
int dl_list_remove_it (DLListIterator it)
{
        int             retval = 0;
        DLListIterator  prev = DLListNULLIterator;
        DLListIterator  next = DLListNULLIterator;
        if (it != DLListNULLIterator) {
                prev = it->prev_;
                next = it->next_;

                if (prev != DLListNULLIterator)
                        prev->next_ = next;
                else
                        it->list_->head_ = next;

                if (next != DLListNULLIterator)
                        next->prev_ = prev;
                else
                        it->list_->tail_ = prev;

                free (it);
                it = DLListNULLIterator;
        } else {
                retval = -1;
                errno = EINVAL;
        }
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Finds element on the list using a supplied function as a method of 
 *  comparison.
 *  Given function is called for each element in the list, in the given range.
 *  Function should return 0 when the element is found.
 *  @param begin left border of the search area
 *  @param end right border of the search area
 *  @param unary the function to call for each element.
 *  @param data pointer to user data passed to the function.
 *  @return iterator pointing to found element, or NULL iterator when nothing
 *          is found. In case of failure NULL iterator is returned.
 *
 *  Possible errors:
 *  - EINVAL when one or more parameters are invalid.
 */
DLListIterator dl_list_find (DLListConstIterator begin,
                             DLListConstIterator end, ptr2compare unary,
                             const void *data)
{
        DLListIterator  retval = DLListNULLIterator;
        DLListConstIterator current = DLListNULLIterator;
        if (begin == DLListNULLIterator)
                errno = EINVAL;
        else if (end == DLListNULLIterator)
                errno = EINVAL;
        else if (unary == (ptr2compare) 0xDEADBEEF)
                errno = EINVAL;
        else if (data == INITPTR)
                errno = EINVAL;
        else {
                current = begin;

                while (current != end && current != DLListNULLIterator) {
                        if (unary (current->data_, data) == 0) {
                                retval = (DLListIterator) current;
                                break;
                        }
                        current = current->next_;
                }

                if (retval == DLListNULLIterator) {
                        if (unary (end->data_, data) == 0) {
                                retval = (DLListIterator) end;
                        }
                }
        }
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Counts elements on the list which are find by specified method that is
 *  used to compare.
 *  @param begin left border of the search area
 *  @param end right border of the search area
 *  @param unary the function to call for each element.
 *  @param data pointer to user data passed to the function.
 *  @return number of found items.
 *
 *  Possible errors:
 *  - EINVAL when one or more parameters are invalid.
 */
unsigned dl_list_count (DLListConstIterator begin,
                        DLListConstIterator end, ptr2compare unary,
                        const void *data)
{
        unsigned retval = 0;
        DLListConstIterator current = DLListNULLIterator;
        
        if (begin == DLListNULLIterator)
                errno = EINVAL;
        else if (end == DLListNULLIterator)
                errno = EINVAL;
        else if (unary == (ptr2compare) 0xDEADBEEF)
                errno = EINVAL;
        else if (data == INITPTR)
                errno = EINVAL;
        else {
                current = begin;

                while (current != end && current != DLListNULLIterator) {
                        if (unary (current->data_, data) == 0) {
                                retval++;
                        }
                        current = current->next_;
                }

                if (current == end) {
                        if (unary (end->data_, data) == 0) {
                                retval++;
                        }
                }
        }
        return retval;
}
/* ------------------------------------------------------------------------- */
/** Tells how many elements are appended to the list.
 *  @param list_handle link to list which size is returned.
 *  @return number of elements appended to the list, returns -1 and sets errno
 *          in case of failure.
 *
 *  Possible errors:
 *  - EINVAL list handler is invalid
 */
int dl_list_size (const DLList * list_handle)
{
        int             retval = 0;
        DLListIterator  current = DLListNULLIterator;
        if (list_handle != INITPTR) {
                current = list_handle->head_;
                while (current != DLListNULLIterator) {
                        retval++;
                        current = current->next_;
                }
        } else {
                retval = -1;
                errno = EINVAL;
        }
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Gives pointer to the first element of the list.
 *  NOTE: When handler to list is invalid errno is set to EINVAL
 *  @param list_handle link to list from which element is returned.
 *  @return iterator pointing at the first element on the list. If list is 
 *          empty NULL iterator is returned.
 *
 *  Possible errors:
 *  - EINVAL list handler is invalid
 */
DLListIterator dl_list_head (const DLList * list_handle)
{
        DLListItem     *retval = INITPTR;
        if (list_handle != INITPTR)
                retval = list_handle->head_;
        else
                errno = EINVAL;
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Gives pointer to the last element of the list.
 *  NOTE: When handler to list is invalid errno is set to EINVAL
 *  @param list_handle link to list from which element is returned.
 *  @return iterator pointing at the last element on the list. If list is empty
 *          NULL iterator is returned.
 *
 *  Possible errors:
 *  - EINVAL list handler is invalid
 */
DLListIterator dl_list_tail (const DLList * list_handle)
{
        DLListItem     *retval = INITPTR;
        if (list_handle != INITPTR)
                retval = list_handle->tail_;
        else
                errno = EINVAL;
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Gets user data from iterator.
 *  @param it iterator pointing to one of the elements of list.
 *  @return pointer to the user-data, in case of failure NULL is returned.
 *
 *  Possible errors:
 *  - EINVAL input iterator is invalid
 */
void           *dl_list_data (DLListConstIterator it)
{
        void           *retval = INITPTR;
        if (it != DLListNULLIterator)
                retval = it->data_;
        else
                errno = EINVAL;
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Gets iterator pointing to the element at given position.
 *  @param list_handle link to list.
 *  @param i position of element in the list ( where i = [0,N] )
 *  @return iterator pointing to the element at i-th position. 
 *          In case of failure NULL iterator is returned.
 *
 *  Possible errors:
 *  - EINVAL list handler is invalid
 *  - EFAULT i param is out of its range
 */
DLListIterator dl_list_at (const DLList * list_handle, unsigned int i)
{
        DLListIterator  retval = DLListNULLIterator;
        DLListItem     *current = INITPTR;
        int             k = 0;
        if (list_handle != INITPTR) {
                if (i >= dl_list_size (list_handle))
                        errno = EFAULT;
                else {
                        k = 0;
                        current = list_handle->head_;
                        for (k = 0; k < i; ++k)
                                current = current->next_;
                        retval = current;
                }
        } else
                errno = EINVAL;
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Moves iterator in forward direction.
 *  @param it iterator of some list
 *  @return iterator to the next element on the list. In case of failure,
 *          or when the end of list is reached NULL is returned.
 *
 *  Possible errors:
 *  - EINVAL input iterator is invalid
 */
DLListIterator dl_list_next (DLListConstIterator it)
{
        DLListIterator  retval = DLListNULLIterator;
        if (it != DLListNULLIterator)
                retval = it->next_;
        else
                errno = EINVAL;
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Moves iterator in backward direction.
 *  @param it iterator of some list
 *  @return iterator to the previous element on the list. In case of failure,
 *          or when the begining of list is reached NULL is returned.
 *
 *  Possible errors:
 *  - EINVAL input iterator is invalid
 */
DLListIterator dl_list_prev (DLListConstIterator it)
{
        DLListIterator  retval = DLListNULLIterator;
        if (it != DLListNULLIterator)
                retval = it->prev_;
        else
                errno = EINVAL;
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Alters one element on the list.
 *  @param it iterator which points to the element to be altered.
 *  @param data new data which is going to be passed on the list.
 *  @return 0 in case of success, -1 in case of failure.
 *
 *  Possible errors:
 *  - EINVAL input iterator
 */
int dl_list_alter_it (DLListIterator it, const void *data)
{
        int             retval = 0;
        if (it != DLListNULLIterator)
                it->data_ = (void *)data;
        else {
                retval = -1;
                errno = EINVAL;
        }
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Executes unary function for each data stored on the list.
 *  @param begin [in] left delimiter.
 *  @param end [in] right delimiter.
 *  @param unary [in] adress of the unary function.
 *
 *  Possible errors:
 *  - EINVAL when one or more parameters are invalid.
 */
int dl_list_foreach (DLListConstIterator begin, DLListConstIterator end,
                     ptr2unary unary)
{
        int             retval = -1;
        DLListConstIterator current = DLListNULLIterator;
        DLListConstIterator current_next;

        if (begin == DLListNULLIterator)
                errno = EINVAL;
        else if (end == DLListNULLIterator)
                errno = EINVAL;
        else if (unary == (ptr2unary) 0xDEADBEEF)
                errno = EINVAL;
        else {
                current = begin;

                while (current != end->next_ && current != DLListNULLIterator) {
                        current_next = current->next_;
                        unary (current->data_);
                        current = current_next;
                }

                retval = 0;
        }
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Executes unary function for each iterator on the list.
 *  @param begin [in] left delimiter.
 *  @param end [in] right delimiter.
 *  @param unary [in] adress of the unary function.
 *
 *  Possible errors:
 *  - EINVAL when one or more parameters are invalid.
 */
int dl_list_foreach_it (DLListConstIterator begin, DLListConstIterator end,
                        ptr2itunary unary)
{
        int             retval = -1;
        DLListConstIterator current = DLListNULLIterator;
        DLListConstIterator currentnext = DLListNULLIterator;
        if (begin == DLListNULLIterator)
                errno = EINVAL;
        else if (end == DLListNULLIterator)
                errno = EINVAL;
        else if (unary == (ptr2itunary) 0xDEADBEEF)
                errno = EINVAL;
        else {
                current = begin;

                while (current != end->next_ && current != DLListNULLIterator) {
                        currentnext = current->next_;
                        unary ((DLListIterator) current);
                        current = currentnext;
                }

                retval = 0;
        }
        return retval;
}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "dllist.tests"
#endif                          /* MIN_UNIT_TEST */
/* ------------------------------------------------------------------------- */
/* End of file */
