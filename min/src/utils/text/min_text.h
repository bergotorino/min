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
 *  @file       min_text.h
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
Text           *tx_create (const char *txt);
/* ------------------------------------------------------------------------- */
void            tx_destroy (Text ** txt);
/* ------------------------------------------------------------------------- */
void            tx_append (Text * dest, const Text * src);
/* ------------------------------------------------------------------------- */
void            tx_copy (Text * dest, const Text * src);
/* ------------------------------------------------------------------------- */
void            tx_c_append (Text * dest, const char *src);
/* ------------------------------------------------------------------------- */
void            tx_prepend (Text * src, Text * dest);
/* ------------------------------------------------------------------------- */
void            tx_c_prepend (Text * src, const char * dest);
/* ------------------------------------------------------------------------- */
void            tx_c_copy (Text * dest, const char *src);
/* ------------------------------------------------------------------------- */
char           *tx_get_buf (Text * txt);
/* ------------------------------------------------------------------------- */
char           *tx_share_buf (Text * txt);
/* ------------------------------------------------------------------------- */
void            tx_back_trim (Text * txt, const char *chars);
/* ------------------------------------------------------------------------- */
char            tx_at (Text * txt, unsigned int index);
/* ------------------------------------------------------------------------- */
void            tx_int_append(Text * dest,const char *flags, int src);
/* ------------------------------------------------------------------------- */
#endif                          /* MIN_TEXT_H */
/* End of file */
