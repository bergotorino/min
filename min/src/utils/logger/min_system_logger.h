/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Sampo Saaristo
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
 *  @file       min_system_logger.h
 *  @version    0.1
 *  @brief      Header for MIN System logging API.
 */

#ifndef MIN_SYSTEM_LOGGER_H
#define MIN_SYSTEM_LOGGER_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
/* none */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* none */

/* ------------------------------------------------------------------------- */
/* MACROS */


/** debug level 1 messages with three arguments
 *  @param __format__ format as in printf (3)
 *  @param __args__ arguments matching the format
 */
#define MIN_DEBUG1(__format__,__args__ ...)\
do{\
if (min_debug_level > 0)\
 min_debug(__FUNCTION__,__LINE__,__FILE__,__format__,##__args__);\
} while (0)

/** debug level 2 messages with one argument
 *  @param __format__ format as in printf (3)
 *  @param __args__ arguments matching the format
 */
#define MIN_DEBUG2(__format__,__args__ ...)\
do{\
if (min_debug_level > 1)\
 min_debug(__FUNCTION__,__LINE__,__FILE__,__format__,##__args__);\
} while (0)


/** debug level 3 messages
 *  @param __format__ format as in printf (3)
 *  @param __args__ arguments matching the format
 */
#define MIN_DEBUG3(__format__, __args__ ...)\
do{\
if (min_debug_level > 2)\
 min_debug(__FUNCTION__,__LINE__,__FILE__,__format__, ##__args__);\
} while (0)

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* none */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* none */

/* ------------------------------------------------------------------------- */
/* Structures */
/* none */

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */

/** Writes min emergency message to log. The message is printed also to console.
 * @return 0 always
 * @param format message formatting string as in e.g. printf (3)
 */
int             min_emerg (const char *format, ...);

/** Writes min error message to log.
 * @return 0 always
 * @param format message formatting string as in e.g. printf (3)
 */
int             min_err (const char *format, ...);

/** Writes min warning message to log.
 * @return 0 always
 * @param format message formatting string as in e.g. printf (3)
 */
int             min_warn (const char *format, ...);

/** Writes informational message to log.
 * @return 0 always
 * @param format message formatting string as in e.g. printf (3)
 */
int             min_info (const char *format, ...);

/** 
 * global variable for debug level.
 * values 0 - no debug
 *        1 - some debug. 
 *        2 - more debug. 
 *        3 - most debug. 
 */
extern unsigned int    min_debug_level;

/** Writes debug message to log.
 * Use through macros MIN_DEBUGXX.
 * @return 0 always
 * @param func_name function from which debug is called
 * @param lineno linenumber of the c-file the debug message deals with
 * @param file_name name of the file calling debug
 * @param format message formatting string as in e.g. printf (3)
 */
int             min_debug (const char *func_name, 
                            unsigned int lineno,
                            const char *file_name, 
                            const char *format, ...);

/**
 * Opens the min log facility. Call ones per program execution.
 * @return 0 when ok, 1 if the debug level is invalid (not 0-3)
 * @param identifier will be prepended to all log messges
 * @param debug_level the debug level to be used
 * @see min_debug_level
 */
int             min_log_open (const char *identifier,
                               unsigned int debug_level);

/**
 * Closes the min logger facility
 */
int             min_log_close ();

#endif                          /* MIN_SYSTEM_LOGGER_H */
