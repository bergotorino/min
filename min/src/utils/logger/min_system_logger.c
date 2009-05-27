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
 *  @file       min_system_logger.c
 *  @version    0.1
 *  @brief      This file contains implementation of MIN System logger API
 */


/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <syslog.h>

#include <min_common.h>
#include <min_logger.h>
#include <min_system_logger.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/** maximum size for format string in debug messages */
#define FMT_MAX_SIZE 255

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
MinLogger *__logger__ = INITPTR;
Text *__component_name__ = INITPTR;

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */

/** Writes min log by calling vsyslog() system function
 * @return 1 if priority is invalid, 0 on ok.
 * @param priority min message priority LOG_DEBUG .. 
 * @param format message formatting string as in e.g. printf (3)e
 * @param ap var args strucuture (/usr/include/stdarg.h)
 */
LOCAL int       min_log (int priority, const char *format, va_list ap);

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* None */

LOCAL int min_log (int priority, const char *format, va_list ap)
{

        if (priority < LOG_EMERG || priority > LOG_DEBUG)
                return 1;

        vsyslog (priority, format, ap);

        return 0;
}


/* ======================== FUNCTIONS ====================================== */

/* ------------------------------------------------------------------------- */

int min_log_open (const char *identifier, unsigned int debug_level)
{
	if (__component_name__==INITPTR) {
		__component_name__=tx_create(identifier);
	} else {
		tx_c_copy(__component_name__,identifier);
	}

        if (__logger__!=INITPTR) { mnl_destroy(&__logger__); }

        __logger__ = mnl_create("/tmp","minlog.txt",
			 	MinLoggerDefaultValues);
        return 0;
}

/* ------------------------------------------------------------------------- */

int min_log_close ()
{
	if (__component_name__!=INITPTR) {
		tx_destroy(&__component_name__);
		__component_name__ = INITPTR;
	}
	if (__logger__!=INITPTR) {
		mnl_destroy(&__logger__);
		__logger__ = INITPTR;
	}

        return 0;
}

/* ------------------------------------------------------------------------- */

int min_emerg (const char *format, ...)
{
        va_list         ap;

        va_start (ap, format);
#if 0
        vmnl_log(sys_logger,ESFatal,format,ap);	

        min_log (LOG_EMERG, format, ap);
#endif
        va_end (ap);

        return 0;
}

/* ------------------------------------------------------------------------- */

int min_err (const char *format, ...)
{
        va_list         ap;

        va_start (ap, format);
        min_log (LOG_ERR, format, ap);
        va_end (ap);

        return 0;
}

/* ------------------------------------------------------------------------- */

int min_warn (const char *format, ...)
{
        va_list         ap;

        va_start (ap, format);
        min_log (LOG_WARNING, format, ap);
        va_end (ap);

        return 0;
}

/* ------------------------------------------------------------------------- */

int min_info (const char *format, ...)
{
        va_list         ap;

        va_start (ap, format);
        min_log (LOG_INFO, format, ap);
        va_end (ap);

        return 0;
}

/* ------------------------------------------------------------------------- */

int min_debug (const char *func, unsigned int lineno, const char *file_name,
                const char *format, ...)
{
        char            fmt[FMT_MAX_SIZE];
        va_list         ap, ap2;

        sprintf (fmt, "%s:%d:%s():%s", file_name, lineno, func, format);
        va_start (ap, format);
        va_copy (ap2, ap);
        min_log (LOG_INFO, fmt, ap2);
        va_end (ap);
        va_end (ap2);

        return 0;
}

/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#if 0                           /* def MIN_UNIT_TEST (None) */
#include "check.h"

/* ------------------------------------------------------------------------- */

int template_tests ()
{
        int             number_failed;

        number_failed = 0;

        return number_failed;
}

/* ------------------------------------------------------------------------- */

#endif  /* 0 */                        /* MIN_UNIT_TEST */

/* End of file */
