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
 *  @file       min_logger.c
 *  @version    0.1
 *  @brief      This file contains implementation of MIN logger API
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <limits.h>

#include <min_logger.h>
#include <min_logger_common.h>
/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
extern MinLogger *__logger__;
extern Text *__component_name__;
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
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
LOCAL void      mnl_overwrite_settings (struct logger_settings_t *ls,
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
LOCAL MinTxtLogger *stl_create (const TSChar * path, const TSChar * file,
				TSLoggerType loggertype, unsigned int output,
				TSBool overwrite, TSBool withtimestamp,
				TSBool withlinebreak,
				TSBool witheventranking,
				TSBool pididtologfile, TSBool createlogdir,
				unsigned int staticbuffersize,
				TSBool unicode);
/* ------------------------------------------------------------------------- */
LOCAL void      stl_destroy (MinTxtLogger ** stl);
/* ------------------------------------------------------------------------- */
LOCAL int       stl_send (struct logger_typeinfo_t *lt, int style,
                          const TSChar * msg);
/* ------------------------------------------------------------------------- */
LOCAL MinHtmlLogger *mhl_create (const TSChar * path, const TSChar * file,
				 TSLoggerType loggertype,
				 unsigned int output, TSBool overwrite,
				 TSBool withtimestamp, TSBool withlinebreak,
				 TSBool witheventranking,
				 TSBool pididtologfile, TSBool createlogdir,
				 unsigned int staticbuffersize,
				 TSBool unicode);
/* ------------------------------------------------------------------------- */
LOCAL void      mhl_destroy (MinHtmlLogger ** shl);
/* ------------------------------------------------------------------------- */
LOCAL int       mhl_send (struct logger_typeinfo_t *lt, int style,
                          const TSChar * msg);
/* ------------------------------------------------------------------------- */
LOCAL void      mhl_start_html_page (struct output_typeinfo_t *outtype,
                                     const TSChar * file,
                                     TSBool pididtologfile);
/* ------------------------------------------------------------------------- */
LOCAL void      mhl_end_html_page (struct output_typeinfo_t *outtype);
/* ------------------------------------------------------------------------- */
LOCAL void      mhl_style_starting (struct output_typeinfo_t *outtype,
                                    int style, TSBool * timestamp,
                                    TSBool * eventranking);
/* ------------------------------------------------------------------------- */
LOCAL void      mhl_style_ending (struct output_typeinfo_t *outtype,
                                  int style);
/* ------------------------------------------------------------------------- */
LOCAL MinDataLogger *mdl_create (const TSChar * path, const TSChar * file,
                                  TSLoggerType loggertype,
                                  unsigned int output, TSBool overwrite,
                                  TSBool withtimestamp, TSBool withlinebreak,
                                  TSBool witheventranking,
                                  TSBool pididtologfile, TSBool createlogdir,
                                  unsigned int staticbuffersize,
                                  TSBool unicode);
/* ------------------------------------------------------------------------- */
LOCAL void      mdl_destroy (MinDataLogger ** sdl);
/* ------------------------------------------------------------------------- */
LOCAL int       mdl_send (struct logger_typeinfo_t *lt, int style,
                          const TSChar * msg);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
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
LOCAL void mnl_overwrite_settings (struct logger_settings_t *ls,
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

/* ------------------------------------------------------------------------- */
/** Creates a new MIN text logger instance.
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
 *  @param staticbuffersize [in] size of the static buffer.
 *  @param unicode [in] unicode flag.
 *  @return pointer to logger instance.
 */
LOCAL MinTxtLogger *stl_create (const TSChar * path, 
				const TSChar * file,
				TSLoggerType loggertype, 
				unsigned int output,
				TSBool overwrite, 
				TSBool withtimestamp,
				TSBool withlinebreak,
				TSBool witheventranking,
				TSBool pididtologfile, 
				TSBool createlogdir,
				unsigned int staticbuffersize,
				TSBool unicode)
{
        MinTxtLogger  *retval = NEW (MinTxtLogger);
        struct output_typeinfo_t *out = INITPTR;

        retval->type_ = ESTxt;
        retval->send_ = stl_send;
        retval->output_ = dl_list_create ();

        if (output & ESFile) {
                /* create file output plugin */
                out = (struct output_typeinfo_t *)
                    fo_create (MinOutputPluginParams);
                dl_list_add (retval->output_, (void *)out);
        }
        if (output & ESSyslog) {
                /* create syslog output plugin */
                out = (struct output_typeinfo_t *)
                    so_create (MinOutputPluginParams);
                dl_list_add (retval->output_, (void *)out);
        }
        if (output & ESNull || (dl_list_size (retval->output_) == 0)) {
                /* create null output plugin */
                /* it never fails! */
                out = (struct output_typeinfo_t *)
                    no_create (MinOutputPluginParams);
                dl_list_add (retval->output_, (void *)out);
        }
        if (output & ESStdout) {
                /* create sdtout output plugin */
                out = (struct output_typeinfo_t *)
                    stdo_create (MinOutputPluginParams);
                dl_list_add (retval->output_, (void *)out);
        }
        if (output & ESStderr) {
                /* create sdtout output plugin */
                out = (struct output_typeinfo_t *)
                    stde_create (MinOutputPluginParams);
                dl_list_add (retval->output_, (void *)out);
        }


        return retval;
}

/* ------------------------------------------------------------------------- */
/** Destroys MIN Txt logger instance.
 *  @param stl address of pointer to txt logger instance.
 */
LOCAL void stl_destroy (MinTxtLogger ** stl)
{
        DLListIterator  it = DLListNULLIterator;
        struct output_typeinfo_t *o = INITPTR;

        if (*stl == INITPTR) {
                goto EXIT;
        }

        /* destroy all output plugins */
        it = dl_list_head ((*stl)->output_);
        while (it != DLListNULLIterator) {
                o = (struct output_typeinfo_t *)dl_list_data (it);
                o->destroy_ (&o);
                dl_list_remove_it (it);
                it = dl_list_head ((*stl)->output_);
        }
        dl_list_free (&(*stl)->output_);
        DELETE (*stl);
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
/** Send log message through txt logger output plugin(s).
 *  @param lt holds logger type and plugins
 *  @param style logging style option
 *  @param msg log message
 *  @return ENOERR on success, -1 on error. 
 */
LOCAL int stl_send (struct logger_typeinfo_t *lt, int style,
                    const TSChar * msg)
{
        int retval = ENOERR;
        MinTxtLogger *stl = INITPTR;
        DLListIterator it = DLListNULLIterator;
        struct output_typeinfo_t *outtype = INITPTR;
        Text *message = INITPTR;
        unsigned int loglevel = style & 0xFFF80;
   
	if (lt == INITPTR) { retval = -1; goto EXIT; }
        if (msg == INITPTR) { retval = -1; goto EXIT; }
        if (loglevel == 0x0) { loglevel = ESInfo; }
        message = tx_create("");
        if( loglevel & ESFatal ) { tx_c_append(message,"FATAL"); }
        else if( loglevel & ESError ){ tx_c_append(message,"ERROR"); }
        else if( loglevel & ESWarning ){ tx_c_append(message,"WARNING"); }
        else if( loglevel & ESInfo ){ tx_c_append(message,"INFO"); }
        else if( loglevel & ESNotice ){ tx_c_append(message,"NOTICE"); }
        else if( loglevel & ESDebug ){ tx_c_append(message,"DEBUG"); }
        else if( loglevel & ESTrace ){ tx_c_append(message,"TRACE"); }
        tx_c_append(message,msg);
        stl = (MinTxtLogger *) lt;
        /* Log message through all of outputs */
        it = dl_list_head (stl->output_);
        while (it != DLListNULLIterator) {
                outtype = (struct output_typeinfo_t *)dl_list_data (it);
                outtype->write_ (outtype, ESTrue, ESTrue, ESTrue,
                                 tx_share_buf(message));
                it = dl_list_next (it);
        }
        /* Clean up */
        tx_destroy(&message);
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Creates a new MIN html logger instance.
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
 *  @param staticbuffersize [in] size of the static buffer.
 *  @param unicode [in] unicode flag.
 *  @return pointer to logger instance.
 */
LOCAL MinHtmlLogger *mhl_create (const TSChar * path, 
				 const TSChar * file,
				 TSLoggerType loggertype,
				 unsigned int output, 
				 TSBool overwrite,
				 TSBool withtimestamp, 
				 TSBool withlinebreak,
				 TSBool witheventranking,
				 TSBool pididtologfile, 
				 TSBool createlogdir,
				 unsigned int staticbuffersize,
				 TSBool unicode)
{
        MinHtmlLogger *retval = NEW (MinHtmlLogger);
        struct output_typeinfo_t *out = INITPTR;

        retval->type_ = ESHtml;
        retval->send_ = mhl_send;
        retval->output_ = dl_list_create ();

        if (output & ESFile) {
                /* create file output plugin */
                out = (struct output_typeinfo_t *)
                    fo_create (MinOutputPluginParams);
                dl_list_add (retval->output_, (void *)out);
                mhl_start_html_page (out, file, pididtologfile);
        }
        if (output & ESSyslog) {
                /* create syslog output plugin */
                out = (struct output_typeinfo_t *)
                    so_create (MinOutputPluginParams);
                dl_list_add (retval->output_, (void *)out);
                mhl_start_html_page (out, file, pididtologfile);
        }
        if (output & ESNull || (dl_list_size (retval->output_) == 0)) {
                /* create null output plugin */
                /* it never fails! */
                out = (struct output_typeinfo_t *)
                    no_create (MinOutputPluginParams);
                dl_list_add (retval->output_, (void *)out);
        }

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Destroys MIN html logger instance.
 *  @param shl address of pointer to html logger instance.
 */
LOCAL void mhl_destroy (MinHtmlLogger ** shl)
{
        DLListIterator  it = DLListNULLIterator;
        struct output_typeinfo_t *o = INITPTR;

        if (*shl == INITPTR) {
                goto EXIT;
        }

        /* destroy all output plugins */
        it = dl_list_head ((*shl)->output_);
        while (it != DLListNULLIterator) {
                o = (struct output_typeinfo_t *)dl_list_data (it);
                mhl_end_html_page (o);
                o->destroy_ (&o);
                dl_list_remove_it (it);
                it = dl_list_head ((*shl)->output_);
        }

        DELETE (*shl);
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
/** Send log message through html logger output plugin(s).
 *  @param lt holds logger type and plugins
 *  @param style logging style option
 *  @param msg log message
 *  @return ENOERR on success, -1 on error. 
 */
LOCAL int mhl_send (struct logger_typeinfo_t *lt, int style,
                    const TSChar * msg)
{
        int             retval = ENOERR;
        MinHtmlLogger *shl = INITPTR;
        DLListIterator  it = DLListNULLIterator;
        struct output_typeinfo_t *outtype = INITPTR;
        TSBool          timestamp = ESTrue;
        TSBool          eventranking = ESTrue;

        if (lt == INITPTR) {
                retval = -1;
                goto EXIT;
        }
        if (msg == INITPTR) {
                retval = -1;
                goto EXIT;
        }

        shl = (MinHtmlLogger *) lt;

        /* Log message through all of outputs */
        it = dl_list_head (shl->output_);
        while (it != DLListNULLIterator) {
                outtype = (struct output_typeinfo_t *)dl_list_data (it);

                mhl_style_starting (outtype, style, &timestamp,
                                    &eventranking);
                outtype->write_ (outtype, timestamp, ESTrue, eventranking,
                                 msg);
                mhl_style_ending (outtype, style);
                it = dl_list_next (it);
        }
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Write html page start.
 *  @param outtype output plugin.
 *  @param file filename used to generate html title. 
 *  @param pididtologfile flag stating wether the pid should be appended to 
 *         output filename.   
 */
LOCAL void mhl_start_html_page (struct output_typeinfo_t *outtype,
                                const TSChar * file, TSBool pididtologfile)
{
        TSChar         *title = NEW2 (TSChar, MaxFileName + 1);

        /* If the title size is over the MaxFileName rest of the title
           will cut. */
        STRCPY (title, file, MaxFileName);

        /* Add process id to title if enough space and test file is given and 
           pididtologfile is true */
        if (strlen (title) < (MaxFileName - 5) && strlen (file) > 0 &&
            pididtologfile) {
                /* Appends id in hexadesimal format */
                sprintf (title, "%s_%x", title, getpid ());
        }

        /* Html character encoding */
        outtype->write_ (outtype, ESFalse, ESFalse, ESFalse,
                         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

        /* According to HTML standards, each HTML document requires a document
           type declaration */
        outtype->write_ (outtype, ESFalse, ESFalse, ESFalse,
                         "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0  \
                        Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/ \
                        xhtml1-strict.dtd\">\n");

        /* Html start tags to whole page and header section */
        outtype->write_ (outtype, ESFalse, ESFalse, ESFalse,
                         "<html xmlns=\"http://www.w3.org/1999/xhtml\"   \
                        xml:lang=\"en\" lang=\"en\">\n");

        /* Html header and title start tags */
        outtype->write_ (outtype, ESFalse, ESFalse, ESFalse,
                         "<head>\n<title>");

        /* Html file title */
        outtype->write_ (outtype, ESFalse, ESFalse, ESFalse, title);

        /* Html header and title end tags and start tag to body section
           Style tag defines user styles */
        outtype->write_ (outtype, ESFalse, ESFalse, ESFalse,
                         "</title>\n<style type=\"text/css\">\n          \
                        .green { color: #009900; }\n                    \
                        .red { color: #FF0000; }\n                      \
                        .blue { color: #3333FF; }\n                     \
                        .bold { font-weight: bold; }\n                  \
                        .cursive { font-style: italic; }\n              \
                        .underline { text-decoration: underline; }\n    \
                        </style>\n</head>\n<body>\n<p>\n");

        DELETE (title);
}

/* ------------------------------------------------------------------------- */
/** Write html page ending.
 *  @param outtype output plugin.
 */
LOCAL void mhl_end_html_page (struct output_typeinfo_t *outtype)
{
        /* Html page and body section end tags */
        outtype->write_ (outtype, ESFalse, ESFalse, ESFalse,
                         "\n</p>\n</body>\n</html>\n");
}

/* ------------------------------------------------------------------------- */
/** Write starting for html style. 
 *  @param outtype output plugin.
 *  @param style style flags
 *  @param timestamp flag stating whether time stamps are used.
 *  @param eventranking flag stating whether event ranking is used.
 */
LOCAL void mhl_style_starting (struct output_typeinfo_t *outtype, int style,
                               TSBool * timestamp, TSBool * eventranking)
{
        if (0x00020 <= style || /* ESRed, 0x00020 => 32 */
            0x00040 <= style || /* ESImportant, 0x00040 => 64 */
            0x00080 <= style || /* ESWarning, 0x00080 => 128 */
            0x00100 <= style) { /* ESError, 0x00100 => 256 */
                outtype->write_ (outtype, ESFalse, ESFalse, ESFalse,
                                 "<span class=\"red\">");
                if (0x00100 <= style) { /* ESError, 0x00100 >= 256 */
                        outtype->write_ (outtype, *timestamp, ESFalse,
                                         *eventranking, "ERROR...");
                        style -= 0x00100;
                        *timestamp = ESFalse;
                        *eventranking = ESFalse;
                }
                if (0x00080 <= style) { /* ESWarning, 0x00080 >= 128 */
                        outtype->write_ (outtype, *timestamp, ESFalse,
                                         *eventranking, "WARNING...");
                        style -= 0x00080;
                        *timestamp = ESFalse;
                        *eventranking = ESFalse;
                }
                if (0x00040 <= style) { /* ESImportant, 0x00040 >= 64 */
                        outtype->write_ (outtype, *timestamp, ESFalse,
                                         *eventranking, "IMPORTANT...");
                        style -= 0x00040;
                        *timestamp = ESFalse;
                        *eventranking = ESFalse;
                }
                if (0x00020 <= style) {
                        style -= 0x00020;
                }
        }
        /* ESGreen, 0x00010 => 16 */
        if (0x00010 <= style) {
                outtype->write_ (outtype, ESFalse, ESFalse, ESFalse,
                                 "<span class=\"green\">");
                style -= 0x00010;
        }
        /* ESBlue, 0x00008 => 8 */
        if (0x00008 <= style) {
                outtype->write_ (outtype, ESFalse, ESFalse, ESFalse,
                                 "<span class=\"blue\">");
                style -= 0x00008;
        }
        /* ESUnderline, 0x00004 => 4 */
        if (0x00004 <= style) {
                outtype->write_ (outtype, ESFalse, ESFalse, ESFalse,
                                 "<span class=\"underline\">");
                style -= 0x00004;
        }
        /* ESCursive, 0x00002 => 2 */
        if (0x00002 <= style) {
                outtype->write_ (outtype, ESFalse, ESFalse, ESFalse,
                                 "<span class=\"cursive\">");
                style -= 0x00002;
        }
        /* ESBold, 0x00001 => 1 */
        if (0x00001 <= style) {
                outtype->write_ (outtype, ESFalse, ESFalse, ESFalse,
                                 "<span class=\"bold\">");
                style -= 0x00001;
        }
}

/* ------------------------------------------------------------------------- */
/** Write ending for HTML style.
 *  @param outtype output plugin.
 *  @param style style flags.
 */
LOCAL void mhl_style_ending (struct output_typeinfo_t *outtype, int style)
{
        if (0x00020 <= style || /* ESRed, 0x00020 => 32 */
            0x00040 <= style || /* ESImportant, 0x00040 => 64 */
            0x00080 <= style || /* ESWarning, 0x00080 => 128 */
            0x00100 <= style) { /* ESError, 0x00100 => 256 */
                outtype->write_ (outtype, ESFalse, ESFalse, ESFalse,
                                 "</span>");
                if (0x00100 <= style) {
                        style -= 0x00100;
                }
                if (0x00080 <= style) {
                        style -= 0x00080;
                }
                if (0x00040 <= style) {
                        style -= 0x00040;
                }
                if (0x00020 <= style) {
                        style -= 0x00020;
                }
        }
        /* ESGreen, 0x00010 => 16 */
        if (0x00010 <= style) {
                outtype->write_ (outtype, ESFalse, ESFalse, ESFalse,
                                 "</span>");
                style -= 0x00010;
        }
        /* ESBlue, 0x00008 => 8 */
        if (0x00008 <= style) {
                outtype->write_ (outtype, ESFalse, ESFalse, ESFalse,
                                 "</span>");
                style -= 0x00008;
        }
        /* ESUnderline, 0x00004 => 4 */
        if (0x00004 <= style) {
                outtype->write_ (outtype, ESFalse, ESFalse, ESFalse,
                                 "</span>");
                style -= 0x00004;
        }
        /* ESCursive, 0x00002 => 2 */
        if (0x00002 <= style) {
                outtype->write_ (outtype, ESFalse, ESFalse, ESFalse,
                                 "</span>");
                style -= 0x00002;
        }
        /* EBold, 0x00001 => 1 */
        if (0x00001 <= style) {
                outtype->write_ (outtype, ESFalse, ESFalse, ESFalse,
                                 "</span>");
                style -= 0x00001;
        }
}

/* ------------------------------------------------------------------------- */
/** Creates a new MIN data logger instance.
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
 *  @param staticbuffersize [in] size of the static buffer.
 *  @param unicode [in] unicode flag.
 *  @return pointer to logger instance.
 */
LOCAL MinDataLogger *mdl_create (const TSChar * path, 
				 const TSChar * file,
				 TSLoggerType loggertype,
				 unsigned int output, 
				 TSBool overwrite,
				 TSBool withtimestamp, 
				 TSBool withlinebreak,
				 TSBool witheventranking,
				 TSBool pididtologfile, 
				 TSBool createlogdir,
				 unsigned int staticbuffersize,
				 TSBool unicode)
{
        MinDataLogger *retval = NEW (MinDataLogger);
        struct output_typeinfo_t *out = INITPTR;

        retval->type_ = ESData;
        retval->send_ = mdl_send;
        retval->output_ = dl_list_create ();

        /* Time stamp not allowed in data logging */
        withtimestamp = ESFalse;
        /* Line break not allowed in data logging */
        withlinebreak = ESFalse;
        /* Event ranking not allowed in data logging */
        witheventranking = ESFalse;
        /* For data type unicode is not allowed */
        unicode = ESFalse;

        if (output & ESFile) {
                /* create file output plugin */
                out = (struct output_typeinfo_t *)
                    fo_create (MinOutputPluginParams);
                dl_list_add (retval->output_, (void *)out);
        }
        if (output & ESSyslog) {
                /* create syslog output plugin */
                out = (struct output_typeinfo_t *)
                    so_create (MinOutputPluginParams);
                dl_list_add (retval->output_, (void *)out);
        }
        if (output & ESNull || (dl_list_size (retval->output_) == 0)) {
                /* create null output plugin */
                /* it never fails! */
                out = (struct output_typeinfo_t *)
                    no_create (MinOutputPluginParams);
                dl_list_add (retval->output_, (void *)out);
        }

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Destroys MIN data logger instance.
 *  @param shl address of pointer to data logger instance.
 */
LOCAL void mdl_destroy (MinDataLogger ** sdl)
{
        DLListIterator  it = DLListNULLIterator;
        struct output_typeinfo_t *o = INITPTR;

        if (*sdl == INITPTR) {
                goto EXIT;
        }

        /* destroy all output plugins */
        it = dl_list_head ((*sdl)->output_);
        while (it != DLListNULLIterator) {
                o = (struct output_typeinfo_t *)dl_list_data (it);
                o->destroy_ (&o);
                dl_list_remove_it (it);
                it = dl_list_head ((*sdl)->output_);
        }

        DELETE (*sdl);
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
/** Send log message through data logger output plugin(s).
 *  @param lt holds logger type and plugins
 *  @param style logging style option
 *  @param msg log message
 *  @return ENOERR on success, -1 on error. 
 */
LOCAL int mdl_send (struct logger_typeinfo_t *lt, int style,
                    const TSChar * msg)
{
        int             retval = ENOERR;
        MinDataLogger *sdl = INITPTR;
        DLListIterator  it = DLListNULLIterator;
        struct output_typeinfo_t *outtype = INITPTR;

        if (lt == INITPTR) {
                retval = -1;
                goto EXIT;
        }
        if (msg == INITPTR) {
                retval = -1;
                goto EXIT;
        }

        sdl = (MinDataLogger *) lt;

        /* Log message through all of outputs */
        it = dl_list_head (sdl->output_);
        while (it != DLListNULLIterator) {
                outtype = (struct output_typeinfo_t *)dl_list_data (it);
                outtype->write_ (outtype, ESFalse, ESTrue, ESFalse, msg);
                it = dl_list_next (it);
        }
      EXIT:
        return retval;
}

/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Creates MIN Logger 'object' 
 *  @param path [in] output directory.
 *  @param file [in] output file.
 *  @param loggertype [in] type of the logger that is in use.
 *  @param overwrite [in] overwrite file if exists flag.
 *  @param withtimestamp [in] add timestamp flag.
 *  @param withlinebreak [in] add linebreak flag.
 *  @param witheventranking [in] do event ranking flag.
 *  @param pididtologfile [in] process id to logfile flag.
 *  @param createlogdir [in] create output directory if not exists flag.
 *  @param staticbuffersize [in] size of the static buffer.
 *  @param unicode [in] unicode flag.
 *  @return new instance of MinLogger or INITPTR in case of failure.
 */
MinLogger     *mnl_create (const TSChar * path, 
			   const TSChar * file,
			   unsigned int loggertype, 
			   unsigned int output,
			   TSBool overwrite, 
			   TSBool withtimestamp,
			   TSBool withlinebreak, 
			   TSBool witheventranking,
			   TSBool pididtologfile, 
			   TSBool createlogdir,
			   unsigned int staticbuffersize, 
			   TSBool unicode)
{
        MinLogger     *retval = INITPTR;
        struct logger_typeinfo_t *endpoint = INITPTR;
        int             shmid = 0;
        void           *shmaddr = INITPTR;
        int             ret = ENOERR;
        unsigned int    logtypebackup = 0;
        struct logger_settings_t settings;
        TSChar         *c = INITPTR;
        TSChar          npath[PATH_MAX];
        TSChar          nfile[NAME_MAX];
        TSStyle         loglevel = ESInfo;

        memset (npath,'\0',PATH_MAX);
        memset (nfile,'\0',NAME_MAX);

        if (path == INITPTR) {
                goto EXIT;
        }
        if (file == INITPTR) {
                goto EXIT;
        }

        STRCPY (npath, path, PATH_MAX);
        STRCPY (nfile, file, NAME_MAX);

        /* NOTE unicode now not supported */
        unicode = ESFalse;

        /* 1. Here we should contact the settings system and get settings
         *    for logger. */
        shmid = sm_create ((int)'a', sizeof (struct logger_settings_t));
        if (shmid == -1) {
                MIN_WARN ("SHM segment not created");
                return retval;
        } else {
                shmaddr = sm_attach (shmid);
                if (shmaddr == INITPTR) {
                        MIN_WARN ("Could not attach to shared memory segment");
                }
                ret =
                    sm_read (shmaddr, &settings,
                             sizeof (struct logger_settings_t));
        }
        sm_detach (shmaddr);

        if (ret == ENOERR) {
                mnl_overwrite_settings (&settings, npath, nfile, &loggertype,
                                        &output, &overwrite, &withtimestamp,
                                        &withlinebreak, &witheventranking,
                                        &pididtologfile, &createlogdir,
                                        &unicode,&loglevel);
        } else {
                MIN_WARN ("Could not read from shared memory segment");
        }

        /* Because we are using multiple logging facilities we are skipping the
         * extension. That will cause logging facility to add its own extension
         */
        c = strrchr (nfile, '.');
        if (c != NULL)
                *c = '\0';

        /* 2. Create logger according to the settings. */
        retval = NEW (MinLogger);
        retval->endpoint_ = dl_list_create ();
        retval->loglevel_ = loglevel;

        /* 3. Create endpoints */
        if (loggertype & ESTxt) {
                /* Create TXT endpoint */
                logtypebackup = loggertype;
                loggertype = ESTxt;
                endpoint = (struct logger_typeinfo_t *)
                    stl_create (MinCreateLoggerParams);
                dl_list_add (retval->endpoint_, (void *)endpoint);
                loggertype = logtypebackup;
        }
        if (loggertype & ESData) {
                /* Create DATA endpoint */
                logtypebackup = loggertype;
                loggertype = ESData;
                endpoint = (struct logger_typeinfo_t *)
                    mdl_create (MinCreateLoggerParams);
                dl_list_add (retval->endpoint_, (void *)endpoint);
                loggertype = logtypebackup;
        }
        if (loggertype & ESHtml) {
                /* Create HTML endpoint */
                logtypebackup = loggertype;
                loggertype = ESHtml;
                endpoint = (struct logger_typeinfo_t *)
                    mhl_create (MinCreateLoggerParams);
                dl_list_add (retval->endpoint_, (void *)endpoint);
                loggertype = logtypebackup;
        }
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Destroys MIN Logger instance.
 *  @param mnl [in:out] logger instance to be destroyed.
 *   
 *  NOTE: after being freed the pointer is set to INITPTR;
 */
void mnl_destroy (MinLogger ** mnl)
{
        DLListIterator  it = INITPTR;
        struct logger_typeinfo_t *logtype = INITPTR;
        struct min_txt_logger_t *txtlog = INITPTR;
        struct min_html_logger_t *htmllog = INITPTR;
        struct min_data_logger_t *datalog = INITPTR;

        if (*mnl == INITPTR) {
                goto EXIT;
        }

        /* Clear endpoints */
        it = dl_list_head ((*mnl)->endpoint_);
        while (it != DLListNULLIterator) {
                /* free endpoint */
                logtype = (struct logger_typeinfo_t *)dl_list_data (it);

                /* call destroy method for each type */
                switch (logtype->type_) {
                case ESTxt:
                        txtlog = (struct min_txt_logger_t *)logtype;
                        stl_destroy (&txtlog);
                        break;
                case ESData:
                        datalog = (struct min_data_logger_t *)logtype;
                        mdl_destroy (&datalog);
                        break;
                case ESHtml:
                        htmllog = (struct min_html_logger_t *)logtype;
                        mhl_destroy (&htmllog);
                        break;
                default:
                        MIN_WARN ("%s:%s:%d - Unknown log type [%d]",
                                     __FILE__, __FUNCTION__, __LINE__,
                                     logtype->type_);
                }

                /* remove from list */
                dl_list_remove_it (it);
                it = dl_list_head ((*mnl)->endpoint_);
        }

        dl_list_free (&(*mnl)->endpoint_);
        DELETE (*mnl);
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
/** Logs message.
 *  @param mnl [in] adress of the MinLogger instance to be used.
 *  @param style [in] the style to be used.
 *  @param format [in] format of the message to be logged.
 *  @param ... [in] extra parameters, according to the msg format.
 *
 *  NOTE: see man printf
 */
int mnl_log (MinLogger * mnl, TSStyle style, const TSChar * format, ...)
{
        int             retval = ENOERR;
        DLListIterator  it = INITPTR;
        struct logger_typeinfo_t *logtype = INITPTR;
        char            msg[MaxLogMessage];
        memset (msg, '\0', MaxLogMessage);
        TLogLevel loglevel = style & 0xFFF80;
        if (loglevel==0x00000) { loglevel = ESInfo; }

        if (mnl == INITPTR) {
                retval = -1;
                goto EXIT;
        }
        if (format == INITPTR) {
                retval = -1;
                goto EXIT;
        }
        if (loglevel > mnl->loglevel_) {
                goto EXIT;
        }

        /* Process the extra arguments */
        va_list         vl;
        va_start (vl, format);
        vsnprintf (msg, MaxLogMessage-1, format, vl);
        va_end (vl);

        /* Log message through all of endpoints */
        it = dl_list_head (mnl->endpoint_);
        while (it != DLListNULLIterator) {
                logtype = (struct logger_typeinfo_t *)dl_list_data (it);
                logtype->send_ (logtype, style, msg);
                it = dl_list_next (it);

        }
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Varargs version of mnl_log()
 *  @param mnl [in] adress of the MinLogger instance to be used.
 *  @param style [in] the style to be used.
 *  @param format [in] format of the message to be logged.
 *  @param va_list [in] extra parameters, according to the msg format.
 *
 *  NOTE: see man printf
 */
int vmnl_log (MinLogger * mnl, TSStyle style, const TSChar * format, 
	      va_list vl)
{
        int             retval = ENOERR;
        DLListIterator  it = INITPTR;
        struct logger_typeinfo_t *logtype = INITPTR;
        char            msg[MaxLogMessage];
        memset (msg, '\0', MaxLogMessage);
        TLogLevel loglevel = style & 0xFFF80;
        if (loglevel==0x00000) { loglevel = ESInfo; }

        if (mnl == INITPTR) {
                retval = -1;
                goto EXIT;
        }
        if (format == INITPTR) {
                retval = -1;
                goto EXIT;
        }
        if (loglevel > mnl->loglevel_) {
                goto EXIT;
        }

        /* Process the extra arguments */
        vsnprintf (msg, MaxLogMessage-1, format, vl);

        /* Log message through all of endpoints */
        it = dl_list_head (mnl->endpoint_);
        while (it != DLListNULLIterator) {
                logtype = (struct logger_typeinfo_t *)dl_list_data (it);
                logtype->send_ (logtype, style, msg);
                it = dl_list_next (it);
        }
      EXIT:
        return retval;
}
/* ------------------------------------------------------------------------- */
/** Writes delimiter to the log.
 *  @param mnl [in] adress of the MinLogger instance to be used.
 *
 *  Writes sequence of 60 '#' to the log.
 */
void mnl_write_delimiter (MinLogger * mnl)
{
        char            delim[61];
        int             ret = ENOERR;

        if (mnl == INITPTR) {
                goto EXIT;
        }

        memset (delim, '#', 60);
        delim[60] = '\0';

        ret = mnl_log (mnl, ESNoStyle, "%s", delim);
        if (ret == -1) {
                MIN_WARN ("%s:%s:%d - Writing of delimiter failed",
                           __FILE__, __FUNCTION__, __LINE__);
        }
      EXIT:
        return;
}
/* ------------------------------------------------------------------------- */
/** Writes user defined delimiter to the log.
 *  @param mnl [in] adress of the MinLogger instance to be used.
 *  @param c [in] character used as a delmiter.
 *  @param t [in] nuber of character in the delimiter string.
 */
void mnl_write_own_delimiter (MinLogger * mnl, const TSChar c,
                              unsigned int t)
{
        TSChar         *delim = INITPTR;
        int             ret = ENOERR;
        if (mnl == INITPTR) {
                goto EXIT;
        }

        delim = NEW2 (TSChar, t + 1);
        memset (delim, c, t);
        delim[t] = '\0';

        ret = mnl_log (mnl, ESNoStyle, "%s", delim);
        if (ret == -1) {
                MIN_WARN ("%s:%s:%d - Writing of delimiter failed",
                           __FILE__, __FUNCTION__, __LINE__);
        }

        DELETE (delim);
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
/** Gives information which output plugins are used with logger.
 *  @param mnl [in] adress of the MinLogger instance to be used.
 *  @return ORed mask of output plugins types that were created for this logger
 */
unsigned int mnl_output_type (MinLogger * mnl)
{
        unsigned int    retval = 0;
        DLListIterator  it = DLListNULLIterator;
        DLListIterator  it2 = DLListNULLIterator;
        struct logger_typeinfo_t *lt = INITPTR;
        struct output_typeinfo_t *ot = INITPTR;

        if (mnl == INITPTR) {
                goto EXIT;
        }

        it = dl_list_head (mnl->endpoint_);
        while (it != DLListNULLIterator) {
                lt = (struct logger_typeinfo_t *)dl_list_data (it);
                it2 = dl_list_head (lt->output_);
                while (it2 != DLListNULLIterator) {
                        ot = (struct output_typeinfo_t *)dl_list_data (it2);
                        retval |= ot->output_type_;
                        it2 = dl_list_next (it2);
                }
                it = dl_list_next (it);
        }
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Gives information which logs are created.
 *  @param mnl [in] adress of the MinLogger instance to be used.
 *  @return ORed mask of log typed plugins types that were created.
 */
unsigned int mnl_logger_type (MinLogger * mnl)
{
        unsigned int    retval = 0;
        DLListIterator  it = DLListNULLIterator;
        struct logger_typeinfo_t *lt = INITPTR;

        if (mnl == INITPTR) {
                goto EXIT;
        }

        it = dl_list_head (mnl->endpoint_);
        while (it != DLListNULLIterator) {
                lt = (struct logger_typeinfo_t *)dl_list_data (it);
                retval |= lt->type_;
                it = dl_list_next (it);
        }
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Getter for the logger instance.
 *  @return global logger.
 */
MinLogger *mnl_get_logger_instance()
{
	return __logger__;
}
/* ------------------------------------------------------------------------- */
/** Return the global logging component name.
 *  @return component name or INIT_PTR
 */
char *mnl_get_component_name()
{
	if (__component_name__==INITPTR) { return INITPTR; }
	else { return tx_share_buf(__component_name__); }
}
/* ------------------------------------------------------------------------- */
/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "min_logger.tests"
#endif                          /* MIN_UNIT_TEST */
/* End of file */
