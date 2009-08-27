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
 *  @file       min_logger_output.c
 *  @version    0.1
 *  @brief      This file contains implementation of MIN Logger output plugins
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <min_system_logger.h>
#include <min_logger_output.h>
#include <min_logger.h>
#include <sys/time.h>
#include <syslog.h>
/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
LOCAL void      fo_write (struct output_typeinfo_t *fo,
                          TSBool with_timestamp,
                          TSBool with_linebreak,
                          TSBool with_eventrank, const TSChar * data);
/* ------------------------------------------------------------------------- */
LOCAL void      fo_create_directory (struct min_logger_file_output_t *fo,
                                     const TSChar * path);
/* ------------------------------------------------------------------------- */
LOCAL void      fo_open_existing_file (struct min_logger_file_output_t *fo,
                                       const TSChar * path,
                                       const TSChar * file);
/* ------------------------------------------------------------------------- */
LOCAL void      fo_create_new_file (struct min_logger_file_output_t *fo,
                                    const TSChar * path, const TSChar * file);
/* ------------------------------------------------------------------------- */
LOCAL void      fo_event_ranking (TSChar * data);
/* ------------------------------------------------------------------------- */
LOCAL void      fo_add_timestamp_to_data (TSChar * data);
/* ------------------------------------------------------------------------- */
LOCAL void      fo_file_type (struct min_logger_file_output_t *fo,
                              const TSChar * filename, TSChar ** filetype);
/* ------------------------------------------------------------------------- */
LOCAL void      fo_test_file_with_pid (const TSChar * file, TSChar * newfile);
/* ------------------------------------------------------------------------- */
LOCAL inline void no_write (struct output_typeinfo_t *fo,
                            TSBool with_timestamp,
                            TSBool with_linebreak,
                            TSBool with_eventrank, const TSChar * data);
/* ------------------------------------------------------------------------- */
LOCAL void      so_write (struct output_typeinfo_t *so,
                          TSBool with_timestamp,
                          TSBool with_linebreak,
                          TSBool with_eventrank, const TSChar * data);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/** Writes message through the file output. 
 *  @param fo [in] file output plugin used for writing. 
 *  @param with_timestamp [in] flag indicates presence of timestamp.
 *  @param with_linebreak [in] flag indicating presence of linebreak. 
 *  @param with_eventrank [in] on/off for event ranking feature.
 *  @param data [in] the data to be written. 
 */
LOCAL void fo_write (struct output_typeinfo_t *fo,
                     TSBool with_timestamp,
                     TSBool with_linebreak,
                     TSBool with_eventrank, const TSChar * data)
{
        MinLoggerFileOutput *sxfo = INITPTR;
        unsigned int    extralength = 0;
        unsigned int    datalen = 0;
        unsigned int    currlen = 0;
        int             ret = 0;
	TSChar         *malloced = NULL;
        TSChar         *buffer = INITPTR;

        if (fo == INITPTR) {
                goto EXIT;
        }
        if (data == INITPTR) {
                goto EXIT;
        }

        sxfo = (MinLoggerFileOutput *) fo;
        if (!sxfo->isfileopen_) { goto EXIT; }
        datalen = strlen (data);

        /* Extra space calculation */
        if (sxfo->witheventranking_ && with_eventrank) {
                extralength = MaxEventRanking;
        }

        if ((sxfo->loggertype_ == ESHtml) && sxfo->withlinebreak_
            && with_linebreak) {
                /* <br /> = 6 */
                extralength += MaxHtmlLineBreak;
        }

        if (sxfo->withlinebreak_ && with_linebreak) {
                /* \n\0 */
                extralength += 1 + 1;
        }

        if (sxfo->withtimestamp_ && with_timestamp) {
                extralength += MaxTimeString;
        }

        /* Data straight to the file */
        if ((extralength == 0) && (sxfo->unicode_ == ESFalse)) {
                ret = fwrite (data, sizeof (TSChar), datalen, sxfo->file_);
                if (ret != datalen) {
                        MIN_WARN ("Error in writing to the file");
                }
                goto EXIT;
        }

        /* Output buffer creation */
        if (sxfo->databuf_ != INITPTR) {
                /* will use buffer created in fo_create */
                if ((datalen + extralength) >= sxfo->buffersize_) {
                        MIN_WARN ("Buffersize too small [%d]>=[%d]",
                                   datalen + extralength,
                                   sxfo->buffersize_);
                        goto EXIT;
                } else {
                        buffer = &sxfo->databuf_[0];
                }
        } else {
                /* will use locally created buffer */
                malloced = NEW2 (TSChar, datalen + extralength + 1);
		buffer = malloced;
                if (buffer == NULL) {
                        MIN_WARN ("Buffer size to big [%d]",
                                   datalen + extralength + 1);
                        goto EXIT;
                }
                memset (buffer, '\0', datalen + extralength + 1);
        }

        /* Prefix: eventranking, timestamp */
        if (sxfo->witheventranking_ && with_eventrank && sxfo->withtimestamp_) {
                fo_event_ranking (buffer);
                currlen = strlen (buffer);
        } else if (sxfo->withtimestamp_ && with_timestamp) {
                if (sxfo->witheventranking_ && with_eventrank) {
                        fo_event_ranking (buffer);
                        currlen = strlen (buffer);
                }
                fo_add_timestamp_to_data (&buffer[currlen]);
                currlen = strlen (buffer);
        }

        /* The real message: copy data */
        STRCPY (&buffer[currlen], data, datalen);
        currlen = strlen (buffer);

        /* Postfix: line ending */
        if ((sxfo->loggertype_ == ESHtml) && sxfo->withlinebreak_
            && with_linebreak) {
                STRCPY (&buffer[currlen], "<br />", 6);
                currlen += 6;
        }

        if (sxfo->withlinebreak_ && with_linebreak) {
                STRCPY (&buffer[currlen], "\n", 2);
                currlen += 1;
        }

        /* Write data to the file */
        if (sxfo->unicode_ == ESTrue) {
                /* Not supported yet */

        } else {
                /* Normal writing */
                ret = fwrite (buffer, sizeof (TSChar), currlen, sxfo->file_);
                fflush(sxfo->file_);
                if (ret != currlen) {
                        MIN_WARN ("Error in writing to the file");
                }
        }

        /* Clean-up */
        if (sxfo->buffersize_ == 0) {
		if (buffer != malloced)
			DELETE (buffer);
        }
        else {
                memset (sxfo->databuf_, '\0', sxfo->buffersize_);
        }

      EXIT:
	DELETE (malloced);
        return;
}

/* ------------------------------------------------------------------------- */
/** Opens existing file. 
 *  @param fo [in] file output plugin used for this operation.
 *  @param path [in] path to the file we want to open.
 *  @param file [in] file name we want to open.
 */
LOCAL void fo_open_existing_file (struct min_logger_file_output_t *fo,
                                  const TSChar * path, const TSChar * file)
{
        TSChar         *fname = INITPTR;
        TSChar         *file_ext = INITPTR;
        TSChar         *file_new = INITPTR;
        TSChar         *c = INITPTR;
        TSChar          file_tmp[MaxFileName];
        int             len = 0;

        if (fo == INITPTR) {
                goto EXIT;
        }
        if (path == INITPTR) {
                goto EXIT;
        }
        if (file == INITPTR) {
                goto EXIT;
        }

        /* Filename string must be valid for open existing file */
        if (strlen (file) > 0) {
                /* If existing file is open, then close this file first */
                if (fo->isfileopen_) {
                        fclose (fo->file_);
                        if (fo->fileanddirname_) {
                                DELETE (fo->fileanddirname_);
                                fo->isfileopen_ = ESFalse;
                        }
                }

                fo_file_type (fo, file, &file_ext);
                c = strrchr (file, '.');

                if ((c != NULL) && (fo->pididtologfile_)) {
                        fo_test_file_with_pid (file, file_tmp);
                        file_new = NEW2 (TSChar, MaxFileName);
                        len = c - file;
                        STRCPY (file_new, file, len);
                        STRCPY (file_new + len, file_tmp,
                                strlen (file_tmp) + 1);
                } else {
                        file_new =
                            NEW2 (TSChar,
                                  strlen (file) + strlen (file_ext) + 1);
                        sprintf (file_new, "%s%s", file, file_ext);
                }

                DELETE (file_ext);

                fname = NEW2 (TSChar, strlen (path) + strlen (file_new) + 2);

                if (path[strlen (path) - 1] == '/') {
                        sprintf (fname, "%s%s", path, file_new);
                } else {
                        sprintf (fname, "%s/%s", path, file_new);
                }

		DELETE (file_new);

                fo->file_ = fopen (fname, "a");

                if (fo->file_) {
                        fo->fileanddirname_ = fname;
                        fo->isfileopen_ = ESTrue;
                } else {
                        fo->file_ = INITPTR;
                        fo->isfileopen_ = ESFalse;
                        DELETE (fname);
                }
        }
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
/** Creates new file. 
 *  @param fo [in] file output plugin used for this operation.
 *  @param path [in] path under which we want file to be created.
 *  @param file [in] name of the file we want to create.
 */
LOCAL void fo_create_new_file (struct min_logger_file_output_t *fo,
                               const TSChar * path, const TSChar * file)
{
        TSChar         *fname = INITPTR;
        TSChar         *file_ext = INITPTR;
        TSChar         *file_new = INITPTR;
        TSChar         *c = INITPTR;
        TSChar          file_tmp[MaxFileName];
        int             len = 0;

        if (fo == INITPTR) {
                goto EXIT;
        }
        if (path == INITPTR) {
                goto EXIT;
        }
        if (file == INITPTR) {
                goto EXIT;
        }

        /* Filename string must be exist for new file creation */
        if (strlen (file) > 0) {
                /* If existing file is open, then close this file first */
                if (fo->isfileopen_) {
                        fclose (fo->file_);
                        if (fo->fileanddirname_) {
                                DELETE (fo->fileanddirname_);
                                fo->isfileopen_ = ESFalse;
                        }
                }
                fo_file_type (fo, file, &file_ext);
                c = strrchr (file, '.');

                if ((c != NULL) && (fo->pididtologfile_)) {
                        fo_test_file_with_pid (file, file_tmp);
                        file_new = NEW2 (TSChar, MaxFileName);
                        len = c - file;
                        STRCPY (file_new, file, len);
                        STRCPY (file_new + len, file_tmp,
                                strlen (file_tmp) + 1);
                } else {
                        file_new =
                            NEW2 (TSChar,
                                  strlen (file) + strlen (file_ext) + 1);
                        sprintf (file_new, "%s%s", file, file_ext);
                }

                DELETE (file_ext);

                fname = NEW2 (TSChar, strlen (path) + strlen (file_new) + 2);

                if (path[strlen (path) - 1] == '/') {
                        sprintf (fname, "%s%s", path, file_new);
                } else {
                        sprintf (fname, "%s/%s", path, file_new);
                }
                DELETE (file_new);

                fo->file_ = fopen (fname, "a");

                if (fo->file_) {
                        fo->fileanddirname_ = fname;
                        fo->isfileopen_ = ESTrue;
                } else {
                        fo->file_ = INITPTR;
                        fo->isfileopen_ = ESFalse;
                        DELETE (fname);
                }
        }
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
/** Creates new directory.
 *  @param fo [in] file output plugin used for this operation.
 *  @param path [in] new directory to be created.
 */
LOCAL void fo_create_directory (struct min_logger_file_output_t *fo,
                                const TSChar * path)
{
        int             retval = ENOERR;
        if (fo == INITPTR) {
                goto EXIT;
        }
        if (path == INITPTR) {
                goto EXIT;
        }
        retval = mkdir (path, S_IRWXU | S_IRWXG | S_IRWXO);
        if (retval == -1) {
                MIN_WARN ("Cannot create a directory: [%s], because: %s",
                           path, strerror (errno));
        }
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
/** Adds event ranking to the data.
 *  @param data [in:out] data to which event ranking is appended.
 */
LOCAL void fo_event_ranking (TSChar * data)
{
        time_t          ticks;

        if (data == INITPTR) {
                min_err ("%s:%s:%d - data string equal to INITPTR", __FILE__,
                          __FUNCTION__, __LINE__);
                goto EXIT;
        }

        ticks = time (NULL) / (long int)64;
        ticks &= 0xffff;

        /* Output it */
        sprintf (data, "%5ld    ", (long int)ticks);
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
/** Gest the type of the file.
 *  @param fo [in] file output plugin used for this operation.
 *  @param filename [in] the name of the file to be checked.
 *  @param filetype [out] the type of checked file.
 *
 *  NOTE: filetype is malloced inside of this function.
 */
LOCAL void fo_file_type (struct min_logger_file_output_t *fo,
                         const TSChar * filename, TSChar ** filetype)
{
        TSBool          filetypeisset = ESFalse;
        TSChar         *c = INITPTR;
        char            tmp[16];
        tmp[0] = '\0';

        if (fo == INITPTR) {
                goto EXIT;
        }
        if (filename == INITPTR) {
                goto EXIT;
        }

        /* Check if extension is set for file. If not then add process ID - if
         * it is allowed. */
        c = strrchr (filename, '.');
        if (c != NULL) {
                filetypeisset = ESTrue;
        } else if (fo->pididtologfile_ == ESTrue) {
                sprintf (tmp, "_%x", getpid ());
        }

        /* Add file type after process id. */
        if (fo->loggertype_ == ESTxt && !filetypeisset) {
                c = strchr (tmp, '\0');
                sprintf (c, ".txt");
        } else if (fo->loggertype_ == ESHtml && !filetypeisset) {
                c = strchr (tmp, '\0');
                sprintf (c, ".html");
        }

        *filetype = NEW2 (char, strlen (tmp) + 1);
        STRCPY (*filetype, tmp, strlen (tmp) + 1);
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
/** Generate pricess id to test file name. 
 *  @param file [in] old file name.
 *  @param newfile [out] file name with pid.
 *
 */
LOCAL void fo_test_file_with_pid (const TSChar * file, TSChar * newfile)
{
        char           *c = INITPTR;

        if (file == INITPTR) {
                goto EXIT;
        }
        if (newfile == INITPTR) {
                goto EXIT;
        }
        if (strlen (file) == 0) {
                goto EXIT;
        }

        /* Look for dot */
        c = strrchr (file, '.');

        if (c != NULL) {
                sprintf (newfile, "_%x%s", getpid (), c);
        } else {
                sprintf (newfile, "%s", file);
        }
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
/** Adds timestamp tp data.
 *  @param data [in:out] data to which timestamp is appended.
 */
LOCAL void fo_add_timestamp_to_data (TSChar * data)
{
        struct timeval  tv;
        struct timezone tz;
        struct tm      *time_data = INITPTR;

        if (data == INITPTR) {
                goto EXIT;
        }

        gettimeofday (&tv, &tz);
        time_data = localtime (&tv.tv_sec);

        /*
           Timestamp string format: "DD.MON.YEAR HH:MM:SS.MSC"
           DD   = Day number (01-31)
           MON  = Month number (01-12)
           YEAR = Year number (19xx)
           HH   = Hours (00-11)
           MM   = Minutes (00-59)
           SS   = Seconds (00-59)
           MSC  = Microseconds (000-999)
         */
        strftime (data, MaxTimeString, "%d.%b.%Y %X.", time_data);
        sprintf (data + (MaxTimeString - 7), "%3ld    ", tv.tv_usec / 1000);
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
/** Writes message through the null output. 
 *  @param fo [in] file output plugin used for writing. 
 *  @param with_timestamp [in] flag indicates presence of timestamp.
 *  @param with_linebreak [in] flag indicating presence of linebreak. 
 *  @param with_eventrank [in] on/off for event ranking feature.
 *  @param data [in] the data to be written. 
 */
LOCAL inline void no_write (struct output_typeinfo_t *fo,
                            TSBool with_timestamp,
                            TSBool with_linebreak,
                            TSBool with_eventrank, const TSChar * data)
{
        return;
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
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
                                 TSLoggerType loggertype,
                                 TSBool overwrite,
                                 TSBool withtimestamp,
                                 TSBool withlinebreak,
                                 TSBool witheventranking,
                                 TSBool pididtologfile,
                                 TSBool createlogdir,
                                 unsigned int buffersize, TSBool unicode)
{
        MinLoggerFileOutput *retval = INITPTR;
        unsigned int    flen = 0;

        /* Some errorchecking */
        if (path == INITPTR) {
                goto EXIT;
        }
        if (file == INITPTR) {
                goto EXIT;
        }
        flen = strlen (file);
        if (flen == 0) {
                goto EXIT;
        }

        /* Attach pointers */
        retval = NEW (MinLoggerFileOutput);
        retval->write_ = fo_write;
        retval->destroy_ = fo_destroy;

        /* Initialize variables */
        retval->loggertype_ = loggertype;
        retval->overwrite_ = overwrite;
        retval->withtimestamp_ = withtimestamp;
        retval->withlinebreak_ = withlinebreak;
        retval->witheventranking_ = witheventranking;
        retval->pididtologfile_ = pididtologfile;
        retval->createlogdir_ = createlogdir;
        retval->buffersize_ = buffersize;
        retval->unicode_ = unicode;
        retval->isfileopen_ = 0;
        retval->databuf_ = INITPTR;

        /** Perform the init phase */
        if (retval->createlogdir_)
                fo_create_directory (retval, path);

        if (retval->overwrite_)
                fo_create_new_file (retval, path, file);
        else
                fo_open_existing_file (retval, path, file);

        if (retval->buffersize_ > 0)
                retval->databuf_ = NEW2 (TSChar, retval->buffersize_);
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Destroys MIN FileOutput instance.
 *  @param o [in:out] sfo instance to be destroyed.
 *   
 *  NOTE: after being freed the pointer is set to INITPTR;
 */
void fo_destroy (struct output_typeinfo_t **o)
{
        struct min_logger_file_output_t *sfo = INITPTR;

        if (*o == INITPTR) {
                goto EXIT;
        }

        sfo = (struct min_logger_file_output_t *)(*o);

        if (sfo->buffersize_ > 0) {
                DELETE (sfo->databuf_);
        }
        DELETE (sfo->fileanddirname_);
        DELETE (*o);
        *o = INITPTR;
      EXIT:
        return;
}

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
                                 TSLoggerType loggertype,
                                 TSBool overwrite,
                                 TSBool withtimestamp,
                                 TSBool withlinebreak,
                                 TSBool witheventranking,
                                 TSBool pididtologfile,
                                 TSBool createlogdir,
                                 unsigned int buffersize, TSBool unicode)
{
        MinLoggerNullOutput *retval = INITPTR;

        /* No errorchecking because there is no need for that,
         * NullOutput is not doing anything. */

        /* Attach pointers */
        retval = NEW (MinLoggerNullOutput);
        retval->write_ = no_write;
        retval->destroy_ = no_destroy;

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Destroys MIN NullOutput instance.
 *  @param o [in:out] sfo instance to be destroyed.
 *   
 *  NOTE: after being freed the pointer is set to INITPTR;
 */
void no_destroy (struct output_typeinfo_t **o)
{
        if (*o == INITPTR) {
                goto EXIT;
        }
        DELETE (*o);
        *o = INITPTR;
      EXIT:
        return;
}

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
                                   unsigned int buffersize, TSBool unicode)
{
        MinLoggerSyslogOutput *retval = INITPTR;

        if (file == INITPTR) {
                goto EXIT;
        }

        /* Attach pointers */
        retval = NEW (MinLoggerSyslogOutput);
        retval->write_ = so_write;
        retval->destroy_ = so_destroy;

        /* Initialize variables */
        retval->unicode_ = unicode;

        /** Perform the init phase */
        openlog (file, LOG_PID | LOG_CONS, LOG_LOCAL0);
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Writes message through the syslog output. 
 *  @param so [in] syslog output plugin used for writing. 
 *  @param with_timestamp [in] flag indicates presence of timestamp.
 *  @param with_linebreak [in] flag indicating presence of linebreak. 
 *  @param with_eventrank [in] on/off for event ranking feature.
 *  @param data [in] the data to be written. */
LOCAL void so_write (struct output_typeinfo_t *so,
                     TSBool with_timestamp,
                     TSBool with_linebreak,
                     TSBool with_eventrank, const TSChar * data)
{
        MinLoggerSyslogOutput *sxso = INITPTR;

        if (so == INITPTR) {
                goto EXIT;
        }
        if (data == INITPTR) {
                goto EXIT;
        }

        sxso = (MinLoggerSyslogOutput *) so;

        /* Write data to the file */
        if (sxso->unicode_ == ESTrue) {
                /* Not supported yet */
        } else {
                /* Normal syslog writing */
                syslog (LOG_INFO,"%s", data);
        }
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
/** Destroys MIN Syslog Output instance.
 *  @param o [in:out] sfo instance to be destroyed.
 *   
 *  NOTE: after being freed the pointer is set to INITPTR;
 */
void so_destroy (struct output_typeinfo_t **o)
{
        if (*o == INITPTR) {
                goto EXIT;
        }

        closelog ();
        DELETE (*o);
        *o = INITPTR;
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "min_logger_output.tests"
#endif                          /* MIN_UNIT_TEST */
/* End of file */
