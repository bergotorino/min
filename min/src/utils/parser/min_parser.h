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
 *  @file       min_parser.h
 *  @version    0.1
 *  @brief      This file contains header file of the MIN Parser
 */

#ifndef MIN_PARSER_H
#define MIN_PARSER_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <stdio.h>
#include <string.h>
#include "min_parser_common.h"
#include "min_file_parser.h"
#include "min_section_parser.h"
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
typedef struct _MinParser MinParser;
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* ------------------------------------------------------------------------- */
/** It is a MIN Framework parser structure. */
struct _MinParser {
        /** For file opening */
        FILE           *file_;

        /** Indicates position where we are currently. */
        int             offset_;

        /** Comment type's indication. */
        TCommentType    comment_type_;

        /** Indication which mode of parsing are in use. */
        TParsingMode    parsing_mode_;

        /** For buffer parsing */
        TSChar         *buffer_tmp_;
        TSChar         *buffer_;

        /** Flag indicating if file is unicode: 1=Yes, 0=No */
        TUnicode        is_unicode_;

        /** Pointer to file parser */
        MinFileParser *file_parser_;
};
/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
MinParser      *mp_create (const TSChar * path, const TSChar * file,
                           TCommentType comments);
/* ------------------------------------------------------------------------- */
MinParser      *mp_create_mem (const TSChar * buffer, TCommentType comments);
/* ------------------------------------------------------------------------- */
void            mp_destroy (MinParser ** sp);
/* ------------------------------------------------------------------------- */
MinSectionParser *mp_section (MinParser * sp, const TSChar * start_tag,
                               const TSChar * end_tag, int seeked);
/* ------------------------------------------------------------------------- */
MinSectionParser *mp_next_section (MinParser * sp, 
                                    const TSChar * start_tag,
                                    const TSChar * end_tag, 
                                    int seeked);
/* ------------------------------------------------------------------------- */
#endif                          /* MIN_PARSER_H */
/* End of file */
