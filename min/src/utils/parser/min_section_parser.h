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
 *  @brief      This file contains header file of the MIN Section Parser.
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
MinSectionParser *mmp_create (unsigned int length);
/* ------------------------------------------------------------------------- */
void              mmp_destroy (MinSectionParser ** msp);
/* ------------------------------------------------------------------------- */
MinItemParser    *mmp_get_item_line (MinSectionParser * msp,
				     const TSChar * tag,
				     TSTagToReturnValue tag_indicator);
/* ------------------------------------------------------------------------- */
MinItemParser    *mmp_get_next_item_line (MinSectionParser * msp);
/* ------------------------------------------------------------------------- */
MinItemParser    *mmp_get_next_tagged_item_line (MinSectionParser * msp,
						 const TSChar * tag,
						 TSTagToReturnValue
						 tag_indicator);
/* ------------------------------------------------------------------------- */
MinSectionParser *mmp_sub_section (MinSectionParser * msp,
                                    const TSChar * start_tag,
                                    const TSChar * end_tag, int seeked);
/* ------------------------------------------------------------------------- */
MinSectionParser *mmp_next_sub_section (MinSectionParser * msp,
                                         const TSChar * start_tag,
                                         const TSChar * end_tag, int seeked);
/* ------------------------------------------------------------------------- */
int               mmp_get_line (MinSectionParser * msp, const TSChar * tag,
				TSChar ** line,
				TSTagToReturnValue tag_indicator);
/* ------------------------------------------------------------------------- */
int               mmp_get_next_line (MinSectionParser * msp, TSChar ** line);
/* ------------------------------------------------------------------------- */
int               mmp_get_next_tagged_line (MinSectionParser * msp,
                                          const TSChar * tag, TSChar ** line,
                                          TSTagToReturnValue tag_indicator);
/* ------------------------------------------------------------------------- */
int               mmp_get_position (MinSectionParser * msp);
/* ------------------------------------------------------------------------- */
int               mmp_set_position (MinSectionParser * msp, unsigned int pos);
/* ------------------------------------------------------------------------- */
void              mmp_set_data (MinSectionParser * msp, const TSChar * data,
				TSChar * start_pos, unsigned int length);
/* ------------------------------------------------------------------------- */
const TSChar      *mmp_des (const MinSectionParser * msp);
/* ------------------------------------------------------------------------- */
#endif                          /* MIN_SECTION_PARSER_H */
/* End of file */
