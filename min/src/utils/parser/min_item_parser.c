/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Marko Hyyppä
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
 *  @file       min_item_parser.c
 *  @version    0.1
 *  @brief      This file contains implementation of MIN Item Parser
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "min_common.h"
#include "min_item_parser.h"

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
/* None */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* None */

/* ======================== FUNCTIONS ====================================== */
/** Creates MIN Item Parser data structures with given string data.
 *  @param section Pointer to section string.
 *  @param start_pos Start position with integer value in given section string.
 *  @param length Length integer value of given section string.
 *  @return Pointer to created MIN Item Parser data structure.
 *          If error result then pointer has got INITPTR value.
 *
 *  Possible errors:
 *  - ENOMEM not enough memory to create data structure.
 */
MinItemParser *mip_create (TSChar * section, int start_pos, int length)
{
        MinItemParser *mip = INITPTR;

        if ((section != INITPTR) && (section != NULL) &&
            (start_pos >= 0) && (start_pos < strlen (section)) &&
            (length > 0)) {

                mip = NEW (MinItemParser);

                if (mip != NULL) {
                        mip->item_line_section_ = NEW2 (TSChar, length + 1);

                        if (mip->item_line_section_) {
                                STRCPY (mip->item_line_section_,
                                        (section + start_pos), length);
                                mip->item_line_section_[length] = '\0';
                                mip->item_skip_and_mark_pos_ = INITPTR;
                                mip->get_methods_indicator_ = ESFalse;
                                mip->parsing_type_ = ENormalParsing;
                        } else {
                                /* SIP creation failed, 
                                 * free SIP memory allocation */
                                DELETE (mip);
                                mip = INITPTR;
                                errno = ENOMEM;
                        }
                } else {
                        /* SIP creation failed, returns INITPTR */
                        mip = INITPTR;
                        errno = ENOMEM;
                }
        }

        return mip;
}

/* ------------------------------------------------------------------------- */
/** Frees allocated memory of MIN Item Parser data structure.
 *  @param mip Pointer to existing MIN Item Parser data structure.
 */
void mip_destroy (MinItemParser ** mip)
{
        if (*mip != INITPTR) {
                if ((*mip)->item_line_section_ != INITPTR) {
                        DELETE ((*mip)->item_line_section_);
                }
                DELETE (*mip);
                *mip = INITPTR;
        }
}

/* ------------------------------------------------------------------------- */
/** Parses MIN Item Parser section for start, end and extra end positions 
 *  including length value of parsed part of section.
 *  @param mip Pointer to existing MIN Item Parser data structure.
 *  @param start_tag Pointer to Start Tag string which be used with parsing.
 *  @param ref_start_pos Start Position (char*) of parsed result string.
 *  @param ref_end_pos End Position (char*) of parsed result string.
 *  @param ref_length Length value of parsed result string.
 *  @param ref_extra_end_pos Extra End Position (char*) of parsed result
 *                           string. Used when be found quated strings.
 *  @return Returns result value. If parsing completed successfully then
 *          value 0, else returns error code.
 *
 *  Possible errors:
 *  - EINVAL and error code -1 when parsing operation failed.
 */
int mip_parse_start_and_end_pos (MinItemParser * mip, TSChar * start_tag,
                                 TSChar ** ref_start_pos,
                                 TSChar ** ref_end_pos, int *ref_length,
                                 TSChar ** ref_extra_end_pos)
{
        int             retval = 0;
        TSChar         *start_pos = NULL;
        TSChar         *end_pos = INITPTR;
        TSChar         *extra_end_pos = INITPTR; /* Normally this is INITPTR */
        TSChar          search_type = ENormalSearch;
        TSChar         *section = INITPTR;
        TSChar         *check_end = INITPTR;
        int             length = 0;
        TSBool          loop = ESFalse;

        if (mip == INITPTR || mip == NULL)
                goto EXIT;

        if ((mip->get_methods_indicator_ == ESTrue) &&
            (mip->item_skip_and_mark_pos_ != INITPTR)) {
                section = mip->item_skip_and_mark_pos_;
        } else {
                section = mip->item_line_section_;
        }
        if (start_tag != INITPTR) {
                if (strlen (start_tag) > 0) {
                        /* Find string after given start tag */
                        section = strstr (section, start_tag);
                        if (section == NULL)
                                goto EXIT;
                        start_pos = strchr (section, '=');
                        if (start_pos == NULL)
                                start_pos = strchr (section, ' ');
                }
        }

        if (start_pos != NULL) {
                start_pos++;
        } else {
                start_pos = section;
        }

	if (start_pos == NULL)
		return -1;

        while (*start_pos == ' ' && *start_pos != '\0') {
                start_pos++;
        }
	
        /*
         * Quotes have special meaning and information between quotes is 
         * handled as a one string. Quotes not included to information. 
         * Mode OFF.
         */

        if (strncmp (start_pos, "\\\\", 2) == 0) {
                search_type = ENormalSearch;
                start_pos += 2;
                while (*start_pos == ' ')
                        start_pos++;
                if (*start_pos == '\n' || *start_pos == '\0') {
                        start_pos = INITPTR;
                        goto EXIT;
                }
        } else if ((mip->parsing_type_ == EQuoteStyleParsing) &&
                   (strncmp (start_pos, "\"", 1) == 0)) {
                search_type = EQuoteStartSearch;
                start_pos += 1;
        } else {
                search_type = ENormalSearch;
        }

        check_end = start_pos;
        length = 0;
        loop = ESTrue;

        while (loop) {
                if (((search_type == EQuoteStartSearch)
                     && (*check_end == '"'))
                    || ((search_type != EQuoteStartSearch)
                        && (*check_end == ' ')) || (*check_end == '\r')
                    || (*check_end == '\n') || (*check_end == '\0')) {
                        loop = ESFalse;
                } else {
                        check_end++;
                        length++;
                }
        }

        if (*check_end == '\0') {
                extra_end_pos = INITPTR;
                end_pos = check_end;
        } else if (*check_end == '"') {
                end_pos = check_end - 1;
                check_end = check_end + 1;
                if (*check_end != '\r' &&
                    *check_end != '\n' && *check_end != '\0') {
                        extra_end_pos = check_end;
                }
        } else {
                extra_end_pos = INITPTR;
                end_pos = check_end + 1;
        }

        /* Quotes style parsing is in use. 
         * Special handling when empty quotes "" */
        if ((mip->parsing_type_ == EQuoteStyleParsing) &&
            (length <= 0) &&
            (search_type == EQuoteStartSearch)) {
                /* Set new positions */
                start_pos--;
                end_pos--;
                extra_end_pos = INITPTR;
                length = end_pos - start_pos;
        }

      EXIT:
        /* Start position is NULL or length is negative */
        if ((start_pos == NULL) || (length <= 0)) {
                start_pos = INITPTR;
        }

        if (start_pos == INITPTR) {
                end_pos = INITPTR;
                extra_end_pos = INITPTR;
                length = 0;
                /* Parsing failed, return error code */
                errno = EINVAL;
                retval = -1;
        }

        *ref_start_pos = start_pos;
        *ref_end_pos = end_pos;
        *ref_extra_end_pos = extra_end_pos;
        *ref_length = length;

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Gets first string according to tag from MIN Item Parser section string.
 *  @param mip Pointer to existing MIN Item Parser data structure.
 *  @param tag Pointer to tag string for searching section part string.
 *  @param string String reference for found first section part string.
 *  @return Returns error code 0 if operation completes successfully,
 *          else returns error code.
 *
 *  Possible errors:
 *  - EINVAL and error code -1 if searching operation failed.
 */
int mip_get_string (MinItemParser * mip, TSChar * tag, TSChar ** string)
{
        TSChar         *start_pos = INITPTR;
        TSChar         *end_pos = INITPTR;
        TSChar         *extra_end_pos = INITPTR;
        int             length = 0;
        int             retval;

        errno = EINVAL;
        retval = -1;

        if (mip != INITPTR) {
                if (strlen (mip->item_line_section_) > 0) {
                        mip->item_skip_and_mark_pos_ = INITPTR;
                        retval =
                            mip_parse_start_and_end_pos (mip, tag, &start_pos,
                                                         &end_pos, &length,
                                                         &extra_end_pos);
                        if (retval == 0) {
                                *string = NEW2 (TSChar, length + 1);
                                if (*string) {
                                        STRCPY (*string, start_pos, length);
                                        (*string)[length] = '\0';

                                        if (extra_end_pos == INITPTR) {
                                                mip->item_skip_and_mark_pos_ =
                                                    end_pos;
                                        } else {
                                                mip->item_skip_and_mark_pos_ =
                                                    extra_end_pos;
                                        }

                                        mip->get_methods_indicator_ = ESTrue;
                                        /* Operation completed, no errors */
                                        errno = 0;
                                        retval = 0;
                                }
                        }
                }
        }

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Gets next string according to previous section part string from
 *  MIN Item Parser section string.
 *  @param mip Pointer to existing MIN Item Parser data structure.
 *  @param string String reference for found next section part string.
 *  @return Returns error code 0 if operation completes successfully,
 *          else returns error code.
 *
 *  Possible errors:
 *  - EINVAL and error code -1 if searching operation failed.
 */
int mip_get_next_string (MinItemParser * mip, TSChar ** string)
{
        TSChar         *start_pos = INITPTR;
        TSChar         *end_pos = INITPTR;
        TSChar         *extra_end_pos = INITPTR;
        int             length = 0;
        int             retval;

        errno = EINVAL;
        retval = -1;

        if (mip != INITPTR) {
                if ((mip->get_methods_indicator_ == ESTrue) &&
                    (strlen (mip->item_line_section_) > 0)) {

                        retval = mip_parse_start_and_end_pos (mip, INITPTR,
                                                              &start_pos,
                                                              &end_pos,
                                                              &length,
                                                              &extra_end_pos);

                        if (retval == 0) {
                                if (extra_end_pos == INITPTR) {
                                        mip->item_skip_and_mark_pos_ =
                                            end_pos;
                                } else {
                                        mip->item_skip_and_mark_pos_ =
                                            extra_end_pos;
                                }

                                if ((*string != INITPTR) && (*string != NULL)) {
                                        DELETE (*string);
                                }

                                *string = NEW2 (TSChar, length + 1);
                                if (*string) {
                                        STRCPY (*string, start_pos, length);
                                        (*string)[length] = '\0';
                                        /* Operation completed, no errors */
                                        errno = 0;
                                        retval = 0;
                                }
                        }
                }
        }

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Gets next tagged string according to tag and earlier used 
 *  MIN Item Parser section string.
 *  @param mip Pointer to existing MIN Item Parser data structure.
 *  @param tag Pointer to tag string for searching next section part string.
 *  @param string String reference for found section part string.
 *  @return Returns error code 0 if operation completes successfully,
 *          else returns error code.
 *
 *  Possible errors:
 *  - EINVAL and error code -1 if searching operation failed.
 */
int mip_get_next_tagged_string (MinItemParser * mip, TSChar * tag,
                                TSChar ** string)
{
        TSChar         *start_pos = INITPTR;
        TSChar         *end_pos = INITPTR;
        TSChar         *extra_end_pos = INITPTR;
        int             length = 0;
        int             retval;

        errno = EINVAL;
        retval = -1;

        if (mip != INITPTR) {
                if ((mip->get_methods_indicator_ == ESTrue) &&
                    (strlen (mip->item_line_section_) > 0)) {

                        retval = mip_parse_start_and_end_pos (mip, tag,
                                                              &start_pos,
                                                              &end_pos,
                                                              &length,
                                                              &extra_end_pos);

                        if (retval == 0) {
                                mip->item_skip_and_mark_pos_ = end_pos;
                                *string = NEW2 (TSChar, length + 1);
                                if (*string) {
                                        STRCPY (*string, start_pos, length);
                                        (*string)[length] = '\0';
                                        /* Operation completed, no errors */
                                        errno = 0;
                                        retval = 0;
                                }
                        }
                }
        }

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Gets first integer value according to tag from MIN Item Parser
 *  section string.
 *  @param mip Pointer to existing MIN Item Parser data structure.
 *  @param tag Pointer to tag string for searching next section part string.
 *  @param value Pointer to integer variable for be found first integer
 *               section part.
 *  @return Returns error code 0 if operation completes successfully,
 *          else returns error code.
 *
 *  Possible errors:
 *  - EINVAL and error code -1 if searching operation failed.
 */
int mip_get_int (MinItemParser * mip, TSChar * tag, int *value)
{
        TSChar         *start_pos = INITPTR;
        TSChar         *end_pos = INITPTR;
        TSChar         *extra_end_pos = INITPTR;
        int             length = 0;
        int             retval;

        errno = EINVAL;
        retval = -1;

        if (mip != INITPTR) {
                if (strlen (mip->item_line_section_) > 0) {
                        retval = mip_parse_start_and_end_pos (mip, tag,
                                                              &start_pos,
                                                              &end_pos,
                                                              &length,
                                                              &extra_end_pos);

                        if (retval == 0) {
                                mip->item_skip_and_mark_pos_ = end_pos;
                                mip->get_methods_indicator_ = ESTrue;
                                *value = atoi (start_pos);
                                errno = 0;
                                retval = 0;
                        }
                }
        }

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Gets next integer value according to earlier used MIN Item Parser
 *  section string.
 *  @param mip Pointer to existing MIN Item Parser data structure.
 *  @param value Pointer to integer variable for be found next integer
 *               section part.
 *  @return Returns error code 0 if operation completes successfully,
 *          else returns error code.
 *
 *  Possible errors:
 *  - EINVAL and error code -1 if searching operation failed.
 */
int mip_get_next_int (MinItemParser * mip, int *value)
{
        TSChar         *start_pos = INITPTR;
        TSChar         *end_pos = INITPTR;
        TSChar         *extra_end_pos = INITPTR;
        int             length = 0;
        int             retval;

        errno = EINVAL;
        retval = -1;

        if (mip != INITPTR) {
                if (strlen (mip->item_line_section_) > 0) {
                        retval = mip_parse_start_and_end_pos (mip, INITPTR,
                                                              &start_pos,
                                                              &end_pos,
                                                              &length,
                                                              &extra_end_pos);

                        if (retval == 0) {
                                mip->item_skip_and_mark_pos_ = end_pos;
                                *value = atoi (start_pos);
                                errno = 0;
                                retval = 0;
                        }
                }
        }

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Gets next tagged integer value according to given tag and earlier used
 *  MIN Item Parser section string.
 *  @param mip Pointer to existing MIN Item Parser data structure.
 *  @param value Pointer to integer variable for be found next tagged 
 *               integer section part.
 *  @return Returns error code 0 if operation completes successfully,
 *          else returns error code.
 *
 *  Possible errors:
 *  - EINVAL and error code -1 if searching operation failed.
 */
int mip_get_next_tagged_int (MinItemParser * mip, TSChar * tag, int *value)
{
        TSChar         *start_pos = INITPTR;
        TSChar         *end_pos = INITPTR;
        TSChar         *extra_end_pos = INITPTR;
        int             length = 0;
        int             retval;

        errno = EINVAL;
        retval = -1;

        if (mip != INITPTR) {
                if (strlen (mip->item_line_section_) > 0) {
                        retval = mip_parse_start_and_end_pos (mip, tag,
                                                              &start_pos,
                                                              &end_pos,
                                                              &length,
                                                              &extra_end_pos);

                        if (retval == 0) {
                                mip->item_skip_and_mark_pos_ = end_pos;
                                *value = atoi (start_pos);
                                errno = 0;
                                retval = 0;
                        }
                }
        }

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Gets first unsigned integer value according to given tag from
 *  MIN Item Parser section string.
 *  @param mip Pointer to existing MIN Item Parser data structure.
 *  @param tag Pointer to tag string for searching unsigned integer value.
 *  @param value Pointer to unsigned integer variable for be found
 *               first unsigned integer section part.
 *  @return Returns error code 0 if operation completes successfully,
 *          else returns error code.
 *
 *  Possible errors:
 *  - EINVAL and error code -1 if searching operation failed.
 */
int mip_get_uint (MinItemParser * mip, TSChar * tag, unsigned int *value)
{
        TSChar         *start_pos = INITPTR;
        TSChar         *end_pos = INITPTR;
        TSChar         *extra_end_pos = INITPTR;
        int             length = 0;
        int             retval;

        errno = EINVAL;
        retval = -1;

        if (mip != INITPTR) {
                if (strlen (mip->item_line_section_) > 0) {
                        retval = mip_parse_start_and_end_pos (mip, tag,
                                                              &start_pos,
                                                              &end_pos,
                                                              &length,
                                                              &extra_end_pos);

                        if (retval == 0) {
                                mip->item_skip_and_mark_pos_ = end_pos;
                                mip->get_methods_indicator_ = ESTrue;
                                *value = (unsigned int)atoi (start_pos);
                                errno = 0;
                                retval = 0;
                        }
                }
        }

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Gets next unsigned integer value according to earlier used
 *  MIN Item Parser section string.
 *  @param mip Pointer to existing MIN Item Parser data structure.
 *  @param value Pointer to unsigned integer variable for be found
 *               next unsigned integer value section part.
 *  @return Returns error code 0 if operation completes successfully,
 *          else returns error code.
 *
 *  Possible errors:
 *  - EINVAL and error code -1 if searching operation failed.
 */
int mip_get_next_uint (MinItemParser * mip, unsigned int *value)
{
        TSChar         *start_pos = INITPTR;
        TSChar         *end_pos = INITPTR;
        TSChar         *extra_end_pos = INITPTR;
        int             length = 0;
        int             retval;

        errno = EINVAL;
        retval = -1;

        if (mip != INITPTR) {
                if (strlen (mip->item_line_section_) > 0) {
                        retval = mip_parse_start_and_end_pos (mip, INITPTR,
                                                              &start_pos,
                                                              &end_pos,
                                                              &length,
                                                              &extra_end_pos);

                        if (retval == 0) {
                                mip->item_skip_and_mark_pos_ = end_pos;
                                *value = (unsigned int)atoi (start_pos);
                                errno = 0;
                                retval = 0;
                        }
                }
        }

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Gets next tagged unsigned integer value according to given tag string
 *  and earlier used MIN Item Parser section string.
 *  @param mip Pointer to existing MIN Item Parser data structure.
 *  @param tag Pointer to tag string for searching next tagged unsigned
 *             integer value.
 *  @param value Pointer to unsigned integer variable for be found
 *               next tagged unsigned integer value section part.
 *  @return Returns error code 0 if operation completes successfully,
 *          else returns error code.
 *
 *  Possible errors:
 *  - EINVAL and error code -1 if searching operation failed.
 */
int mip_get_next_tagged_uint (MinItemParser * mip, TSChar * tag,
                              unsigned int *value)
{
        TSChar         *start_pos = INITPTR;
        TSChar         *end_pos = INITPTR;
        TSChar         *extra_end_pos = INITPTR;
        int             length = 0;
        int             retval;

        errno = EINVAL;
        retval = -1;

        if (mip != INITPTR) {
                if (strlen (mip->item_line_section_) > 0) {
                        retval = mip_parse_start_and_end_pos (mip, tag,
                                                              &start_pos,
                                                              &end_pos,
                                                              &length,
                                                              &extra_end_pos);

                        if (retval == 0) {
                                mip->item_skip_and_mark_pos_ = end_pos;
                                *value = (unsigned int)atoi (start_pos);
                                errno = 0;
                                retval = 0;
                        }
                }
        }

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Gets first character according to given tag string from MIN Item Parser
 *  section string.
 *  @param mip Pointer to existing MIN Item Parser data structure.
 *  @param tag Pointer to tag string for searching section part string.
 *  @param chr Pointer to character variable for be found first character.
 *  @return Returns error code 0 if operation completes successfully,
 *          else returns error code.
 *
 *  Possible errors:
 *  - EINVAL and error code -1 if section string length is zero.
 *  - EINVAL and error code -2 if parsing operation failed. 
 */
int mip_get_char (MinItemParser * mip, TSChar * tag, TSChar * chr)
{
        TSChar         *start_pos = INITPTR;
        TSChar         *end_pos = INITPTR;
        TSChar         *extra_end_pos = INITPTR;
        int             length = 0;
        int             retval;

        errno = EINVAL;
        retval = -1;

        if (mip != INITPTR) {
                if (strlen (mip->item_line_section_) > 0) {
                        mip->item_skip_and_mark_pos_ = INITPTR;
                        retval = mip_parse_start_and_end_pos (mip, tag,
                                                              &start_pos,
                                                              &end_pos,
                                                              &length,
                                                              &extra_end_pos);

                        if (retval == 0) {
                                mip->item_skip_and_mark_pos_ = end_pos;
                                chr = start_pos;
                                mip->get_methods_indicator_ = ESTrue;
                                /* Parsing operation completed, no errros */
                                errno = 0;
                                retval = 0;
                        }
                }
        }

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Gets next character according to earlier used MIN Item Parser 
 *  section string.
 *  @param mip Pointer to existing MIN Item Parser data structure.
 *  @param chr Pointer to character variable for be found next character.
 *  @return Returns error code 0 if operation completes successfully,
 *          else returns error code.
 *
 *  Possible errors:
 *  - EINVAL and error code -1 if next character searching failed.
 */
int mip_get_next_char (MinItemParser * mip, TSChar * chr)
{
        TSChar         *start_pos = INITPTR;
        TSChar         *end_pos = INITPTR;
        TSChar         *extra_end_pos = INITPTR;
        int             length = 0;
        int             retval;

        errno = EINVAL;
        retval = -1;

        if (mip != INITPTR) {
                if ((mip->get_methods_indicator_ == ESTrue) &&
                    (strlen (mip->item_line_section_) > 0)) {

                        retval = mip_parse_start_and_end_pos (mip, INITPTR,
                                                              &start_pos,
                                                              &end_pos,
                                                              &length,
                                                              &extra_end_pos);

                        if (retval == 0) {
                                mip->item_skip_and_mark_pos_ = end_pos;
                                chr = start_pos;

                                /* Operation completed, no errors */
                                errno = 0;
                                retval = 0;
                        }
                }
        }

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Gets next tagged character according to tag string and earlier used
 *  MIN Item Parser section string.
 *  @param mip Pointer to existing MIN Item Parser data structure.
 *  @param tag Pointer to tag string for searching next tagged section part
 *             string.
 *  @param chr Pointer to character variable for be found next character.
 *  @return Returns error code 0 if operation completes successfully,
 *          else returns error code.
 *
 *  Possible errors:
 *  - EINVAL and error code -1 if next character searching failed.
 */
int mip_get_next_tagged_char (MinItemParser * mip, TSChar * tag,
                              TSChar * chr)
{
        TSChar         *start_pos = INITPTR;
        TSChar         *end_pos = INITPTR;
        TSChar         *extra_end_pos = INITPTR;
        int             length = 0;
        int             retval;

        errno = EINVAL;
        retval = -1;

        if (mip != INITPTR) {
                if ((mip->get_methods_indicator_ == ESTrue) &&
                    (strlen (mip->item_line_section_) > 0)) {

                        retval = mip_parse_start_and_end_pos (mip, tag,
                                                              &start_pos,
                                                              &end_pos,
                                                              &length,
                                                              &extra_end_pos);

                        if (retval == 0) {
                                mip->item_skip_and_mark_pos_ = end_pos;
                                chr = start_pos;

                                /* Operation completed, no errors */
                                errno = 0;
                                retval = 0;
                        }
                }
        }

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Gets remainder string from MIN Item Parser section string.
 *  @param mip Pointer to existing MIN Item Parser data structure.
 *  @param string Reference to remainder string for be found section part.
 *  @return Returns error code 0 if operation completes successfully,
 *          else returns error code.
 *
 *  Possible errors:
 *  - EINVAL and error code -1 if next character searching failed.
 */
int mip_get_remainder (MinItemParser * mip, TSChar ** string)
{
        TSChar         *pos = INITPTR;
        int             retval;

        errno = EINVAL;
        retval = -1;

        if (mip != INITPTR) {
                if (mip->item_skip_and_mark_pos_ != INITPTR) {
                        if ((strlen (mip->item_line_section_) > 0) &&
                            ((strlen (mip->item_line_section_) >
                              (mip->item_skip_and_mark_pos_ -
                               mip->item_line_section_)))) {
                                pos = mip->item_line_section_;
                                pos = strtok (pos, " ");

                                if (pos != NULL) {
                                        mip->item_skip_and_mark_pos_ = pos;

                                        *string =
                                            NEW2 (TSChar, strlen (pos) + 1);
                                        if (*string) {
                                                STRCPY (*string, pos,
                                                        strlen (pos) + 1);
                                                errno = 0;
                                                retval = 0;
                                        }
                                }
                        }
                }
        }

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Sets parsing type value for MIN Item Parser data structure.
 *  @param mip Pointer to existing MIN Item Parser data structure.
 *  @param type New parsing type for parsing operations.
 *  @return Returns error code 0 if operation completes successfully,
 *          else returns error code.
 *
 *  Possible errors:
 *  - EINVAL and error code -1 if MIN Item Parser data structure not available
 */
int mip_set_parsing_type (MinItemParser * mip, TParsingType type)
{
        int             retval = 0;

        if (mip != INITPTR) {
                mip->parsing_type_ = type;
        } else {
                errno = EINVAL;
                retval = -1;
        }

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Gets parsing type value from MIN Item Parser data structure.
 *  @param mip Pointer to existing MIN Item Parser data structure.
 *  @param type Returned parsing type in use of parsing operations.
 *  @return Returns error code 0 if operation completes successfully,
 *          else returns error code.
 *
 *  Possible errors:
 *  - EINVAL and error code -1 if MIN Item Parser data structure not available
 */
TParsingType mip_get_parsing_type (MinItemParser * mip)
{
        /* If SIP data not available, returns error value -1 */
        TParsingType    type = -1;

        if (mip != INITPTR) {
                type = mip->parsing_type_;
        }

        return type;
}

/* ------------------------------------------------------------------------- */
/** Replaces label inside of mip vith value if any occurence found.
 *  @param mip [in:out] item parser instance which content will be examined.
 *  @param label [in] label which occurence will be checked inside of mip
 *  @param value [in] replacement for 'label'
 *  @return 0 in case of success, -1 otherwise.
 * */
int mip_replace (MinItemParser * mip, const TSChar * label,
                 const TSChar * value)
{
        int             retval = ENOERR;
        char           *c = INITPTR;
        char           *c2 = INITPTR;
        unsigned int    llen = 0;
        unsigned int    vlen = 0;
        unsigned int    length = 0;
        unsigned int    i = 0;
        TSChar         *newstr = INITPTR;

        /* do some error checking */
        if (mip == INITPTR) {
                retval = -1;
                goto EXIT;
        }
        if (label == INITPTR) {
                retval = -1;
                goto EXIT;
        }
        if (value == INITPTR) {
                retval = -1;
                goto EXIT;
        }

        /* Check if label used in this line */
        c = strstr (mip->item_line_section_, label);

        /* label found in current line, we need to replace it with value */
        if (c != NULL) {

                llen = strlen (label);
                vlen = strlen (value);

                /* length of the string after replacement */
                length = strlen (mip->item_line_section_)
                    - llen + vlen + 1;

                /* allocate place for new string */
                newstr = NEW2 (TSChar, length);
                memset (newstr, '\0', length);

                /* copy content and replace label with value */
                c2 = &mip->item_line_section_[0];
                while (c2 != c) {
                        newstr[i] = *c2;
                        c2++;
                        i++;
                }

                STRCPY (&newstr[i], value, vlen);
                c = c + llen;
                STRCPY (&newstr[i + vlen], c, strlen (c));

                /* replace buffers inside of item parser */
                DELETE (mip->item_line_section_);
                mip->item_line_section_ = newstr;
        }
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "min_item_parser.tests"
#endif                          /* MIN_UNIT_TEST */
/* ------------------------------------------------------------------------- */
/* End of file */
