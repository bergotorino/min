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
 *  @file       min_file_parser.h
 *  @version    0.1
 *  @brief      This file contains header file of the Min File Parser
 */
#ifndef MIN_FILE_PARSER_H
#define MIN_FILE_PARSER_H
/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <stdio.h>
#include <errno.h>
#include "min_parser_common.h"
#include <dllist.h>
#include <min_text.h>
/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */
/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */
/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* ------------------------------------------------------------------------- */
typedef struct _MinFileParser MinFileParser;
/* ------------------------------------------------------------------------- */
/** Indicates what we are looking for. */
typedef enum {
        EStart,             /**< '//' or 'c-style comment' or '"' signs */
        EQuota,             /**< '"' sign */
        EEndOfComment       /**< '*\/' sequence */
} TWhatToFind;
/* ------------------------------------------------------------------------- */
/** Indicates what section we are looking for. */
typedef enum {
        ESectionStart, /**< Opening label */
        ESectionEnd    /**< Closing label */
} TSectionFind;
/* ------------------------------------------------------------------------- */
/** Indocates if file is unicode or not. */
typedef enum {
        EFileNotUnicode = 0, /**< indocates that file is not unicode */
        EFileUnicode = 1     /**< indocates that file is unicode */
} TUnicode;
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* ------------------------------------------------------------------------- */
/** MinFileParser entity. */
struct _MinFileParser {
        /**
         * For file opening. This is the first (base) file. Later other
         * files may be opened if they are included to base one.
         */
        FILE           *base_file_;

        /** Comment type's indication. */
        TCommentType    comment_type_;

        /** Flag indicating if file is unicode: 1=Yes, 0=No */
        TUnicode        is_unicode_;

        /**
         * How many bytes per TSChar in file 
         * (2 for unicode, 1 for non-unicode).
         */
        int             bytes_per_char_;

        /** Handle to file which is currently read. */
        FILE           *current_file_;

        /** Stack of opened files (it does not contain base file). */
        DLList         *file_stack_;

        /** Array of lines belonging to seeked section. */
        DLList         *section_lines_;

        /** Array of already included files (to avoid loop in includes). */
        DLList         *file_names_;

        /**
        * Buffer with eol sequence. Filled when include found and used
        * after last line of inlcuded file.
        */
        TSChar          eol_buff_[3];
};
/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
/** Creates a new file parser.
 *  @param file [in] is the pointer to the FILE entity.
 *  @param is_unicode [in] is a flag indicating if file is unicode.
 *  @param comment_type [in] is an indication how to treat comments in file.
 *  @return adress of the allocaded MinFileParser entity.
 */
MinFileParser *mfp_create (FILE * file,
                            TUnicode is_unicode, TCommentType comment_type);
/* ------------------------------------------------------------------------- */
/** Destroys allocated file parser.
 *  @param fp [in] allocated file parser entity.
 */
void            mfp_destroy (MinFileParser ** sfp);
/* ------------------------------------------------------------------------- */
/**
 * Open and read configuration source and parses a required section.
 * This method will parse next section after the earlier section if
 * seeked parameter is equal 1.
 * If configuration file includes several sections with both start and
 * end tags so seeked parameter seeks the required section.
 * If start tag is not given, returned section begins from the beginning of
 * the file. If end tag is not given, returned sections ends at the end of
 * the file.
 * If section is not found function will return INITPTR
 * @param sfp [in] adress of the Min File Parser
 * @param start_tag [in] indicates the tag name to start from.
 * @param end_tag [in] indicates the tag name to end on.
 * @param offset [in] ...
 * @param seeked [in] indicates section that will be parsed.
 *
 * Possible Errors:
 * - EINVAL: invalid value was passed to the function.
 */
TSChar         *mfp_next_section (MinFileParser * sfp,
                                  const TSChar * start_tag,
                                  const TSChar * end_tag,
                                  int *offset, int seeked);
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
#endif                          /* MIN_FILE_PARSER_H */
/* End of file */
