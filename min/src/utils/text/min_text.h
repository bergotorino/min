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
 *  @file       text.h
 *  @version    0.1
 *  @brief      This file contains header file of the Text implementation.
 */

#ifndef MIN_TEXT_H
#define MIN_TEXT_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <stdlib.h>
#include <errno.h>
#include <min_common.h>

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
#define MaxTextBufferSize 128

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* ------------------------------------------------------------------------- */
/** String implementation for MIN.
 *  Just a basic text handling structure */
typedef struct {
        /** The text is stored here. */
        char           *buf_;

        /** Current size of the string. */
        unsigned int    size_;

        /** Max size of the text that will fit. */
        unsigned int    max_size_;
} Text;
/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
/** Creates Text structure and initializes it with given value.
 *  @param txt[in] the string that will initialize Text interface.
 *  @return allocated Text structure or INITPTR in case of failure.
 */
Text           *tx_create (const char *txt);
/* ------------------------------------------------------------------------- */
/** Destroys Text structure previously allocated with tx_create.
 *  @param txt[in:out] Text interface to be deallocated.
 *
 *  @note after deletion the pointer to Text interface is set to INITPTR
 */
void            tx_destroy (Text ** txt);
/* ------------------------------------------------------------------------- */
/** Appends to one Text interface another.
 *  @param dest[in:out] this one will be appended with src
 *  @param src[in] this one will be appended to dest
 */
void            tx_append (Text * dest, const Text * src);
/* ------------------------------------------------------------------------- */
/** Copies content of one Text to another.
 *  @param dest[out] destination of the copy operation.
 *  @param src[in] source for the copy operation.
 */
void            tx_copy (Text * dest, const Text * src);
/* ------------------------------------------------------------------------- */
/** Appends to Text interface c string.
 *  @param dest[in:out] this one will be appended with src
 *  @param src[in] c string to be appended to dest
 */
void            tx_c_append (Text * dest, const char *src);
/* ------------------------------------------------------------------------- */
/** Appends to one Text interface another.
 *  @param dest[in:out] this one will be appended with src
 *  @param src[in] this one will be preppended to dest
 */
void            tx_prepend (Text * src, Text * dest);
/* ------------------------------------------------------------------------- */
/** Prepeends to Text interface c string.
 *  @param dest[in:out] this one will be appended with src
 *  @param src[in] c string to be preppended to dest
 */
void            tx_c_prepend (Text * src, const char * dest);
/* ------------------------------------------------------------------------- */
/** Copies content of c string to Text interface.
 *  @param dest[out] destination of the copy operation.
 *  @param src[in] source for the copy operation.
 */
void            tx_c_copy (Text * dest, const char *src);
/* ------------------------------------------------------------------------- */
/** Getter for c string representation of data held in Text interface.
 *  @param txt[in] Text interface from which the data are returned.
 *  @return pointer to copy of the data held by Text interface.
 *
 *  @note obtained pointer needs to be freed by using for example free()
 */
char           *tx_get_buf (Text * txt);
/* ------------------------------------------------------------------------- */
/** Getter for adress of data held in Text interface.
 *  @param txt[in] Text interface from which the pointer is returned.
 *  @return pointer to the data held by Text interface.
 *
 *  @note obtainted pointer should not be freed by using free()
 */
char           *tx_share_buf (Text * txt);
/* ------------------------------------------------------------------------- */
/** Removes specified set of characters from the end of Text
 *  @param txt[in:out] the Text to be altered.
 *  @param chars[in] set of charatcer that will be removed from the end of Text
 */
void            tx_back_trim (Text * txt, const char *chars);
/* ------------------------------------------------------------------------- */
/** Gives character from the Text that is at given position.
 *  @param txt[in] Text from which character is returned.
 *  @param index[in] denotes the index of character in Text. It starts from 0
 *  @return character that is at index position in Text. In case of failure
 *  \0 is returned.
 */
char            tx_at (Text * txt, unsigned int index);
/* ------------------------------------------------------------------------- */
/** Appends to Text interface an integer.
 *  @param dest[in:out] this one will be appended with src
 *  @param flags[in] like in printf: [flags][width][.precision][length]
 *  @param src[in] integer to be appended to dest
 */
void tx_int_append(Text * dest,const char *flags, int src);
/* ------------------------------------------------------------------------- */
#endif                          /* MIN_TEXT_H */
/* End of file */
