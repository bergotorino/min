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
 *  @file       dllist.h
 *  @version    0.1
 *  @brief      This file contains header file of the Doubly Linked List
 */

#ifndef DLLIST_H
#define DLLIST_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <stdlib.h>
#include <errno.h>
#include <min_common.h>

/* ------------------------------------------------------------------------- */
/* CONSTANTS */

/* ------------------------------------------------------------------------- */
/* MACROS */
/** Initial value for iterator. */
#define DLListNULLIterator      INITPTR
/* ------------------------------------------------------------------------- */
/* DATA TYPES */
typedef struct _DLListItem DLListItem;
typedef DLListItem *DLListIterator;
typedef const DLListItem *DLListConstIterator;
typedef struct _DLList DLList;

/** @brief Pointer to functon which is used as a comparision method in find
 *         function.
 *
 *  @param first is data from the list
 *  @param second value provided by the user.
 *  @return 0 when elements are equal, 1 when first is greater than second,
 *          -1 otherwise.
 */
typedef int     (*ptr2compare) (const void *, const void *);

/** @brief Pointer to the unary function used in foreach list functionality.
 *  @param first is the data from the list passed automatically during
 *         execution.
 */
typedef void    (*ptr2unary) (void *);

/** @brief Pointer to the unary function used in foreach list functionality.
 *  @param first is theiterator pointing to the element of the list
 */
typedef void    (*ptr2itunary) (DLListIterator);

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* ------------------------------------------------------------------------- */
/** Single item on the list.
 */
struct _DLListItem {
    /** Holds the element's data, which can be a pointer to any kind of data */
        void           *data_;
    /** Link to the next element of the list. */
        DLListItem     *next_;
    /** Link to the previous element of the list. */
        DLListItem     *prev_;
    /** Link to the list which owns this item */
        DLList         *list_;
};
/* ------------------------------------------------------------------------- */
/** Double linked list itelf
 */
struct _DLList {
    /** Link to the first element on the list. */
        DLListItem     *head_;
    /** Link to the last element on the list. */
        DLListItem     *tail_;
};
/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
DLList         *dl_list_create ();
/* ------------------------------------------------------------------------- */
void            dl_list_free (DLList ** list_handle);
/* ------------------------------------------------------------------------- */
void            dl_list_free_data (DLList ** list_handle);
/* ------------------------------------------------------------------------- */
int             dl_list_add (DLList * list_handle, void *data);
/* ------------------------------------------------------------------------- */
int             dl_list_add_at (DLList * list_handle, void *data,
                                unsigned int pos);
/* ------------------------------------------------------------------------- */
int             dl_list_remove_it (DLListIterator it);
/* ------------------------------------------------------------------------- */
DLListIterator  dl_list_find (DLListConstIterator begin,
                              DLListConstIterator end, ptr2compare unary,
                              const void *data);
/* ------------------------------------------------------------------------- */
int             dl_list_size (const DLList * list_handle);
/* ------------------------------------------------------------------------- */
DLListIterator  dl_list_head (const DLList * list_handle);
/* ------------------------------------------------------------------------- */
DLListIterator  dl_list_tail (const DLList * list_handle);
/* ------------------------------------------------------------------------- */
void           *dl_list_data (DLListConstIterator it);
/* ------------------------------------------------------------------------- */
DLListIterator  dl_list_at (const DLList * list_handle, unsigned int i);
/* ------------------------------------------------------------------------- */
DLListIterator  dl_list_next (DLListConstIterator it);
/* ------------------------------------------------------------------------- */
DLListIterator  dl_list_prev (DLListConstIterator it);
/* ------------------------------------------------------------------------- */
int             dl_list_alter_it (DLListIterator it, const void *data);
/* ------------------------------------------------------------------------- */
int             dl_list_foreach (DLListConstIterator begin,
                                 DLListConstIterator end, ptr2unary unary);
/* ------------------------------------------------------------------------- */
int             dl_list_foreach_it (DLListConstIterator begin,
                                    DLListConstIterator end,
                                    ptr2itunary unary);
/* ------------------------------------------------------------------------- */
#endif                          /* DLLIST_H */
/* End of file */
