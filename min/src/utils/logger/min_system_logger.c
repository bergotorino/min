/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Sampo Saaristo
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
#include <limits.h>

#include <min_common.h>
#include <min_logger.h>
#include <min_logger_common.h>
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
LOCAL void      min_overwrite_settings (struct logger_settings_t *ls,
                                        TSChar * path, TSChar * file,
                                        TSLoggerType * loggertype,
                                        unsigned int *output,
                                        TSBool * overwrite,
                                        TSBool * withtimestamp,
                                        TSBool * withlinebreak,
                                        TSBool * witheventranking,
                                        TSBool * pididtologfile,
                                        TSBool * createlogdir,
                                        TSBool * unicode,
                                        TLogLevel * loglevel);

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */

LOCAL int min_log (int priority, const char *format, va_list ap)
{

        if (priority < LOG_EMERG || priority > LOG_DEBUG)
                return 1;

        vsyslog (priority, format, ap);

        return 0;
}

/* ------------------------------------------------------------------------- */
/** Overwrites previous MIN Logger settings with new values
 *  @param ls [out] settings struture to be overwritten
 *  @param path [in] output directory.
 *  @param file [in] output file.
 *  @param loggertype [in] type of the logger that is in use.
 *  @param output [in] output plugin.
 *  @param overwrite [in] overwrite file if exists flag.
 *  @param withtimestamp [in] add timestamp flag.
 *  @param withlinebreak [in] add linebreak flag.
 *  @param witheventranking [in] do event ranking flag.
 *  @param pididtologfile [in] process id to logfile flag.
 *  @param createlogdir [in] create output directory if not exists flag.
 *  @param unicode [in] unicode flag.
 *  @param loglevel [in] log level for current logger. 
 */
LOCAL void min_overwrite_settings (struct logger_settings_t *ls,
                                   TSChar * path, 
				   TSChar * file,
                                   TSLoggerType * loggertype,
                                   unsigned int *output, 
				   TSBool * overwrite,
                                   TSBool * withtimestamp,
                                   TSBool * withlinebreak,
                                   TSBool * witheventranking,
                                   TSBool * pididtologfile,
                                   TSBool * createlogdir, 
				   TSBool * unicode,
                                   TSStyle * loglevel )
{
        if (ls == INITPTR) {
                goto EXIT;
        }
        if (path == INITPTR) {
                goto EXIT;
        }
        if (file == INITPTR) {
                goto EXIT;
        }
        if (loggertype == INITPTR) {
                goto EXIT;
        }
        if (output == INITPTR) {
                goto EXIT;
        }
        if (overwrite == INITPTR) {
                goto EXIT;
        }
        if (withtimestamp == INITPTR) {
                goto EXIT;
        }
        if (withlinebreak == INITPTR) {
                goto EXIT;
        }
        if (witheventranking == INITPTR) {
                goto EXIT;
        }
        if (pididtologfile == INITPTR) {
                goto EXIT;
        }
        if (createlogdir == INITPTR) {
                goto EXIT;
        }
        if (unicode == INITPTR) {
                goto EXIT;
        }
        if (loglevel == INITPTR) {
                goto EXIT;
        }

        if (ls->is_defined_.path_ == ESTrue) {
                STRCPY (path, ls->emulator_path_,
                        strlen (ls->emulator_path_) + 1);
        }
        if (ls->is_defined_.format_ == ESTrue) {
                *loggertype = ls->emulator_format_;
        }
	if (ls->is_defined_.output_ == ESTrue) {
		*output = ls->emulator_output_;
        }

        /* Rest of the overwrited values */
        if (ls->is_defined_.overwrite_ == ESTrue) {
                *overwrite = ls->overwrite_;
        }
        if (ls->is_defined_.time_stamp_ == ESTrue) {
                *withtimestamp = ls->time_stamp_;
        }
        if (ls->is_defined_.line_break_ == ESTrue) {
                *withlinebreak = ls->line_break_;
        }
        if (ls->is_defined_.event_ranking_ == ESTrue) {
                *witheventranking = ls->event_ranking_;
        }
        if (ls->is_defined_.pidid_ == ESTrue) {
                *pididtologfile = ls->pidid_;
        }
        if (ls->is_defined_.pidid_ == ESTrue) {
                *pididtologfile = ls->pidid_;
        }
        if (ls->is_defined_.create_log_dir_ == ESTrue) {
                *createlogdir = ls->create_log_dir_;
        }
        if (ls->is_defined_.unicode_ == ESTrue) {
                *unicode = ls->unicode_;
        }
        if (ls->is_defined_.loglevel_ == ESTrue) {
                *loglevel = ls->loglevel_;
        }

      EXIT:
        return;
}


/* ======================== FUNCTIONS ====================================== */

/* ------------------------------------------------------------------------- */
/** Opens the min log facility. Call ones per program execution.
 * @return 0 when ok, 1 if the debug level is invalid (not 0-3)
 * @param identifier will be prepended to all log messges
 * @param debug_level the debug level to be used
 * @see min_debug_level
 */
int min_log_open (const char *identifier, unsigned int debug_level)
{
        int             shmid = 0;
        void           *shmaddr = INITPTR;
        int             ret = ~ENOERR;
        struct logger_settings_t settings;
        TSChar          npath[PATH_MAX];
        TSChar          nfile[NAME_MAX];
        /* Initialize with logger default values */
        unsigned int    loggertype = ESTxt;
        unsigned int    output = ESFile;
        TSBool          overwrite = ESTrue;
        TSBool          withtimestamp = ESTrue;
        TSBool          withlinebreak = ESTrue;
        TSBool          witheventranking = ESFalse;
        TSBool          pididtologfile = ESTrue;
        TSBool          createlogdir = ESFalse;
        unsigned int    staticbuffersize = 0; 
        TSBool          unicode = ESFalse;
        TLogLevel       loglevel = ESInfo;

        STRCPY (npath, "/tmp", PATH_MAX);
        STRCPY (nfile, "minlog.txt", NAME_MAX);

 	if (__component_name__==INITPTR) {
		__component_name__=tx_create(identifier);
	} else {
		tx_c_copy(__component_name__,identifier);
	}

        if (__logger__!=INITPTR) { mnl_destroy(&__logger__); }

        /* Contact the settings system and get settings
         * for logger. */
 
        shmid = sm_create ((int)'a', sizeof (struct logger_settings_t));
        if (shmid != -1) {
                shmaddr = sm_attach (shmid);
                if (shmaddr != INITPTR) {
                        ret = sm_read (shmaddr, &settings,
                                       sizeof (struct logger_settings_t));
                        sm_detach (shmaddr);
                }
        }

        if (ret == ENOERR) {
                min_overwrite_settings (&settings, npath, nfile, &loggertype,
                                        &output, &overwrite, &withtimestamp,
                                        &withlinebreak, &witheventranking,
                                        &pididtologfile, &createlogdir,
                                        &unicode,&loglevel);
                __logger__ = mnl_create(MinCreateLoggerParams);

                /* loglevel must be set here because mnl_create 
                 * doesn't take it as an argument */
                if (__logger__ != INITPTR && 
                    settings.is_defined_.loglevel_ == ESTrue) {
                        __logger__->loglevel_ = settings.loglevel_;
                }
        } else {
                __logger__ = mnl_create(MinCreateLoggerParams);
        }

        return 0;
}

/* ------------------------------------------------------------------------- */
/** Closes the min logger facility
 */
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
/** Writes min emergency message to log. The message is printed also to console.
 * @return 0 always
 * @param format message formatting string as in e.g. printf (3)
 */
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
/** Writes min error message to log.
 * @return 0 always
 * @param format message formatting string as in e.g. printf (3)
 */
int min_err (const char *format, ...)
{
        va_list         ap;

        va_start (ap, format);
        min_log (LOG_ERR, format, ap);
        va_end (ap);

        return 0;
}
/* ------------------------------------------------------------------------- */
/** Writes min warning message to log.
 * @return 0 always
 * @param format message formatting string as in e.g. printf (3)
 */
int min_warn (const char *format, ...)
{
        va_list         ap;

        va_start (ap, format);
        min_log (LOG_WARNING, format, ap);
        va_end (ap);

        return 0;
}
/* ------------------------------------------------------------------------- */
/** Writes informational message to log.
 * @return 0 always
 * @param format message formatting string as in e.g. printf (3)
 */
int min_info (const char *format, ...)
{
        va_list         ap;

        va_start (ap, format);
        min_log (LOG_INFO, format, ap);
        va_end (ap);

        return 0;
}
/* ------------------------------------------------------------------------- */
/** Writes debug message to log.
 * Use through macros MIN_DEBUGXX.
 * @return 0 always
 * @param func_name function from which debug is called
 * @param lineno linenumber of the c-file the debug message deals with
 * @param file_name name of the file calling debug
 * @param format message formatting string as in e.g. printf (3)
 */
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
