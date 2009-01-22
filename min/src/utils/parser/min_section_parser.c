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
 *  @file       min_section_parser.c
 *  @version    0.1
 *  @brief      This file contains implementation of Min Section Parser.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include "min_section_parser.h"

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
/** Generic start and end position parser for given data.
 *  @param msp [in] MinSectionParser entity to operate on.
 *  @param section [in] buffer to be parsed.
 *  @param start_tag [in] tag from which parsing starts.
 *  @param tag_indicator [in] indicates if tag is included in return value.
 *  @param start_pos [out] indicates starting position of extracted section.
 *  @param end_pos [out] indicates ending position of extracted section.
 *  @param length [out] indicates the length of the extracted section.
 *  @return 0 or -1 in case of failure.
 */
LOCAL int       mmp_parse_start_and_end_pos (MinSectionParser * msp,
                                             const TSChar * section,
                                             const TSChar * start_tag,
                                             TSTagToReturnValue tag_indicator,
                                             TSChar ** start_pos,
                                             TSChar ** end_pos, int *length);
/* ------------------------------------------------------------------------- */
/** Generic search for end-of-line.
 *  @param msp [in] MinSectionParser entity to operate on.
 *  @param c [in:out] indicates starting position when looking for EOL, is set
 *         to the beginning of the next line when linefeed is found.
 *  @return position before linefeed or INITPTR in case of failure.
 *
 */
LOCAL TSChar   *mmp_goto_end_of_line (MinSectionParser * msp, TSChar ** c);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
LOCAL int mmp_parse_start_and_end_pos (MinSectionParser * msp,
                                       const TSChar * section,
                                       const TSChar * start_tag,
                                       TSTagToReturnValue tag_indicator,
                                       TSChar ** start_pos, 
                                       TSChar ** end_pos,
                                       int *length)
{
        int             retval = ENOERR;
        int             i = 0;
        TSChar         *c = INITPTR;
        TSChar         *c2 = INITPTR;

        *start_pos = INITPTR;
        *end_pos = INITPTR;
        *length = 0;

        if (msp == INITPTR) {
                retval = -1;
                errno = EINVAL;
                goto EXIT;
        }
        if (section == INITPTR) {
                retval = -1;
                errno = EINVAL;
                goto EXIT;
        }
        if (start_tag == INITPTR) {
                retval = -1;
                errno = EINVAL;
                goto EXIT;
        }

        if (msp->skip_and_mark_pos_ < strlen (section)) {
                c = (TSChar *) & section[msp->skip_and_mark_pos_];
        } else {
                c = (TSChar *) & section[strlen (section) - 1];
        }

        /* Check if start_tag is given */
        if (strlen (start_tag) == 0) {
                while (strchr (" \n\t\r", *c) != NULL) {
                        if (*c == '\0') {
                                break;
                        }
                        c++;
                }
                *start_pos = c;
                c2 = *start_pos;

                if (*c == '\0') {
                        *length = 1;
                        *end_pos = *start_pos;
                        goto EXIT;
                }

        } else {
                /* We are looking for occurence of start_tag */
                c2 = strstr (c, start_tag);

                if (c2 != NULL) {

                        /* tag will not be included to the section */
                        if (tag_indicator != ESTag) {
                                /* because start_pos is before start_tag */
                                c2 += strlen (start_tag);

                                while (strchr (" \t\n", *c2) != NULL) {
                                        c2++;
                                }
                        }

                        /* start position */
                        *start_pos = c2;
                }
        }

        if (*start_pos == INITPTR) {
                goto EXIT;
        }
        if (**start_pos == '\0') {
                goto EXIT;
        }

        /* Look for the end_pos */
        *end_pos = mmp_goto_end_of_line (msp, &c2);

        if (*start_pos == *end_pos) {
                goto EXIT;
        }

        for (i = 0; i < strlen (section); i++) {
                if (*start_pos == &section[i])
                        break;
        }
        msp->skip_and_mark_pos_ = i;

        /* Count the length */
        c2 = *start_pos;
        c = *end_pos;
        while (c2 != c) {
                c2++;
                (*length)++;
        }

        (*length) += 1;

        /* Update next line beginning */
        msp->skip_and_mark_pos_ += *length;
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
LOCAL TSChar   *mmp_goto_end_of_line (MinSectionParser * msp, TSChar ** c)
{
        TSChar         *retval = INITPTR;
        TSChar         *last_item_pos = *c;
        TSChar         *c2 = *c;

        if (msp == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }

        do {
                /* Peek next TSCharacter \n (0x0A) in UNIX */
                if (*(c2 + 1) == 0x0A) {
                        c2++;
                        break;
                }
                /* Peek next TSCharacter \r (0x0D) in Symbian OS */
                if (*(c2 + 1) == 0x0D) {
                        c2++;

                        /* Peek next TSCharacter \n (0x0A) in Symbian OS */
                        if (*(c2 + 1) == 0x0A) {
                                c2++;
                                break;
                        }

                        c2--;
                } else if (*(c2 + 1) == 0x09 || *(c2 + 1) == 0x20) {
                        c2++;
                        continue;
                }

                c2++;
                last_item_pos = c2;

        } while (*c2 != '\0');

        retval = last_item_pos;
        *c = c2 + 1;
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
MinSectionParser *mmp_create (unsigned int length)
{
        MinSectionParser *msp = NEW (MinSectionParser);

        if (msp == NULL) {
                msp = INITPTR;
                errno = ENOMEM;
                goto EXIT;
        }

        msp->section_ = INITPTR;
        msp->length_ = length;
        msp->buffer_section_ = INITPTR;
        msp->skip_and_mark_pos_ = 0;
        msp->line_indicator_ = ESFalse;
        msp->sub_offset_ = INITPTR;

        msp->buffer_section_ = NEW2 (TSChar, msp->length_ + 1);

        if (msp->buffer_section_ == NULL) {
                msp->buffer_section_ = INITPTR;
                DELETE (msp);
                goto EXIT;
        }

        msp->buffer_section_[0] = '\0';
        msp->buffer_section_[length] = '\0';
        msp->section_ = msp->buffer_section_;
        msp->sub_offset_ = msp->buffer_section_;
      EXIT:
        return msp;
}

/* ------------------------------------------------------------------------- */
void mmp_destroy (MinSectionParser ** msp)
{
        if (*msp == INITPTR)
                return;

        DELETE ((*msp)->buffer_section_);
        DELETE (*msp);
        *msp = INITPTR;
}

/* ------------------------------------------------------------------------- */
MinItemParser *mmp_get_item_line (MinSectionParser * msp,
                                   const TSChar * tag,
                                   TSTagToReturnValue tag_indicator)
{
        MinItemParser *mip = INITPTR;
        int             ret = 0;
        TSChar         *start_pos = INITPTR;
        TSChar         *end_pos = INITPTR;
        int             length = 0;
        TSChar         *c = INITPTR;

        if (msp == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }
        if (tag == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }

        /* Indicator that GetItemLineL has been used */
        msp->line_indicator_ = ESTrue;

        msp->skip_and_mark_pos_ = 0;

        ret =
            mmp_parse_start_and_end_pos (msp, msp->section_, tag,
                                         tag_indicator, &start_pos, &end_pos,
                                         &length);
        /* No parsing found */
        if ((ret != ENOERR) || (length <= 0) || (start_pos == INITPTR)) {
                goto EXIT;
        }

        ret = 0;
        c = &msp->section_[0];
        while (c != start_pos) {
                c++;
                ret++;
        }

        mip = mip_create (msp->section_, ret, length);

      EXIT:
        return mip;
}

/* ------------------------------------------------------------------------- */
MinItemParser *mmp_get_next_item_line (MinSectionParser * msp)
{
        MinItemParser *mip = INITPTR;
        int             retval = 0;
        TSChar         *start_pos = INITPTR;
        TSChar         *end_pos = INITPTR;
        TSChar         *c = INITPTR;
        int             length = 0;
        TSTagToReturnValue tag_indicator = ESTag;

        if (msp == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }
        if (msp->line_indicator_ == ESFalse) {
                goto EXIT;
        }

        retval =
            mmp_parse_start_and_end_pos (msp, msp->section_, "",
                                         tag_indicator, &start_pos, &end_pos,
                                         &length);
        /* No parsing found */
        if (retval != ENOERR || length <= 0 || start_pos == INITPTR)
                goto EXIT;

        retval = 0;
        c = &msp->section_[0];
        while (c != start_pos) {
                c++;
                retval++;
        }

        mip = mip_create (msp->section_, retval, length);
      EXIT:
        return mip;
}

/* ------------------------------------------------------------------------- */
MinItemParser *mmp_get_next_tagged_item_line (MinSectionParser * msp,
                                               const TSChar * tag,
                                               TSTagToReturnValue
                                               tag_indicator)
{
        MinItemParser *mip = INITPTR;
        int             retval = 0;
        TSChar         *start_pos = INITPTR;
        TSChar         *end_pos = INITPTR;
        TSChar         *c = INITPTR;
        int             length = 0;

        if (msp == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }
        if (tag == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }

        /* mmp_get_line or mmp_get_item_line has not been called */
        if (msp->line_indicator_ == ESFalse) {
                goto EXIT;
        }

        retval =
            mmp_parse_start_and_end_pos (msp, msp->section_, tag,
                                         tag_indicator, &start_pos, &end_pos,
                                         &length);
        /* No parsing found */
        if (retval != ENOERR || length <= 0 || start_pos == INITPTR) {
                goto EXIT;
        }

        retval = 0;
        c = &msp->section_[0];
        while (c != start_pos) {
                c++;
                retval++;
        }

        mip = mip_create (msp->section_, retval, length);
      EXIT:
        return mip;
}

/* ------------------------------------------------------------------------- */
MinSectionParser *mmp_sub_section (MinSectionParser * msp,
                                    const TSChar * start_tag,
                                    const TSChar * end_tag, int seeked)
{
        MinSectionParser *sp = INITPTR;

        if (msp == INITPTR) {
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

        msp->sub_offset_ = &msp->section_[0];
        sp = mmp_next_sub_section (msp, start_tag, end_tag, seeked);
      EXIT:
        return sp;

}

/* ------------------------------------------------------------------------- */
MinSectionParser *mmp_next_sub_section (MinSectionParser * msp,
                                         const TSChar * start_tag,
                                         const TSChar * end_tag, int seeked)
{
        MinSectionParser *sp = INITPTR;
        TSChar         *buff = INITPTR;
        TSChar         *c = INITPTR;
        TSChar         *c2 = INITPTR;
        TSChar         *length_startpos = INITPTR;
        TSChar         *length_endpos = INITPTR;
        unsigned int    tag_count = 1;
        unsigned int    length = 0;

        if (msp == INITPTR) {
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

        buff = &msp->section_[0];

        /* Check if start_tag is given */
        if (strlen (start_tag) == 0) {
                /* skip spaces, line break, tabs, etc */
                while (strchr (" \n\t\r", *buff) != NULL) {
                        buff++;
                }
                length_startpos = buff;
        } else {
                c = buff;
                c2 = c;

                /* we are looking for occurence of start_tag */
                c2 = strstr (c, start_tag);

                while (c2 != NULL) {
                        if (tag_count == seeked) {
                                /* start tag found of correct one */
                                length_startpos = c2;
                                length_startpos += strlen (start_tag);
                                break;
                        } else {
                                /* start tag found but not correct section */
                                tag_count++;
                        }
                        c = c2 + 1;
                        c2 = strstr (c, start_tag);
                }
        }

        /* Seeked section is not found */
        if (tag_count != seeked || c2 == NULL) {
                goto EXIT;
        }

        while (strchr (" \n\t\r", *length_startpos) != NULL) {
                length_startpos++;
        }

        /* Check if end_tag is given */
        if (strlen (end_tag) == 0) {
                length_endpos = &msp->section_[strlen (msp->section_) - 1];
        } else {
                c = length_startpos;
                c2 = c;

                /* We are looking for occurence of end_tag */
                c2 = strstr (c, end_tag);

                if (c2 != NULL) {
                        length_endpos = c2;
                        length_endpos--;
                } else {
                        /* end_tag was not found => length will be 0 */
                        length_endpos = length_startpos;
                }
        }

        while (strchr (" \n\t\r", *length_endpos) != NULL) {
                length_endpos--;
        }

        /* The length includes spaces and end of lines */
        length = strlen (length_startpos) - strlen (length_endpos);

        if (length <= 0) {
                goto EXIT;
        } else {
                /* Create MinSectionParser */
                sp = mmp_create (length + 2);
                mmp_set_data (sp, msp->section_, length_startpos, length + 1);
        }
      EXIT:
        return sp;
}

/* ------------------------------------------------------------------------- */
int mmp_get_line (MinSectionParser * msp, const TSChar * tag, TSChar ** line,
                  TSTagToReturnValue tag_indicator)
{
        int             retval = ENOERR;
        int             length = 0;
        TSChar         *start_pos = INITPTR;
        TSChar         *end_pos = INITPTR;

        *line = INITPTR;

        if (msp == INITPTR) {
                retval = -1;
                errno = EINVAL;
                goto EXIT;
        }
        if (tag == INITPTR) {
                retval = -1;
                errno = EINVAL;
                goto EXIT;
        }

        if (strlen (msp->section_) == 0) {
                retval = -1;
                goto EXIT;
        }

        msp->line_indicator_ = ESTrue;
        msp->skip_and_mark_pos_ = 0;

        retval =
            mmp_parse_start_and_end_pos (msp, msp->section_, tag,
                                         tag_indicator, &start_pos, &end_pos,
                                         &length);

        /* No parsing found */
        if (retval != ENOERR || length <= 0 || start_pos == INITPTR) {
                retval = -1;
                goto EXIT;
        }

        /* we can return found line */
        *line = NEW2 (TSChar, length + 1);
        STRCPY (*line, start_pos, length);
        (*line)[length] = '\0';
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
int mmp_get_next_line (MinSectionParser * msp, TSChar ** line)
{
        int             retval = ENOERR;
        int             length = 0;
        TSChar         *start_pos = INITPTR;
        TSChar         *end_pos = INITPTR;
        TSTagToReturnValue tag_indicator;

        *line = INITPTR;

        if (msp == INITPTR) {
                retval = -1;
                errno = EINVAL;
                goto EXIT;
        }

        if (msp->line_indicator_ == ESFalse) {
                retval = -1;
                goto EXIT;
        }

        /* tag_indicator has no meaning */
        tag_indicator = ESTag;

        retval =
            mmp_parse_start_and_end_pos (msp, msp->section_, "",
                                         tag_indicator, &start_pos, &end_pos,
                                         &length);
        /* No parsing found */
        if (retval != ENOERR || length <= 0 || start_pos == INITPTR) {
                goto EXIT;
        }

        /* we can return found line */
        *line = NEW2 (TSChar, length + 1);
        STRCPY (*line, start_pos, length);
        (*line)[length] = '\0';
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
int mmp_get_next_tagged_line (MinSectionParser * msp, const TSChar * tag,
                              TSChar ** line,
                              TSTagToReturnValue tag_indicator)
{
        int             retval = ENOERR;
        int             length = 0;
        TSChar         *start_pos = INITPTR;
        TSChar         *end_pos = INITPTR;

        *line = INITPTR;

        if (msp == INITPTR) {
                retval = -1;
                errno = EINVAL;
                goto EXIT;
        }
        if (tag == INITPTR) {
                retval = -1;
                errno = EINVAL;
                goto EXIT;
        }

        if (msp->line_indicator_ == ESFalse) {
                retval = -1;
                goto EXIT;
        }

        retval =
            mmp_parse_start_and_end_pos (msp, msp->section_, tag,
                                         tag_indicator, &start_pos, &end_pos,
                                         &length);
        /* No parsing found */
        if (retval != ENOERR || length <= 0 || start_pos == INITPTR) {
                goto EXIT;
        }

        /* we can return found line */
        *line = NEW2 (TSChar, length + 1);
        if (start_pos != INITPTR) {
                STRCPY (*line, start_pos, length + 1);
                (*line)[length] = '\0';
        } else {
                (*line)[0] = '\0';
        }

      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
int mmp_get_position (MinSectionParser * msp)
{
        unsigned int    retval = 0;
        if (msp == INITPTR) {
                retval = -1;
                errno = EINVAL;
                goto EXIT;
        }
        retval = msp->skip_and_mark_pos_;
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
int mmp_set_position (MinSectionParser * msp, unsigned int pos)
{
        unsigned int    retval = ENOERR;
        if (msp == INITPTR) {
                retval = -1;
                errno = EINVAL;
                goto EXIT;
        }
        msp->skip_and_mark_pos_ = pos;
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
void mmp_set_data (MinSectionParser * msp, const TSChar * data,
                   TSChar * start_pos, unsigned int length)
{
        if (msp == INITPTR) {
                goto EXIT;
        }
        if (data == INITPTR) {
                goto EXIT;
        }
        if (start_pos == INITPTR) {
                goto EXIT;
        }
        if (msp->section_ == INITPTR) {
                goto EXIT;
        }

        STRCPY (msp->section_, start_pos, length);
        if (length < msp->length_)
                msp->section_[length] = '\0';
        else
                msp->section_[msp->length_] = '\0';

      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
const TSChar   *mmp_des (const MinSectionParser * msp)
{
        if (msp == INITPTR)
                return INITPTR;
        else
                return msp->section_;
}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */


/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "min_section_parser.tests"
#endif                          /* MIN_UNIT_TEST */
/* ------------------------------------------------------------------------- */
/* End of file */
