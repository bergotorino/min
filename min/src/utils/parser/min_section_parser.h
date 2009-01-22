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
 *  @file       min_section_parser.h
 *  @version    0.1
 *  @brief      This file contains header file of the Min Section Parser.
 */

#ifndef MIN_SECTION_PARSER_H
#define MIN_SECTION_PARSER_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <stdio.h>
#include "min_parser_common.h"
#include "min_item_parser.h"

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* ------------------------------------------------------------------------- */
/** Indicates is tag included in returned value. */
typedef enum {
        ESTag,   /**< Return value includes the tag */
        ESNoTag  /**< Return value include no tag */
} TSTagToReturnValue;
/* ------------------------------------------------------------------------- */
typedef struct _MinSectionParser MinSectionParser;
/* ------------------------------------------------------------------------- */
/** Min Section Parser entity. */
struct _MinSectionParser {
        /** Parsed section with SectionL() or SubSectionL() method. */
        TSChar         *section_;

        /** Section length */
        unsigned int    length_;

        /** Section length definitions. */
        TSChar         *buffer_section_;

        /** Indicates position where start the parsing. */
        int             skip_and_mark_pos_;

        /**
        * For line and item line parsing. Indicator is GetLine() or
        * GetItemLineL() method used.
        */
        TSBool          line_indicator_;

        /** Indicates position where we are currently(subsection). */
        TSChar         *sub_offset_;
};
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
/** Creates a new MinSectionParser.
 *  @param length [in] the length of the section to be parsed with this parser.
 *  @return pointer to the newly allocated parser entity or INITPTR in case
 *          of failure.
 *
 *  Possible Errors:
 *  - ENOMEM: not sufficient memmory to allocate new object.
 */
MinSectionParser *mmp_create (unsigned int length);
/* ------------------------------------------------------------------------- */
/** Destroys previously created MinSectionParser.
 *  @param msp [in:out] MinSectionParser entity to be deallocated.
 *
 *  NOTE: msp in set to INITPTR inside in this function.
 */
void            mmp_destroy (MinSectionParser ** msp);
/* ------------------------------------------------------------------------- */
/** Parses a line for items parsing with a tag.
 *  @param msp [in] MinSectionParser entity to operate on.
 *  @param tag [in] indicates the tag to start with. If start tag is empty
 *         the parsing starts beging of the section.
 *  @param tag_indicator [in] indicates if tag will be included to the
 *         returned object.
 *  @return adress of MinItemParser or INITPTR in case of failue.
 */
MinItemParser *mmp_get_item_line (MinSectionParser * msp,
                                   const TSChar * tag,
                                   TSTagToReturnValue tag_indicator);
/* ------------------------------------------------------------------------- */
/** Parses a next line for items parsing.
 *  @param msp [in] MinSectionParser entity to operate on.
 *  @return adress of MinItemParser.
 */
MinItemParser *mmp_get_next_item_line (MinSectionParser * msp);
/* ------------------------------------------------------------------------- */
/** Parses a next line for items parsing with a tag.
 *  @param msp [in] MinSectionParser entity to operate on.
 *  @param tag [in] indicates the tag to start with. If start tag is empty
 *         the parsing starts beging of the section.
 *  @param tag_indicator [in] indicates if tag will be included to the
 *         returned object.
 *  @return adress of MinItemParser.
 */
MinItemParser *mmp_get_next_tagged_item_line (MinSectionParser * msp,
                                               const TSChar * tag,
                                               TSTagToReturnValue
                                               tag_indicator);
/* ------------------------------------------------------------------------- */
/** Parses a sub sections from the main section with a start and with
 *  an end tag.
 *  @param msp [in] MinSectionParser entity to operate on.
 *  @param start_tag [in] indicates the tag to start on.
 *  @param end_tag [in] indicates the tag to end on.
 *  @param seeked [in] The seeked parameters indicates subsection that will
 *         be parsed.
 *  @return adress of MinSectionParser or INITPTR in case of failure.
 *
 *  - If start tag is empty the parsing starts beging of the section.
 *  - If end tag is empty the parsing goes end of section.
 *  - If configuration file includes several subsections with both start
 *    and end tags so seeked parameter seeks the required subsection.
 */
MinSectionParser *mmp_sub_section (MinSectionParser * msp,
                                    const TSChar * start_tag,
                                    const TSChar * end_tag, int sekked);
/* ------------------------------------------------------------------------- */
/** Parses a next subsections from the main section with a start and with
 *  a end tag.
 *  @param msp [in] MinSectionParser entity to operate on.
 *  @param start_tag [in] indicates the tag to start on.
 *  @param end_tag [in] indicates the tag to end on.
 *  @param seeked [in] The seeked parameters indicates subsection that will
 *         be parsed.
 *  @return adress of MinSectionParser or INITPTR in case of failure.
 *
 *  - If start tag is empty the parsing starts beging of the section.
 *  - If end tag is empty the parsing goes end of section.
 *  - This method will parse next subsection after the earlier subsection
 *    if seeked parameter is not given.
 *  - If configuration file includes several subsections with both start
 *    and end tags so seeked parameter seeks the required subsection.
 */
MinSectionParser *mmp_next_sub_section (MinSectionParser * msp,
                                         const TSChar * start_tag,
                                         const TSChar * end_tag, int seeked);
/* ------------------------------------------------------------------------- */
/** Get a line from section with a tag.
 *  @param msp [in] MinSectionParser entity to operate on.
 *  @param tag [in] indicates the tag to start with. If start tag is empty
 *         the parsing starts beging of the section.
 *  @param tag_indicator [in] indicates if tag will be included to the
 *         returned line.
 *  @param line [out] parsed line is returned through this variable.
 *  @return 0 in case of success, -1 in case of failure.
 *
 *  NOTE: Keep in mind that, after usage, you must deallocate line because it
 *        is malloced in this function.
 *  NOTE: In case of failure the line is set to INITPTR
 *
 *  Possible Errors:
 *  - EINVAL: when invalid value was passed as a parameter.
 */
int             mmp_get_line (MinSectionParser * msp, const TSChar * tag,
                              TSChar ** line,
                              TSTagToReturnValue tag_indicator);
/* ------------------------------------------------------------------------- */
/** Get next line.
 *  @param msp [in] MinSectionParser entity to operate on.
 *  @param line [out] parsed line is returned through this variable.
 *  @return 0 in case of success, -1 in case of failure.
 *
 *  NOTE: Keep in mind that, after usage, you must deallocate line because it
 *        is malloced in this function.
 *  NOTE: In case of failure the line is set to INITPTR
 *
 *  Possible Errors:
 *  - EINVAL: when invalid value was passed as a parameter.
 */
int             mmp_get_next_line (MinSectionParser * msp, TSChar ** line);
/* ------------------------------------------------------------------------- */
/** Get next line with tag.
 *  @param msp [in] MinSectionParser entity to operate on.
 *  @param tag [in] indicates the tag to start with. If start tag is empty
 *         the parsing starts beging of the section.
 *  @param line [out] parsed line is returned through this variable. If line
 *         is not found INITPTR is returned.
 *  @param tag_indicator [in] indicates if tag will be included to the
 *         returned line.
 *  @return 0 in case of success, -1 in case of failure.
 *
 *  NOTE: Keep in mind that, after usage, you must deallocate line because it
 *        is malloced in this function.
 *  NOTE: In case of failure the line is set to INITPTR
 *
 *  Possible Errors:
 *  - EINVAL: when invalid value was passed as a parameter.
 */
int             mmp_get_next_tagged_line (MinSectionParser * msp,
                                          const TSChar * tag, TSChar ** line,
                                          TSTagToReturnValue tag_indicator);
/* ------------------------------------------------------------------------- */
/** Get current position.
 *  @param msp [in] MinSectionParser entity to operate on.
 *  @return current parsing position or -1 in case of failure.
 *
 *  Possible Errors:
 *  - EINVAL: invalid value passed as a parameter.
 */
int             mmp_get_position (MinSectionParser * msp);
/* ------------------------------------------------------------------------- */
/** Set position.
 *  @param msp [in] MinSectionParser entity to operate on.
 *  @param pos [in] indicates the position to which section parser should go.
 *  @return an error code.
 *
 *  SetPosition can be used to set parsing position, e.g. to rewind
 *  back to some old position retrieved with GetPosition.
 */
int             mmp_set_position (MinSectionParser * msp, unsigned int pos);
/* ------------------------------------------------------------------------- */
/** Create a section.
 *  @param msp [in] MinSectionParser entity to operate on.
 *  @param data [in] which are to be parsed.
 *  @param start_pos [in] position from which parsing should start.
 *  @param length [in] of the data in bytes.
 */
void            mmp_set_data (MinSectionParser * msp, const TSChar * data,
                              TSChar * start_pos, unsigned int length);
/* ------------------------------------------------------------------------- */
/** Returns a current section.
 *  @param msp [in] MinSectionParser entity to operate on.
 *  @return pointer to the data that parser is parsing.
*/
const TSChar   *mmp_des (const MinSectionParser * msp);
/* ------------------------------------------------------------------------- */
#endif                          /* MIN_SECTION_PARSER_H */
/* End of file */
