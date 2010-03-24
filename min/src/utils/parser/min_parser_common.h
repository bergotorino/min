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
 *  @file       min_parser_common.h
 *  @version    0.1
 *  @brief      This file contains header file of the MIN Parsers common stuff
 */

#ifndef MIN_PARSER_COMMON_H
#define MIN_PARSER_COMMON_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <min_common.h>

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* ------------------------------------------------------------------------- */
/** Indicates what we are looking for in parse_comments_off fction. */
typedef enum {
        ENormalSearch,                  /**< both: // and c-style comments */
        ECStyleSlash,                   /**< // comments */
        ECStyleSlashAndAsterix,         /**< c-style comments */
        EDoRemove,                      /**< Remove comment */
        EQuoteStartSearch
} TSearchType;
/* ------------------------------------------------------------------------- */
/** Indicates the type of used comments. */
typedef enum {
        ENoComments,        /**< No comments type given, parse all. */
        ECStyleComments     /**< C-style comments are removed.      */
} TCommentType;
/* ------------------------------------------------------------------------- */
/** Indicates the source of text to be parsed. */
typedef enum {
        EFileParsing,       /**< Indication for file parsing.   */
        EBufferParsing      /**< Indication for buffer parsing. */
} TParsingMode;
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
#endif                          /* MIN_PARSER_COMMON_H */
/* End of file */
