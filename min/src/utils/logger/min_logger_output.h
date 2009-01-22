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
 *  @file       min_logger_output.h
 *  @version    0.1
 *  @brief      This file contains output plugins for MIN Logger.
 */

#ifndef MIN_LOGGER_OUTPUT_H
#define MIN_LOGGER_OUTPUT_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
/* ------------------------------------------------------------------------- */
#include <stdio.h>              /* FILE */
#include <time.h>               /* struct tm */
#include <sys/types.h>          /* mkdir */
#include <sys/stat.h>           /* mkdir */
#include <errno.h>              /* errno */
#include <unistd.h>             /* getpid */
#include <min_logger_common.h>
/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* ------------------------------------------------------------------------- */
typedef struct min_logger_file_output_t MinLoggerFileOutput;
typedef struct min_logger_null_output_t MinLoggerNullOutput;
typedef struct min_logger_syslog_output_t MinLoggerSyslogOutput;

typedef void    (*ptr2write) (struct output_typeinfo_t *, TSBool, TSBool,
                              TSBool, const TSChar *);
typedef void    (*ptr2destroy) (struct output_typeinfo_t **);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* ------------------------------------------------------------------------- */
/** General structure for output plugins. Acts like a proxy forwrding function
 *  calls to the appropriate plugin structure that is cased based on its type
 *  information. */
struct output_typeinfo_t {
        /** Real output plugin, hidden behind its type information. */
        TSOutput        output_type_;

        /** Pointer to the general write function. */
        ptr2write       write_;

        /** Pointer to destroy function */
        ptr2destroy     destroy_;
};
/* ------------------------------------------------------------------------- */
struct min_logger_file_output_t {
        /** Things that we have to have because this is 'derrived' from
         *  output_typeinfo_t @{*/

        /** Type of the output plugin. */
        TSOutput        output_type_;

        /** Pointer to write function from file output plugin. */
        ptr2write       write_;

        /** Pointer to destroy function */
        ptr2destroy     destroy_;

        /**@}*/

        /** min file output internal variables @{ */
        TSChar         *fileanddirname_;        /**< path to the output file */
	TSLoggerType loggertype_;
                                                /**< the type of the logger */
        TSBool          overwrite_;             /**< overwrite file flag */
        TSBool          withtimestamp_;         /**< add timestamp flag */
        TSBool          withlinebreak_;         /**< add linebreak flag */
        TSBool          witheventranking_;      /**< do event ranking flag */
        TSBool          pididtologfile_;        /**< log pid to logfile */
        TSBool          createlogdir_;          /**< create output directory */
        unsigned int    buffersize_;            /**< size of the buffer */
        TSBool          unicode_;               /**< unicode flag */
        FILE           *file_;                  /**< opened file handler */
        int             isfileopen_;            /**< is file opened flag */
        TSChar         *databuf_;               /**< buffer for the data */
        /**@}*/
};
/* ------------------------------------------------------------------------- */
struct min_logger_null_output_t {
        /** Things that we have to have because this is 'derrived' from
         *  output_typeinfo_t @{*/
        /** Type of the output plugin. */
        TSOutput        output_type_;

        /** Pointer to write function from file output plugin. */
        ptr2write       write_;

        /** Pointer to destroy function */
        ptr2destroy     destroy_;
        /**@}*/
};
/* ------------------------------------------------------------------------- */
struct min_logger_syslog_output_t {
        /** Things that we have to have because this is 'derrived' from
         *  output_typeinfo_t @{*/

        /** Type of the output plugin. */
        TSOutput        output_type_;

        /** Pointer to write function from file output plugin. */
        ptr2write       write_;

        /** Pointer to destroy function */
        ptr2destroy     destroy_;

        /**@}*/

        /** min syslog output internal variables @{ */
        TSBool          unicode_;               /**< unicode flag */
        /**@}*/
};
/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
/** Creates MIN FileOutput plugin 'object' 
 *  @param path [in] output directory.
 *  @param file [in] output file.
 *  @param loggertype [in] type of the logger that is in use.
 *  @param overwrite [in] overwrite file if exists flag.
 *  @param withtimestamp [in] add timestamp flag.
 *  @param withlinebreak [in] add linebreak flag.
 *  @param witheventranking [in] do event ranking flag.
 *  @param pididtologfile [in] process id to logfile flag.
 *  @param createlogdir [in] create output directory if not exists flag.
 *  @param buffersize [in] size of the static buffer.
 *  @param unicode [in] unicode flag.
 *  @return new instance of FileOutput or INITPTR in case of failure.
 */
MinLoggerFileOutput *fo_create (const TSChar * path,
                                 const TSChar * file,
                                 unsigned int loggertype,
                                 TSBool overwrite,
                                 TSBool withtimestamp,
                                 TSBool withlinebreak,
                                 TSBool witheventranking,
                                 TSBool pididtologfile,
                                 TSBool createlogdir,
                                 unsigned int buffersize, TSBool unicode);
/* ------------------------------------------------------------------------- */
/** Destroys MIN FileOutput instance.
 *  @param o [in:out] sfo instance to be destroyed.
 *   
 *  NOTE: after being freed the pointer is set to INITPTR;
 */
void            fo_destroy (struct output_typeinfo_t **o);
/* ------------------------------------------------------------------------- */
/** Creates MIN NullOutput plugin 'object' 
 *  @param path [in] output directory.
 *  @param file [in] output file.
 *  @param loggertype [in] type of the logger that is in use.
 *  @param overwrite [in] overwrite file if exists flag.
 *  @param withtimestamp [in] add timestamp flag.
 *  @param withlinebreak [in] add linebreak flag.
 *  @param witheventranking [in] do event ranking flag.
 *  @param pididtologfile [in] process id to logfile flag.
 *  @param createlogdir [in] create output directory if not exists flag.
 *  @param buffersize [in] size of the static buffer.
 *  @param unicode [in] unicode flag.
 *  @return new instance of FileOutput or INITPTR in case of failure.
 */
MinLoggerNullOutput *no_create (const TSChar * path,
                                 const TSChar * file,
                                 unsigned int loggertype,
                                 TSBool overwrite,
                                 TSBool withtimestamp,
                                 TSBool withlinebreak,
                                 TSBool witheventranking,
                                 TSBool pididtologfile,
                                 TSBool createlogdir,
                                 unsigned int buffersize, TSBool unicode);
/* ------------------------------------------------------------------------- */
/** Destroys MIN NullOutput instance.
 *  @param o [in:out] sfo instance to be destroyed.
 *   
 *  NOTE: after being freed the pointer is set to INITPTR;
 */
void            no_destroy (struct output_typeinfo_t **o);
/* ------------------------------------------------------------------------- */
/** Creates MIN Syslog Output plugin 'object' 
 *  @param path [in] output directory.
 *  @param file [in] output idenfication string for syslog output.
 *  @param loggertype [in] type of the logger that is in use.
 *  @param overwrite [in] overwrite file if exists flag.
 *  @param withtimestamp [in] add timestamp flag.
 *  @param withlinebreak [in] add linebreak flag.
 *  @param witheventranking [in] do event ranking flag.
 *  @param pididtologfile [in] process id to logfile flag.
 *  @param createlogdir [in] create output directory if not exists flag.
 *  @param buffersize [in] size of the static buffer.
 *  @param unicode [in] unicode flag.
 *  @return new instance of SyslogOutput or INITPTR in case of failure.
 */
MinLoggerSyslogOutput *so_create (const TSChar * path,
                                   const TSChar * file,
                                   TSLoggerType loggertype,
                                   TSBool overwrite,
                                   TSBool withtimestamp,
                                   TSBool withlinebreak,
                                   TSBool witheventranking,
                                   TSBool pididtologfile,
                                   TSBool createlogdir,
                                   unsigned int buffersize, TSBool unicode);
/* ------------------------------------------------------------------------- */
/** Destroys MIN Syslog Output instance.
 *  @param o [in:out] sfo instance to be destroyed.
 *   
 *  NOTE: after being freed the pointer is set to INITPTR;
 */
void            so_destroy (struct output_typeinfo_t **o);
/* ------------------------------------------------------------------------- */
#endif                          /* MIN_LOGGER_OUTPUT_H */
/* End of file */
