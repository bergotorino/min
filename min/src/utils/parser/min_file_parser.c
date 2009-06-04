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
 *  @file       min_file_parser.c
 *  @version    0.1
 *  @brief      This file contains implementation of Min File Parser
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <unistd.h>
#include "min_file_parser.h"

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
/*#define MIN_UNIT_TEST*/

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
/** Reads configuration source and returns a complete line.
 * This method will return also and end of line sequence.
 * @param line_buffer [out] handler to which the buffer will be attached.
 * @param eol_buffer [out] table for eol sequence.
 * @return > 0 on success, 0 on failure.
 */
LOCAL TSBool    mfp_read_line (MinFileParser * sfp,
                               TSChar ** line_buffer, TSChar * eol_buffer);
/* ------------------------------------------------------------------------- */
/**
 * Parses a given line and removes all c-style comments from it.
 * Result is passed in destination buffer.
 * @param src_buff [in] buffer to be processed.
 * @param dst_buff [out] the result of processing.
 * @param find [in] what kind of comments we are looking for.
 */
LOCAL void      mfp_replace_comments_line (const TSChar * src_buff,
                                           TSChar ** dst_buff,
                                           TWhatToFind find);
/* ------------------------------------------------------------------------- */
/**
 * Parses a given line and removes all #-style comments.
 * Result is passed in destination buffer.
 * @param src_buff [in] buffer to be processed.
 * @param dst_buff [out] the result of processing.
 */
LOCAL void      mfp_replace_hash_comments_line (const TSChar * src_buff,
                                                TSChar ** dst_buff);
/* ------------------------------------------------------------------------- */
/** Closes all files on file stack and clears the stack.
 *  @param sfp [in] pointer to the MinFileParser to operate on.
 */
LOCAL void      mfp_clear_file_stack (MinFileParser * sfp);
/* ------------------------------------------------------------------------- */
/** Pops FILE handle from file stack and sets correct current file handle.
 *  @param sfp [in] pointer to the MinFileParser to operate on.
 */
LOCAL void      mfp_pop_from_file_stack (MinFileParser * sfp);
/* ------------------------------------------------------------------------- */
/** Opens file and pushes it to stack.
 *  @param sfp [in] pointer to the MinFileParser to operate on.
 *  @param file_name [in] value to be pushed to the stack.
 */
LOCAL void      mfp_push_file_to_stack (MinFileParser * sfp,
                                        const TSChar * file_name);
/* ------------------------------------------------------------------------- */
/** Deletes all descriptors assigned to array and empties array.
 *  @param sfp [in] pointer to the MinFileParser to operate on.
 */
/* LOCAL void clear_section_lines_array( MinFileParser* sfp );missing -ssaa */
/* ------------------------------------------------------------------------- */
/** Used for comparing filenames on the list */
LOCAL int       compare_filename (const void *a, const void *b);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
LOCAL int compare_filename (const void *a, const void *b)
{
        int             retval =
            strcmp ((const TSChar *)a, (const TSChar *)b);

        if (retval == 0)
                return 0;
        if (retval < 0)
                return -1;
        if (retval > 0)
                return 1;

        return retval;
}

/* ------------------------------------------------------------------------- */
LOCAL TSBool mfp_read_line (MinFileParser * sfp, TSChar ** line_buffer,
                            TSChar * eol_buffer)
{
        TSBool          retval = ESFalse;
        Text            *buf = tx_create ("");
        TSChar          *c = INITPTR;
        TSChar          tmp[2];
        tmp[0] = '\0';
        tmp[1] = '\0';

        if (sfp == INITPTR) {
                errno = EINVAL;
                return retval;
        }
        if (eol_buffer == INITPTR) {
                errno = EINVAL;
                return retval;
        }

        /* Reset eol buffer */
        eol_buffer[0] = '\0';
        eol_buffer[1] = '\0';
        eol_buffer[2] = '\0';

        /* 1. Read whole line, char by char till endline character is meet */
        while (1) {
                tmp[0] = (TSChar) getc (sfp->current_file_);

                if (tmp[0] == '\n') {

                        /* It is newline, so we have to check if line contains
                         * INCLUDE keyword
                         */
                        eol_buffer[0] = '\n';
                        c = strstr (buf->buf_, "INCLUDE");
                        if (c != NULL) {
                                c += 7;
                                while (strchr (" \n\t\r", *c) != NULL) {
                                        c++;
                                }
                                if (strlen (c) > 0) {
                                        mfp_push_file_to_stack (sfp, c);
                                        sfp->eol_buff_[0] = eol_buffer[0];
                                        sfp->eol_buff_[1] = eol_buffer[1];
                                        sfp->eol_buff_[2] = eol_buffer[2];
                                }
                        }

                        retval = ESTrue;
                        break;

                } else if (((char)tmp[0] == (char)EOF) || feof (sfp->current_file_)) {

                        if (dl_list_size (sfp->file_stack_) > 0) {
                                /* Try to pop file from stack */
                                mfp_pop_from_file_stack (sfp);
                                retval = ESTrue;
                        } else {
                                if (strlen (buf->buf_) > 0)
                                        retval = ESTrue;
                                else
                                        retval = ESFalse;
                                break;
                        }
                }

                tx_c_append (buf, tmp);
        }
        if (retval ==  ESTrue) {
                tx_back_trim (buf, " \n\r\t");
                *line_buffer = tx_get_buf (buf);
                tx_destroy (&buf);
        }
        else {
                *line_buffer = INITPTR;
                tx_destroy (&buf);
        }
        return retval;
}

/* ------------------------------------------------------------------------- */
LOCAL void mfp_replace_comments_line (const TSChar * src_buff,
                                      TSChar ** dst_buff, TWhatToFind find)
{

        const TSChar   *c;
        TSChar         *buff;
        TSChar         *cc;
        int             len;
        TSChar          space, slash, quota, multi, hash, append[2];

        if (src_buff == INITPTR)
                return;

        c = &src_buff[0];
        buff = NEW2 (TSChar, strlen (src_buff) + 1);
        cc = &buff[0];
        len = 0;
        space = ' ';
        slash = '/';
        quota = '\"';
        multi = '*';
        hash = '#';
        append[0] = '\0';
        append[1] = '\0';

        while (*c != '\0') {

                /* We're looking for start of quoted text or comment */
                if (find == EStart) {

                        if (*c == quota) {
                                find = EQuota;
                                append[0] = space;
                                STRCPY (cc, append, strlen (append));
                                cc = cc + strlen (append);
                        } else if (*c == slash) {
                                if (*(c + 1) == slash) {
                                        /* Comment to the end of
                                           line found */
                                        break;
                                } else if (*(c + 1) == multi) {
                                        /* Beginning of a comment found */
                                        find = EEndOfComment;
                                        c++;
                                        append[0] = space;
                                        STRCPY (cc, append, strlen (append));
                                        cc = cc + strlen (append);
                                        STRCPY (cc, append, strlen (append));
                                        cc = cc + strlen (append);
                                } else {
                                        /* No start of comment - add read
                                           slash */
                                        append[0] = *c;
                                        STRCPY (cc, append, strlen (append));
                                        cc = cc + strlen (append);
                                }
                        } else if (*c == hash) {
                                /* Start of hash comment
                                   (to the end of line) */
                                break;
                        } else {
                                /* Append readed TSCharacter to the destination
                                   buffer */
                                append[0] = *c;
                                STRCPY (cc, append, strlen (append));
                                cc = cc + strlen (append);
                        }

                } else if (find == EQuota) {
                        /* We're looking for the end of quoted text */
                        if (*c == quota) {
                                find = EStart;
                        }
                        append[0] = space;
                        STRCPY (cc, append, strlen (append));
                        cc = cc + strlen (append);
                } else if (find == EEndOfComment) {
                        /* We're looking for the end of comment */
                        if (*c == multi) {
                                /* It may be the end of a comment */
                                if (*(c + 1) == slash) {
                                        /* It is the end of a comment */
                                        find = EStart;
                                        c++;
                                        append[0] = space;
                                        STRCPY (cc, append, strlen (append));
                                        cc = cc + strlen (append);
                                        STRCPY (cc, append, strlen (append));
                                        cc = cc + strlen (append);
                                } else {
                                        /* It is not the end of a comment,
                                           add this TSCharacter to the
                                           destinaton buffer */
                                        append[0] = *c;
                                        STRCPY (cc, append, strlen (append));
                                        cc = cc + strlen (append);
                                }
                        } else {
                                /* It is not the end of a comment,
                                   add this TSCharacter to the destinaton
                                   buffer */
                                append[0] = space;
                                STRCPY (cc, append, strlen (append));
                                cc = cc + strlen (append);
                        }
                }
                c++;
        }

        c = &buff[0];

        while (*c == ' ' && c != cc) {
                c++;
        }
        cc--;
        while (*cc == ' ' && cc + 1 != c) {
                cc--;
        }

        if (cc == &buff[0])
                *(cc) = '\0';
        else
                *(cc + 1) = '\0';

        len = strlen (c);

        *dst_buff = NEW2 (TSChar, len + 2);
        STRCPY (*dst_buff, c, len + 1);
        (*dst_buff)[len] = '\0';

        free (buff);
}

/* ------------------------------------------------------------------------- */
LOCAL void mfp_replace_hash_comments_line (const TSChar * src_buff,
                                           TSChar ** dst_buff)
{

        const TSChar   *c;
        TSChar         *buff;
        TSChar         *cc;
        TSChar          hash;
        TSChar          append[2];

        if (src_buff == INITPTR)
                return;

        c = &src_buff[0];
        buff = NEW2 (TSChar, strlen (src_buff) + 1);
        cc = &buff[0];
        hash = '#';
        append[0] = '\0';
        append[1] = '\0';

        while (*c != '\0') {

                if (*c == hash)
                        break;
                else {
                        append[0] = *c;
                        STRCPY (cc, append, strlen (append));
                        cc = cc + strlen (append);
                }
                c++;
        }

        append[0] = '\0';
        STRCPY (cc, append, strlen (append));
        cc = cc + strlen (append);

        /* Point buff to the output */
        *dst_buff = buff;
}

/* ------------------------------------------------------------------------- */
LOCAL void mfp_clear_file_stack (MinFileParser * sfp)
{
        if (sfp == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }

        while (dl_list_size (sfp->file_stack_) != 0) {
                mfp_pop_from_file_stack (sfp);
        }
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
LOCAL void mfp_pop_from_file_stack (MinFileParser * sfp)
{
        DLListIterator  it = DLListNULLIterator;
        DLListIterator  it2 = DLListNULLIterator;
        TSChar          file[4096];
        TSChar          buf[64];
	ssize_t         rval;

        if (sfp == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }
        if (dl_list_size (sfp->file_stack_) == 0) {
                goto EXIT;
        }

        /* prepare table */
        memset (file, 0x0, 4096);

        /* Pop from stack */
        it = dl_list_tail (sfp->file_stack_);
        sfp->current_file_ = (FILE *) dl_list_data (it);
        dl_list_remove_it (it);

        /* Lets get filename associated with this descriptor */
        sprintf (buf, "/proc/self/fd/%d", fileno (sfp->current_file_));
        rval = readlink (buf, &file[0], 4095);
	if (rval < 0) {
		return;
	}
        /* Remove from file names list */
        it2 = dl_list_find (dl_list_head (sfp->file_names_)
                            , dl_list_tail (sfp->file_names_)
                            , compare_filename, file);

        if (it2 != DLListNULLIterator) {
                dl_list_remove_it (it2);
        }

        /* Close file */
        fclose (sfp->current_file_);

        /* Set correct current file */
        if (dl_list_size (sfp->file_stack_) == 0) {
                sfp->current_file_ = sfp->base_file_;
        } else {
                it = dl_list_tail (sfp->file_stack_);
                sfp->current_file_ = (FILE *) dl_list_data (it);
        }
        /* Lets get filename associated with this descriptor */
        memset (file, 0x0, 4096);
        memset (buf, 0x0, 64);
        sprintf (buf, "/proc/self/fd/%d", fileno (sfp->current_file_));
        rval = readlink (buf, &file[0], 4095);

      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
LOCAL void mfp_push_file_to_stack (MinFileParser * sfp,
                                   const TSChar * file_name)
{
        DLListIterator  it = DLListNULLIterator;
        FILE           *fp = INITPTR;

        if (sfp == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }
        if (file_name == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }

        /* Check if file is not laready included */
        it = dl_list_find (dl_list_head (sfp->file_names_)
                           , dl_list_tail (sfp->file_names_)
                           , compare_filename, (const void *)file_name);

        if (it != DLListNULLIterator) {
                goto EXIT;
        }

        /* Open file */
        fp = fopen (file_name, "r");

        if (fp == NULL) {
                goto EXIT;
        }

        /* Add to stack */
        dl_list_add (sfp->file_stack_, (void *)fp);

        /* Add name to names array */
        dl_list_add (sfp->file_names_, (void *)file_name);

        /* Set current file */
        sfp->current_file_ = fp;

      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
MinFileParser *mfp_create (FILE * file, TUnicode is_unicode,
                            TCommentType comment_type)
{
        MinFileParser *tmp = INITPTR;
        TSChar          path[4096];
        TSChar          buf[64];
	ssize_t         rval;

        if (file == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }
        if (file == NULL) {
                errno = EINVAL;
                goto EXIT;
        }

        /* Initialize */
        tmp = NEW (MinFileParser);
        tmp->base_file_ = file;
        tmp->comment_type_ = comment_type;
        tmp->is_unicode_ = is_unicode;
        tmp->bytes_per_char_ = tmp->is_unicode_ ? 2 : 1;
        tmp->current_file_ = tmp->base_file_;
        tmp->section_lines_ = INITPTR;  /* dl_list_create(); 
                                           mfp_read_line() creates */
        tmp->file_stack_ = dl_list_create ();
        tmp->file_names_ = dl_list_create ();
        tmp->eol_buff_[0] = '\0';
        tmp->eol_buff_[1] = '\0';
        tmp->eol_buff_[2] = '\0';

        /* prepare table */
        memset (path, 0x0, 4096);

        /* Lets get filename associated with this descriptor */
        sprintf (buf, "/proc/self/fd/%d", fileno (file));
        rval = readlink (buf, &path[0], 4095);
	if (rval < 0) {
		return NULL;
	}

        /* Add base file to the file names array */
        dl_list_add (tmp->file_names_, path);
      EXIT:
        return tmp;
}

/* ------------------------------------------------------------------------- */
void mfp_destroy (MinFileParser ** sfp)
{
        if (*sfp == INITPTR)
                return;

        mfp_clear_file_stack (*sfp);

        if ((*sfp)->section_lines_ != INITPTR) {
                dl_list_free_data (&((*sfp)->section_lines_));
                dl_list_free (&((*sfp)->section_lines_));
        }
        dl_list_free_data (&((*sfp)->file_stack_));
        dl_list_free (&((*sfp)->file_stack_));

        dl_list_free (&((*sfp)->file_names_));

        free (*sfp);
        *sfp = INITPTR;
}

/* ------------------------------------------------------------------------- */
TSChar         *mfp_next_section (MinFileParser * sfp,
                                  const TSChar * start_tag,
                                  const TSChar * end_tag, int *offset,
                                  int seeked)
{
        TSChar         *retval = INITPTR;
        int             found_section = 0;
        int             ret = 0;
        int             size = 0;
        TSChar         *buff = INITPTR;
        TSChar         *without_comments_buff = INITPTR;
        TSChar         *line = INITPTR;
        TSChar          end_of_line_buff[3];
        int             validSectionBeginFound = 0;
        int             validSectionEndFound = 0;
        TSectionFind    whatToFindSection = ESectionStart;
        TWhatToFind     whatToFind = EStart;
        DLListIterator  it = DLListNULLIterator;
        int             buff_len = 0;

        if (seeked < 1) {
                errno = EINVAL;
                goto EXIT;
        }
        if (*offset < 0) {
                errno = EINVAL;
                goto EXIT;
        }
        if (sfp == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }
        if (start_tag == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }
        if (end_tag == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }
        end_of_line_buff[0] = '\0';
        end_of_line_buff[1] = '\0';
        end_of_line_buff[2] = '\0';

        sfp->section_lines_ = dl_list_create ();

        if (*offset == 0) {
                /* Set valid position in file
                   but only if offset shows to 0. Otherwise keep previous
                   position */
                fseek (sfp->current_file_, 0L, SEEK_SET);
                /*rewind( sfp->current_file_ ); */
                *offset = 1;
        }

        /* If no start tag is given start to find end tag immediatly */
        if (strlen (start_tag) == 0) {
                found_section++;
                whatToFindSection = ESectionEnd;
                validSectionBeginFound = 1;
        }
        if (strlen (end_tag) == 0)
                validSectionEndFound = 1;

        /* Perform reading file */
        while (mfp_read_line (sfp, &buff, end_of_line_buff) != ESFalse) {

                if (sfp->comment_type_ == ECStyleComments) {
                        mfp_replace_comments_line (buff,
                                                   &without_comments_buff,
                                                   whatToFind);
                } else {
                        mfp_replace_hash_comments_line (buff,
                                                        &without_comments_buff);
                }

                if (whatToFindSection == ESectionStart) {
                        /* Find in line star tag (if start tag is not given,
                           behave like we've found it) */
                        if (strstr (without_comments_buff, start_tag) != NULL)
                                ret = 0;
                        else
                                ret = -1;

                        if (ret >= 0) {

                                whatToFindSection = ESectionEnd;
                                whatToFind = EStart;    /* reset marker */
                                found_section++;

                                /* Add this line to section lines array */
                                if (found_section == seeked) {

                                        /* Add start tag if line 
                                         * contains something more
                                         * than just a start tag. */
                                        if (strlen (start_tag) <
                                            strlen (without_comments_buff)) {
                                                buff_len =
                                                    strlen
                                                    (without_comments_buff) +
                                                    1;

                                                line =
                                                    NEW2 (TSChar,
                                                          buff_len +
                                                          strlen
                                                          (end_of_line_buff) +
                                                          1);

                                                STRCPY (line,
                                                        without_comments_buff,
                                                        buff_len);

                                                STRCPY (&line[buff_len - 1]
                                                        , &end_of_line_buff[0]
                                                        ,
                                                        strlen
                                                        (end_of_line_buff) +
                                                        2);

                                                dl_list_add (sfp->
                                                             section_lines_,
                                                             (void *)line);
                                        }
                                        validSectionBeginFound = 1;
                                }
                                goto loopend;
                        }
                } else if (whatToFindSection == ESectionEnd) {
                        /* Find in line star tag (if start tag is not given,
                           behave like we've found it) */

                        if (strstr (without_comments_buff, end_tag) != NULL)
                                ret = 0;
                        else
                                ret = -1;

                        if (ret >= 0) {


                                whatToFindSection = ESectionStart;
                                whatToFind = EStart;    /* reset marker */

                                if (found_section == seeked) {
                                        /* Add this line to 
                                         * section lines array */
                                        if (strlen (end_tag) <
                                            strlen (without_comments_buff)) {
                                                buff_len =
                                                    strlen
                                                    (without_comments_buff) +
                                                    1;

                                                TSChar         *line =
                                                    NEW2 (TSChar,
                                                          buff_len +
                                                          strlen
                                                          (end_of_line_buff) +
                                                          1);
                                                STRCPY (line,
                                                        without_comments_buff,
                                                        buff_len);

                                                STRCPY (&line[buff_len - 1]
                                                        , &end_of_line_buff[0]
                                                        ,
                                                        strlen
                                                        (end_of_line_buff) +
                                                        2);

                                                dl_list_add (sfp->
                                                             section_lines_,
                                                             (void *)line);
                                        }
                                        validSectionEndFound = 1;
                                        DELETE (buff);
                                        DELETE (without_comments_buff);
                                        break;
                                } else {
                                        goto loopend;
                                }
                        } else {
                                /* If we're in section we are looking for, 
                                 * add line to array */
                                if (found_section == seeked) {


                                        buff_len =
                                            strlen (without_comments_buff) +
                                            1;

                                        line =
                                            NEW2 (TSChar,
                                                  buff_len +
                                                  strlen (end_of_line_buff) +
                                                  1);

                                        STRCPY (line, without_comments_buff,
                                                buff_len);

                                        STRCPY (&line[buff_len - 1]
                                                , &end_of_line_buff[0]
                                                ,
                                                strlen (end_of_line_buff) +
                                                2);

                                        dl_list_add (sfp->section_lines_,
                                                     (void *)line);
                                }
                        }
                }
              loopend:
                /* Clean data */
                DELETE (buff);
                DELETE (without_comments_buff);

        }                       /* while */

        /* Load into section if found */
        if (validSectionBeginFound && validSectionEndFound) {
                /* Count amount of memory needed for section */
                it = dl_list_head (sfp->section_lines_);

                while (it != DLListNULLIterator) {
                        TSChar         *tmp = (TSChar *) dl_list_data (it);
                        size += strlen (tmp);
                        it = dl_list_next (it);

                        /* Do not copy last line that contains end tag */
                        //if( dl_list_next(it) == INITPTR ) { break; }
                }

                /* Copy section from array to buffer */
                retval = NEW2 (TSChar, size + 2);
                retval[size + 1] = '\0';
                buff = &retval[0];

                it = dl_list_head (sfp->section_lines_);
                while (it != DLListNULLIterator) {
                        TSChar         *tmp = (TSChar *) dl_list_data (it);


                        STRCPY (buff, tmp, strlen (tmp) + 1);

                        buff += strlen (tmp);
                        it = dl_list_next (it);

                        /* Do not copy last line that contains end tag */
                        //if( dl_list_next(it) == INITPTR ) { break; }
                }
        }

        /* Clean */
        it = dl_list_head (sfp->section_lines_);
        while (it != DLListNULLIterator) {
                free (it->data_);
                it = dl_list_next (it);
        }

        dl_list_free (&sfp->section_lines_);

      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */


/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "min_file_parser.tests"
#endif                          /* MIN_UNIT_TEST */
/* ------------------------------------------------------------------------- */
/* End of file */
