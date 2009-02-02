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
 *  @file       min_parser.c
 *  @version    0.1
 *  @brief      This file contains implementation of MIN Parser
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include "min_parser.h"

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
/* ------------------------------------------------------------------------- */
/** Parses sections from configuration files.
 *
 *  Open and read configuration source and parses a required section.
 *  If start tag is empty the parsing starts beginning of the configuration
 *  file. If end tag is empty the parsing goes end of configuration file.
 *  This method will parse next section after the earlier section if seeked
 *  parameter is equal to 1.
 *  If configuration file includes several sections with both start and end
 *  tags so seeked parameter seeks the required section. The seeked
 *  parameters indicates section that will be parsed.
 *
 *  @param sp [in] adress of the MinParser
 *  @param start_tag [in] Indicates a start tag for parsing.
 *  @param end_tag [in] Indicates an end tag for parsing.
 *  @param seeked [in] a seeked section which will be parsed.
 *  @return MinSectionParser object or INITPTR in case of failure.
 */
MinSectionParser *mp_next_section_file (MinParser * sp,
                                         const TSChar * start_tag,
                                         const TSChar * end_tag, int seeked);
/* ------------------------------------------------------------------------- */
/** Parses sections from memory.
 *
 *  Open and read configuration source and parses a required section.
 *  If start tag is empty the parsing starts beginning of the configuration
 *  file. If end tag is empty the parsing goes end of configuration file.
 *  This method will parse next section after the earlier section if seeked
 *  parameter is set to 1.
 *  If configuration source includes several sections with both start and end
 *  tags so seeked parameter seeks the required section. The aSeeked
 *  parameters indicates section that will be parsed.
 *
 *  @param sp [in] adress of the MinParser
 *  @param start_tag [in] Indicates a start tag for parsing.
 *  @param end_tag [in] Indicates an end tag for parsing.
 *  @param seeked [in] a seeked section which will be parsed.
 *  @return MinSectionParser object or INITPTR in case of failure.
 *
 *  Possible Errors:
 *  - EINVAL: invalid parameter.
 *  - EFAULT: sp->offset bigger than section length
 */
MinSectionParser *mp_next_section_memory (MinParser * sp,
                                           const TSChar * start_tag,
                                           const TSChar * end_tag,
                                           int seeked);
/* ------------------------------------------------------------------------- */
/** Convert a section without comments.
 *
 *  @param buff [in:out] section to be processed
 */
void            mp_parse_comments_off (TSChar * buff);
/* ------------------------------------------------------------------------- */
/** Handles special marks.( '\/\/', '\/\*' and '*\/\/' ).
 *  This is used when ECStyleComments comment type is used.
 *
 *  @param buff [in:out] section to be processed
 */
void            mp_handle_special_marks (TSChar * buff);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
MinSectionParser *mp_next_section_file (MinParser * sp,
                                         const TSChar * start_tag,
                                         const TSChar * end_tag, int seeked)
{
        MinSectionParser *section = INITPTR;
        TSChar         *buf_section = INITPTR;

        if (sp == INITPTR) {
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

        buf_section =
            mfp_next_section (sp->file_parser_, start_tag, end_tag,
                              &(sp->offset_)
                              , seeked);

        if (buf_section != INITPTR) {
                if (sp->comment_type_ == ECStyleComments) {
                        mp_parse_comments_off (buf_section);
                }
                /* Make MinSectionParser object and alloc required length */
                section = mmp_create (strlen (buf_section) + 2);
                /* Copy required data to the section object */
                mmp_set_data (section, buf_section, &buf_section[0]
                              , strlen (buf_section) + 1);
                /* Clean */
                DELETE (buf_section);
        }

      EXIT:
        return section;
}

/* ------------------------------------------------------------------------- */
MinSectionParser *mp_next_section_memory (MinParser * sp,
                                           const TSChar * start_tag,
                                           const TSChar * end_tag, int seeked)
{
        MinSectionParser *section = INITPTR;
        TSChar         *whole_section = INITPTR;
        TSChar         *current_section = INITPTR;
        TSChar         *buff = INITPTR;
        unsigned int    size = 0;
        const unsigned int tmpsize = 128;
        int             read = 0;
        int             tag_count = 0;
        int             offset = 0;
        TSChar         *start_pos = INITPTR;
        TSChar         *end_pos = INITPTR;
        TSChar         *c = INITPTR;
        TSChar         *c2 = INITPTR;
        int             length = 0;

        if (sp == INITPTR) {
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
        if (sp->buffer_ == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }

        if (sp->parsing_mode_ == EBufferParsing) {
                /* Parser is created straight with data */
                size = strlen (sp->buffer_);
        } else {
                /* Parser is created with path and file information */
                size = fseek (sp->file_, 0, SEEK_END);
        }

        /* size or seeked cannot be 0 or negative */
        if (size <= 0 || seeked <= 0) {
                errno = EINVAL;
                goto EXIT;
        }

        whole_section = NEW2 (TSChar, size + 1);
        buff = NEW2 (TSChar, tmpsize + 1);
        current_section = NEW2 (TSChar, tmpsize + 1);

        /* Create parser */
        if (sp->parsing_mode_ == EBufferParsing) {
                /* Parser is created straight with data */
                STRCPY (whole_section, sp->buffer_, size + 1);
        } else {
                /* Parser is created with path and file information */
                do {
                        read =
                            fread (buff, sizeof (TSChar), tmpsize, sp->file_);

                        if (sp->is_unicode_ == EFileUnicode) {

                                /* Not supported yet!! */

                        } else {

                                /* This should be: 8bit to 16bit */
                                STRCPY (current_section, buff,
                                        strlen (buff) + 1);

                                /* Appends current section to whole section */
                                STRCPY (&whole_section
                                        [strlen (whole_section) + 1]
                                        , current_section,
                                        strlen (current_section) + 1);
                        }

                        offset += tmpsize;

                } while (offset < size);
        }

        DELETE (buff);
        DELETE (current_section);

        /* User wants without c-style comments */
        if (sp->comment_type_ == ECStyleComments) {
                mp_parse_comments_off (whole_section);
        }

        if (sp->offset_ > (size - 1)) {
                errno = EFAULT;
                goto EXIT;
        }
        buff = &whole_section[sp->offset_];

        /* Check if start tag was given */
        if (strlen (start_tag) == 0) {

                /* skip line break, tabs, spaces, etc */
                while (strchr (" \n\t\r", *buff) != NULL) {
                        buff++;
                }
                tag_count++;
                start_pos = buff;
        } else {
                c = buff;
                c2 = c;

                /* We are looking for occurence of start_tag */
                c2 = strstr (c, start_tag);
                while (c2 != NULL) {
                        tag_count++;
                        if (tag_count == seeked) {
                                /* start tag found of correct one */
                                start_pos = c2;
                                start_pos += strlen (start_tag);

                                /* skip line break, etc... */
                                while (strchr (" \n\t\r", *start_pos) != NULL) {
                                        start_pos++;
                                }

                                break;
                        } else {
                                /* start tag found but not correct section */
                                /* tag_count++; */
                        }
                        c = c2 + 1;
                        c2 = strstr (c, start_tag);
                }
        }

        /* Seeked section is not found */
        if (tag_count != seeked) {
                goto EXIT;
        }

        /* Check if end tag was given */
        if (strlen (end_tag) == 0) {
                end_pos = &whole_section[strlen (whole_section) - 1];
        } else {
                c = start_pos;
                c2 = c;

                /* We are looking for occurence of end_tag */
                c2 = strstr (c, end_tag);

                if (c2 != NULL) {
                        end_pos = c2;
                } else {
                        /* end_tag was not found => length will be 0 */
                        end_pos = start_pos;
                }
        }

        /* The length includes spaces and end of lines */
        length = strlen (start_pos) - strlen (end_pos);

        if (length <= 0) {
                goto EXIT;
        } else {
                /* Create MinSectionParser */
                section = mmp_create (length);
                mmp_set_data (section, whole_section, start_pos, length);
        }
      EXIT:
        return section;
}

/* ------------------------------------------------------------------------- */
void mp_parse_comments_off (TSChar * buff)
{
        TSChar         *start_pos = INITPTR;
        TSChar         *end_pos = INITPTR;
        TSChar         *c = INITPTR;
        TSChar         *tmp = INITPTR;
        int             length = 0;
        TSearchType     search_type = ENormalSearch;

        if (buff == INITPTR || strlen (buff) == 0)
                return;

        c = buff;

        /* Remove comments */
        do {
                switch (search_type) {
                case ENormalSearch:
                        if (*c == '/') {
                                /* Peek next TSCharacter */
                                if (*(c + 1) == '/') {
                                        start_pos = c;
                                        c++;
                                        search_type = ECStyleSlash;
                                } else if (*(c + 1) == '*') {
                                        start_pos = c;
                                        c++;
                                        search_type = ECStyleSlashAndAsterix;
                                }
                        }
                        c++;
                        break;

                case ECStyleSlash:
                        /* Peek next TSCharacter(10 or '\n' in UNIX style ) */
                        if (*(c + 1) == 0x0A) {
                                /* Don't remove line break!!
                                 * ( Else this fails:
                                 * 1st line:"this is parsed text 1"
                                 * 2nd line:"this is parsed text 2 
                                 *           // this is comments"
                                 * 1st and 2nd lines will be together 
                                 * and following
                                 *  operations may fail) 
                                 */
                                end_pos = c + 1;
                                search_type = EDoRemove;
                                break;
                        }

                        /* Peek next TSCharacter(13 or '\r' in Symbian OS) */
                        if (*(c + 1) == 0x0D) {
                                /* Increment position */
                                c++;
                                if (*(c + 1) == 0x0A) {
                                        /* Don't remove line break!!
                                         * ( Else this fails:
                                         * 1st line:"this is parsed text 1"
                                         * 2nd line:"this is parsed text 2 
                                         *           // this is comments"
                                         * 1st and 2nd lines will be together 
                                         * and followingoperations may fail) 
                                         */
                                        end_pos = c;
                                        search_type = EDoRemove;
                                        break;
                                }
                                /* 0x0A not found, decrement position */
                                c--;
                        }

                        /* Increment the lex position */
                        c++;
                        /* Take current end position */
                        end_pos = c;
                        break;

                case ECStyleSlashAndAsterix:

                        if (*(c + 1) == '*') {
                                c++;
                                if (*(c + 1) == '/') {
                                        c++;
                                        end_pos = c + 1;
                                        search_type = EDoRemove;
                                        break;
                                }
                                c--;
                        }
                        c++;
                        end_pos = c + 1;
                        break;

                default:
                        search_type = ENormalSearch;
                        break;
                }               /* switch */

                /* Remove comment */
                if (search_type == EDoRemove) {
                        length = strlen (end_pos);
                        tmp = NEW2 (TSChar, length + 2);
                        STRCPY (tmp, end_pos, length + 1);

                        STRCPY (start_pos, tmp, strlen (tmp) + 1);

                        DELETE (tmp);
                        tmp = INITPTR;
                        length = 0;

                        c = start_pos;
                        search_type = ENormalSearch;
                }
        } while (*c != '\0');

        /* If comment is started and configure file ends to eof we remove
           comments althougt there are no end of line or '*\/' TSCharacters */
        if (search_type == ECStyleSlash
            || search_type == ECStyleSlashAndAsterix) {

                length = strlen (end_pos);
                tmp = NEW2 (TSChar, length + 2);
                STRCPY (tmp, end_pos, length + 1);

                STRCPY (start_pos, tmp, length + 1);

                DELETE (tmp);
                tmp = INITPTR;
                length = 0;
        }

        mp_handle_special_marks (buff);
}

/* ------------------------------------------------------------------------- */
void mp_handle_special_marks (TSChar * buff)
{
        TSChar         *firstpos = INITPTR;
        TSChar         *secondpos = INITPTR;
        TSChar         *c = INITPTR;

        if (buff == INITPTR || strlen (buff) == 0)
                return;

        c = &buff[0];

        do {
                firstpos = c + 1;
                if (*c == '\\') {

                        if (*(c + 1) == '/') {
                                c++;
                                secondpos = c + 1;

                                if (*(c + 1) == '\\') {
                                        c++;
                                        if (*(c + 1) == '/'
                                            || *(c + 1) == '*') {
                                                STRCPY (secondpos,
                                                        secondpos + 1,
                                                        strlen (secondpos +
                                                                1));
                                                STRCPY (firstpos,
                                                        firstpos + 1,
                                                        strlen (firstpos +
                                                                1));
                                        }
                                }

                        } else if (*(c + 1) == '*') {
                                c++;
                                secondpos = c + 1;

                                if (*(c + 1) == '\\') {
                                        c++;
                                        if (*(c + 1) == '/') {
                                                STRCPY (secondpos,
                                                        secondpos + 1,
                                                        strlen (secondpos +
                                                                1));
                                                STRCPY (firstpos,
                                                        firstpos + 1,
                                                        strlen (firstpos +
                                                                1));
                                        }
                                }
                        }
                }
                firstpos = INITPTR;
                secondpos = INITPTR;
                c++;
        } while (*c != '\0');
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
MinParser     *mp_create (const TSChar * path, const TSChar * file,
                           TCommentType comments)
{
        MinParser     *tmp = INITPTR;
        int             plen = 0;
        int             flen = 0;
        TSChar         *filename = INITPTR;
        TSChar         *c = INITPTR;

        if (path == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }
        if (file == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }

        plen = strlen (path);
        flen = strlen (file);

        if (plen == 0) {
                errno = EINVAL;
                goto EXIT;
        }
        if (flen == 0) {
                errno = EINVAL;
                goto EXIT;
        }

        tmp = NEW (MinParser);
        filename = NEW2 (TSChar, plen + flen + 2);
        c = &filename[plen];

        /* Initial values */
        tmp->file_ = INITPTR;
        tmp->offset_ = 0;
        tmp->comment_type_ = comments;
        tmp->parsing_mode_ = EFileParsing;
        tmp->buffer_tmp_ = INITPTR;
        tmp->buffer_ = INITPTR;
        tmp->is_unicode_ = 0;
        tmp->file_parser_ = INITPTR;

        /* Concatenate path and file and make sure that slash will be present */
        STRCPY (filename, path, plen);
        if (*(c - 1) != '/') {
                *c = '/';
                c++;
        }
        STRCPY (c, file, flen + 1);
        c[flen] = '\0';

        /* Open file */
        tmp->file_ = fopen (filename, "r");
        if (tmp->file_ == NULL) {
                DELETE (tmp);
                tmp = INITPTR;
                goto EXIT;
        }

        /* Create Min File Parser */
        tmp->file_parser_ =
            mfp_create (tmp->file_, EFileNotUnicode, tmp->comment_type_);
      EXIT:
        if (filename != INITPTR)
                DELETE (filename);
        return tmp;
}

/* ------------------------------------------------------------------------- */
MinParser     *mp_create_mem (const TSChar * buffer, TCommentType comments)
{
        MinParser     *tmp = INITPTR;

        if (buffer == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }
        if (strlen (buffer) == 0) {
                errno = EINVAL;
                goto EXIT;
        }

        tmp = NEW (MinParser);
        tmp->file_ = INITPTR;
        tmp->offset_ = 0;
        tmp->comment_type_ = comments;
        tmp->parsing_mode_ = EBufferParsing;
        tmp->buffer_tmp_ = INITPTR;
        tmp->buffer_ = INITPTR;
        tmp->is_unicode_ = EFileNotUnicode;
        tmp->file_parser_ = INITPTR;
        tmp->buffer_tmp_ = NEW2 (TSChar, strlen (buffer) + 1);
        tmp->buffer_ = &tmp->buffer_tmp_[0];
        STRCPY (tmp->buffer_, buffer, strlen (buffer) + 1);
      EXIT:
        return tmp;
}

/* ------------------------------------------------------------------------- */
void mp_destroy (MinParser ** sp)
{
        if (*sp == INITPTR)
                return;

        mfp_destroy (&((*sp)->file_parser_));

        if ((*sp)->parsing_mode_ == EFileParsing)
                fclose ((*sp)->file_);

        DELETE (*sp);
        *sp = INITPTR;
}

/* ------------------------------------------------------------------------- */
MinSectionParser *mp_section (MinParser * sp, const TSChar * start_tag,
                               const TSChar * end_tag, int seeked)
{
        MinSectionParser *msp = INITPTR;

        if (sp == INITPTR) {
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

        sp->offset_ = 0;
        msp = mp_next_section (sp, start_tag, end_tag, seeked);

      EXIT:
        return msp;
}

/* ------------------------------------------------------------------------- */
MinSectionParser *mp_next_section (MinParser * sp, const TSChar * start_tag,
                                    const TSChar * end_tag, int seeked)
{
        MinSectionParser *msp = INITPTR;

        if (sp == INITPTR) {
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

        /* If parsing mode is set to file, we parse directly in the file */
        if (sp->parsing_mode_ == EFileParsing) {
                msp = mp_next_section_file (sp, start_tag, end_tag, seeked);
        } else {
                /* If parsing mode is set to buffer, process in old way */
                msp = mp_next_section_memory (sp, start_tag, end_tag, seeked);
        }

      EXIT:
        return msp;
}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "min_parser.tests"
#endif                          /* MIN_UNIT_TEST */
/* ------------------------------------------------------------------------- */
/* End of file */
