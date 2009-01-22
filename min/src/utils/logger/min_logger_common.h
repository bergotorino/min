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
 *  @file       min_logger_common.h
 *  @version    0.1
 *  @brief      This file contains common definitions for MIN Logger.
 */

#ifndef MIN_LOGGER_COMMON_H
#define MIN_LOGGER_COMMON_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
/* ------------------------------------------------------------------------- */
#include <min_common.h>
#include <dllist.h>
#include <min_text.h>
/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* ------------------------------------------------------------------------- */
#define MaxLogMessage           512
#define MaxEventRanking         9       /* used in: fo_event_rank */
#define MaxHtmlLineBreak        6       /* <br \> */
#define MaxLineBreak            1       /* \n */
#define MaxTimeString           28      /* "DD.MMM.YYYY HH:MM:SS.MSC    " */
/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* ------------------------------------------------------------------------- */
/** Indicates presentation way of MIN Logger. */
typedef enum {
        ESTxt = 0x1,            /**< Plain text file logger */
        ESData = 0x2,           /**< Data logger */
        ESHtml = 0x4            /**< Html logger */
} TSLoggerType;
/* ------------------------------------------------------------------------- */
/** Indicates MIN Logger output plugin */
typedef enum {
        ESNull = 0x1,           /**< Null ouptut */
        ESFile = 0x2,           /**< File output */
        ESSyslog = 0x4          /**< Syslog output */
} TSOutput;
/* ------------------------------------------------------------------------- */
typedef enum {
        ESNoStyle = 0x00000,    /**< No style parameter */
        ESBold = 0x00001,       /**< Bold */
        ESCursive = 0x00002,    /**< Cursive */
        ESUnderline = 0x00004,  /**< Underline */
        ESBlue = 0x00008,       /**< Blue */
        ESGreen = 0x00010,      /**< Green */
        ESRed = 0x00020,        /**< Red */
        ESImportant = 0x00040,  /**< Important (Line begins with 'IMPORTANT')*/

        ESFatal   = 0x00080,    /**< Fatal errors, MIN cannot operate. */
        ESError   = 0x00100,    /**< Error (Line begins with 'ERROR') */
        ESWarning = 0x00200,    /**< Warning (Line begins with 'WARNING') */
        ESInfo    = 0x00400,    /**< Informational messages. */
        ESNotice  = 0x00800,    /**< More detailed informational messages. */
        ESDebug   = 0x01000,    /**< Debug messages, values of variables.. */
        ESTrace   = 0x02000     /**< Function calls. */
} TSStyle,TLogLevel;
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* ------------------------------------------------------------------------- */
struct output_typeinfo_t;
struct logger_typeinfo_t;
/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* ------------------------------------------------------------------------- */
/** Indicates MIN Logger settings overwriting */
struct overwritten_settings_t {
        TSBool          create_log_dir_;
        TSBool          path_;
        TSBool          hw_path_;
        TSBool          format_;
        TSBool          hw_format_;
        TSBool          output_;
        TSBool          hw_output_;
        TSBool          overwrite_;
        TSBool          time_stamp_;
        TSBool          line_break_;
        TSBool          event_ranking_;
        TSBool          pidid_;
        TSBool          unicode_;
        TSBool          loglevel_;
};
/* ------------------------------------------------------------------------- */
/** Contains overwrited settings. */
struct logger_settings_t {
        TSBool          create_log_dir_;
        char            emulator_path_[4096];
        unsigned int    emulator_format_;
        unsigned int    emulator_output_;
        TSBool          overwrite_;
        TSBool          time_stamp_;
        TSBool          line_break_;
        TSBool          event_ranking_;
        TSBool          pidid_;
        TSBool          unicode_;
        struct overwritten_settings_t is_defined_;
        TLogLevel       loglevel_;
};
/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
#endif                          /* MIN_LOGGER_COMMON_H */
/* End of file */
