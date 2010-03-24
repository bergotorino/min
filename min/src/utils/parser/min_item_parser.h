/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Marko Hyyppä
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
 *  @file       min_item_parser.h
 *  @version    0.1
 *  @brief      This file contains header file of the MIN Item Parser
 */

#ifndef MIN_ITEM_PARSER_H
#define MIN_ITEM_PARSER_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <errno.h>
#include "min_common.h"
#include "min_parser_common.h"

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* ------------------------------------------------------------------------- */
typedef struct _MinItemParser MinItemParser;
/* ------------------------------------------------------------------------- */
/** Parsing type can be given when parsing string(GetString and
 *  GetNextString). This enumeration indicates parsing type.
 */
typedef enum {
        /** ENormalParsing:
         *  - Indicates normal parsing without any modifications to
         *    parsed information.
         */
        ENormalParsing,

        /** EQuoteStyleParsing:
         *  - Indicates special parsing. This style of parsing gives to
         *    quote(" ") characters special meaning.
         *  - Information between quotes is handled as a one string. Quotes
         *    not included to information.
         */
        EQuoteStyleParsing
} TParsingType;
/* ------------------------------------------------------------------------- */
/** MinItemParser entity. */
struct _MinItemParser {
        /**
        * Parsed section with GetItemLineL() or GetNextItemLineL() method.
        */
        TSChar         *item_line_section_;

        /**
        * Indicates position where start the parsing.
        */
        TSChar         *item_skip_and_mark_pos_;

        /**
        * Indicator is GetString(), GetInt() or GetChar() called.
        */
        int             get_methods_indicator_;

        /**
        * Parsing type indicator for GetString and GetNextString use.
        */
        TParsingType    parsing_type_;
};
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
MinItemParser *mip_create (TSChar * section, int start_pos, int length);
/* ------------------------------------------------------------------------- */
void            mip_destroy (MinItemParser ** mip);
/* ------------------------------------------------------------------------- */
int             mip_parse_start_and_end_pos (MinItemParser * mip,
                                             TSChar * start_tag,
                                             TSChar ** ref_start_pos,
                                             TSChar ** ref_end_pos,
                                             int *ref_length,
                                             TSChar ** ref_extra_end_pos);
/* ------------------------------------------------------------------------- */
int             mip_get_string (MinItemParser * mip, TSChar * tag,
                                TSChar ** string);
/* ------------------------------------------------------------------------- */
int             mip_get_next_string (MinItemParser * mip, TSChar ** string);
/* ------------------------------------------------------------------------- */
int             mip_get_next_tagged_string (MinItemParser * mip,
                                            TSChar * tag, TSChar ** string);
/* ------------------------------------------------------------------------- */
int             mip_get_int (MinItemParser * mip, TSChar * tag, int *value);
/* ------------------------------------------------------------------------- */
int             mip_get_next_int (MinItemParser * mip, int *value);
/* ------------------------------------------------------------------------- */
int             mip_get_next_tagged_int (MinItemParser * mip, TSChar * tag,
                                         int *value);
/* ------------------------------------------------------------------------- */
int             mip_get_uint (MinItemParser * mip, TSChar * tag,
                              unsigned int *value);
/* ------------------------------------------------------------------------- */
int             mip_get_next_uint (MinItemParser * mip, unsigned int *value);
/* ------------------------------------------------------------------------- */
int             mip_get_next_tagged_uint (MinItemParser * mip, TSChar * tag,
                                          unsigned int *value);
/* ------------------------------------------------------------------------- */
int             mip_get_char (MinItemParser * mip, TSChar * tag,
                              TSChar * chr);
/* ------------------------------------------------------------------------- */
int             mip_get_next_char (MinItemParser * mip, TSChar * chr);
/* ------------------------------------------------------------------------- */
int             mip_get_next_tagged_char (MinItemParser * mip, TSChar * tag,
                                          TSChar * chr);
/* ------------------------------------------------------------------------- */
int             mip_get_remainder (MinItemParser * mip, TSChar ** string);
/* ------------------------------------------------------------------------- */
int             mip_set_parsing_type (MinItemParser * mip,
                                      TParsingType type);
/* ------------------------------------------------------------------------- */
TParsingType    mip_get_parsing_type (MinItemParser * mip);
/* ------------------------------------------------------------------------- */
int             mip_replace (MinItemParser * mip, const TSChar * label,
                             const TSChar * value);
/* ------------------------------------------------------------------------- */

#endif                          /* min_item_parser_H */
/* End of file */
