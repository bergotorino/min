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
 *  @file       min_text.c
 *  @version    0.1
 *  @brief      This file contains implementation of Text interface.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <stdio.h>
#include <min_text.h>

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

/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* None */

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Creates Text structure and initializes it with given value.
 *  @param txt[in] the string that will initialize Text interface.
 *  @return allocated Text structure or INITPTR in case of failure.
 */
Text           *tx_create (const char *txt)
{
        Text           *retval = NEW (Text);
        unsigned int    tlen = 0;
        unsigned int    tmp = 0;

        retval->max_size_ = MaxTextBufferSize;
        retval->buf_ = NEW2 (char, MaxTextBufferSize);
        memset (retval->buf_, '\0', MaxTextBufferSize);
        retval->size_ = 0;

        if (txt == INITPTR || txt == NULL) {
                goto EXIT;
        }

        tlen = strlen (txt);

        if (tlen == 0) {
                goto EXIT;
        }

        tmp = ((int)((float)tlen / (float)MaxTextBufferSize)) + 1;

        if (tmp > 1) {
                DELETE (retval->buf_);
                retval->max_size_ = tmp * MaxTextBufferSize;
                retval->buf_ = NEW2 (char, retval->max_size_);
                memset (retval->buf_, '\0', retval->max_size_);
        }

        STRCPY (retval->buf_, txt, tlen);
        retval->size_ = tlen;
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Destroys Text structure previously allocated with tx_create.
 *  @param txt[in:out] Text interface to be deallocated.
 *
 *  @note after deletion the pointer to Text interface is set to INITPTR
 */
void tx_destroy (Text ** txt)
{
        if (txt != INITPTR) {
                if (*txt != INITPTR) {
                        DELETE ((*txt)->buf_)
                            DELETE (*txt);
                        *txt = INITPTR;
                }
        }
}

/* ------------------------------------------------------------------------- */
/** Appends one Text instance to another.
 *  @param dest[in:out] this one will be appended with src
 *  @param src[in] this one will be appended to dest
 */
void tx_append (Text * dest, const Text * src)
{
        unsigned int    space = 0;
        unsigned int    totalsize = 0;
        unsigned int    tmp = 0;
        char           *tmpbuf = INITPTR;

        if (dest == INITPTR) {
                goto EXIT;
        }
        if (src == INITPTR) {
                goto EXIT;
        }

        space = dest->max_size_ - dest->size_;

        if (src->size_ > space) {
                totalsize = dest->size_ + src->size_;
                tmp =
                    ((int)((float)totalsize / (float)MaxTextBufferSize)) + 1;
                dest->max_size_ = tmp * MaxTextBufferSize;
                tmpbuf = NEW2 (char, dest->max_size_);
                memset (tmpbuf, '\0', dest->max_size_);
                STRCPY (tmpbuf, dest->buf_, dest->size_);
                STRCPY (&tmpbuf[dest->size_], src->buf_, src->size_);
                dest->size_ = totalsize;
                DELETE (dest->buf_);
                dest->buf_ = tmpbuf;
        } else {
                STRCPY (&dest->buf_[dest->size_], src->buf_, src->size_);
                dest->size_ += src->size_;
        }

      EXIT:
        return;
}
/* ------------------------------------------------------------------------- */
/** Copies content of one Text to another.
 *  @param dest[out] destination of the copy operation.
 *  @param src[in] source for the copy operation.
 */
void tx_copy (Text * dest, const Text * src)
{
        if (dest == INITPTR) {
                goto EXIT;
        }
        if (src == INITPTR) {
                goto EXIT;
        }
        if (dest == src) {
                goto EXIT;
        }

        DELETE (dest->buf_);

        dest->max_size_ = src->max_size_;
        dest->buf_ = NEW2 (char, dest->max_size_);
        memset (dest->buf_, '\0', dest->max_size_);
        dest->size_ = src->size_;

        STRCPY (dest->buf_, src->buf_, dest->size_);
      EXIT:
        return;
}
/* ------------------------------------------------------------------------- */
/** Appends to Text interface c string.
 *  @param dest[in:out] this one will be appended with src
 *  @param src[in] c string to be appended to dest
 */
void tx_c_append (Text * dest, const char *src)
{
        unsigned int    space = 0;
        unsigned int    totalsize = 0;
        unsigned int    tmpsize = 0;
        unsigned int    slen = 0;
        char           *tmpbuf = INITPTR;

        if (dest == INITPTR) {
                goto EXIT;
        }
        if (src == INITPTR) {
                goto EXIT;
        }

        space = dest->max_size_ - dest->size_;
        slen = strlen (src);

        if (slen >= space) {
                totalsize = dest->size_ + slen;
                tmpsize =
                    ((int)((float)totalsize / (float)MaxTextBufferSize)) + 1;
                dest->max_size_ = tmpsize * MaxTextBufferSize;
                tmpbuf = NEW2 (char, dest->max_size_);
                memset (tmpbuf, '\0', dest->max_size_);
                STRCPY (tmpbuf, dest->buf_, dest->size_);
                STRCPY (&tmpbuf[dest->size_], src, slen);
                dest->size_ = totalsize;
                DELETE (dest->buf_);
                dest->buf_ = tmpbuf;
        } else {
                STRCPY (&dest->buf_[dest->size_], src, slen);
                dest->size_ += slen;
        }

      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
/** Prepends  one Text instance to another.
 *  @param dest[in:out] this one will be appended with src
 *  @param src[in] this one will be preppended to dest
 */
void tx_prepend (Text * src, Text * dest)
{
        Text *tmp=INITPTR;
        if( src==INITPTR || dest == INITPTR ) return;
        tmp=src;
        src=dest;
        dest=tmp;
        tx_append(src,dest);
}

/* ------------------------------------------------------------------------- */
/** Prepends to Text interface c string.
 *  @param dest[in:out] this one will be appended with src
 *  @param src[in] c string to be preppended to dest
 */
void tx_c_prepend (Text * src, const char * dest)
{
        Text *tmp=INITPTR;
        if( src==INITPTR || dest == INITPTR ) return;
        tmp = tx_create(dest);
        tx_append(tmp,src);
        tx_copy(src,tmp);
        tx_destroy (&tmp);
}

/* ------------------------------------------------------------------------- */
/** Copies content of c string to Text interface.
 *  @param dest[out] destination of the copy operation.
 *  @param src[in] source for the copy operation.
 */
void tx_c_copy (Text * dest, const char *src)
{
        unsigned int    slen = 0;
        unsigned int    tmp = 0;

        if (dest == INITPTR) {
                goto EXIT;
        }
        if (src == INITPTR) {
                goto EXIT;
        }
        if (dest->buf_ == src) {
                goto EXIT;
        }

        slen = strlen (src);
        tmp = ((int)((float)slen / (float)MaxTextBufferSize)) + 1;
        DELETE (dest->buf_);

        dest->max_size_ = tmp * MaxTextBufferSize;
        dest->buf_ = NEW2 (char, dest->max_size_);
        memset (dest->buf_, '\0', dest->max_size_);
        dest->size_ = slen;

        STRCPY (dest->buf_, src, slen);
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
/** Getter for c string representation of data held in Text interface.
 *  @param txt[in] Text interface from which the data are returned.
 *  @return pointer to copy of the data held by Text interface.
 *
 *  @note obtained pointer needs to be freed by using for example free()
 */
char           *tx_get_buf (Text * txt)
{
        char           *retval;
        if (txt == INITPTR) {
                return INITPTR;
        } else {
                retval = NEW2 (char, txt->size_ + 1);
                STRCPY (retval, txt->buf_, txt->size_ + 1);
                return retval;
        }
}

/* ------------------------------------------------------------------------- */
/** Getter for address of data held in Text interface.
 *  @param txt[in] Text interface from which the pointer is returned.
 *  @return pointer to the data held by Text interface.
 *
 *  @note obtainted pointer should not be freed by using free()
 */
char           *tx_share_buf (Text * txt)
{
        if (txt == INITPTR) {
                return INITPTR;
        } else {
                return txt->buf_;
        }
}
/* ------------------------------------------------------------------------- */
/** Removes specified set of characters from the end of Text
 *  @param txt[in:out] the Text to be altered.
 *  @param chars[in] set of charatcer that will be removed from the end of Text
 */
void tx_back_trim (Text * txt, const char *chars)
{
        char           *c = INITPTR;
        int             tmp = 0;
        int             tmp2 = 0;
        if (txt == INITPTR) {
                return;
        }
        if (chars == INITPTR) {
                return;
        }
        if (txt->size_ == 0) {
                return;
        }
        tmp = ((int)((float)txt->size_ / (float)MaxTextBufferSize)) + 1;
        c = &txt->buf_[txt->size_ - 1];
        while (strchr (chars, *c) != NULL && c != &txt->buf_[0]) {
                c--;
        }
        *(c + 1) = '\0';
        txt->size_ = strlen (txt->buf_);
        tmp2 = ((int)((float)txt->size_ / (float)MaxTextBufferSize)) + 1;
        if (tmp != tmp2) {
                txt->max_size_ = tmp2 * MaxTextBufferSize;
                c = NEW2 (char, txt->max_size_);
                memset (c, '\0', txt->max_size_);
                STRCPY (c, txt->buf_, txt->size_);
                DELETE (txt->buf_);
                txt->buf_ = c;
        }
}

/* ------------------------------------------------------------------------- */
/** Gives character from the Text that is at given position.
 *  @param txt[in] Text from which character is returned.
 *  @param index[in] denotes the index of character in Text. It starts from 0
 *  @return character that is at index position in Text. In case of failure
 *  \0 is returned.
 */
char tx_at (Text * txt, unsigned int index)
{
        if(txt==INITPTR || index>=txt->size_) return '\0';
        else return txt->buf_[index];
}

/* ------------------------------------------------------------------------- */
/** Appends to Text interface an integer.
 *  @param dest[in:out] this one will be appended with src
 *  @param flags[in] like in printf: [flags][width][.precision][length]
 *  @param src[in] integer to be appended to dest
 */
void tx_int_append(Text * dest,const char *flags, int src)
{
        /* assumed that there will be no number longer than 63 digits */
        char tmp[64];
        char format[64];
        if (dest==INITPTR) { return; }
        memset(tmp,'\0',64);
        memset(format,'\0',64);
        sprintf(format,"%%%sd",flags);
        sprintf(tmp,format,src);
        tx_c_append(dest,tmp);
}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "text.tests"
#endif                          /* MIN_UNIT_TEST */
/* ------------------------------------------------------------------------- */
/* End of file */
