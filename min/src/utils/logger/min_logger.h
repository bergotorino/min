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
 *  @file       min_logger.h
 *  @version    0.1
 *  @brief      Header for MIN Logger API
 */

#ifndef MIN_LOGGER_H
#define MIN_LOGGER_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <stdarg.h>
#include <stdio.h>
#include <min_logger_common.h>
#include <min_system_logger.h>
#include <min_logger_output.h>
#include <min_ipc_mechanism.h>
/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* ------------------------------------------------------------------------- */
#define MinLoggerDefaultValues ESTxt,ESFile,1,1,1,0,1,0,0,0
#define MinOutputPluginParams path,file,loggertype,overwrite,withtimestamp,\
withlinebreak,witheventranking,pididtologfile,createlogdir,staticbuffersize,\
unicode
#define MinCreateLoggerParams npath,nfile,loggertype,output,overwrite,\
withtimestamp,withlinebreak,witheventranking,pididtologfile,createlogdir,\
staticbuffersize,unicode
/* ------------------------------------------------------------------------- */
#define MIN_FATAL(__format__,__args__ ...)\
do{\
        Text *txt = tx_create(" ");\
	MinLogger *logger = mnl_get_logger_instance();\
	if (logger!=INITPTR) {\
		tx_c_append(txt,"   ");\
                tx_c_append(txt,mnl_get_component_name());\
		tx_c_append(txt," ");\
		tx_int_append(txt,"06",getpid());\
		tx_c_append(txt," ");\
		tx_c_append(txt,__FILE__);\
		tx_c_append(txt,":");\
		tx_int_append(txt,"04",__LINE__);\
		tx_c_append(txt,":");\
		tx_c_append(txt,__FUNCTION__);\
		tx_c_append(txt,"() ");\
		tx_c_append(txt,__format__);\
		mnl_log(logger,ESFatal,tx_share_buf(txt),##__args__);\
	}\
        tx_destroy(&txt);\
} while (0)

#define MIN_ERROR(__format__,__args__ ...)\
do{\
        Text *txt = tx_create(" ");\
	MinLogger *logger = mnl_get_logger_instance();\
	if (logger!=INITPTR) {\
		tx_c_append(txt,"   ");\
                tx_c_append(txt,mnl_get_component_name());\
		tx_c_append(txt," ");\
		tx_int_append(txt,"06",getpid());\
		tx_c_append(txt," ");\
		tx_c_append(txt,__FILE__);\
		tx_c_append(txt,":");\
		tx_int_append(txt,"04",__LINE__);\
		tx_c_append(txt,":");\
		tx_c_append(txt,__FUNCTION__);\
		tx_c_append(txt,"() ");\
		tx_c_append(txt,__format__);\
		mnl_log(logger,ESError,tx_share_buf(txt),##__args__);\
	}\
        tx_destroy(&txt);\
} while (0)

#define MIN_WARN(__format__,__args__ ...)\
do{\
        Text *txt = tx_create(" ");\
	MinLogger *logger = mnl_get_logger_instance();\
	if (logger!=INITPTR) {\
		tx_c_append(txt," ");\
                tx_c_append(txt,mnl_get_component_name());\
		tx_c_append(txt," ");\
		tx_int_append(txt,"06",getpid());\
		tx_c_append(txt," ");\
		tx_c_append(txt,__FILE__);\
		tx_c_append(txt,":");\
		tx_int_append(txt,"04",__LINE__);\
		tx_c_append(txt,":");\
		tx_c_append(txt,__FUNCTION__);\
		tx_c_append(txt,"() ");\
		tx_c_append(txt,__format__);\
		mnl_log(logger,ESWarning,tx_share_buf(txt),##__args__);\
	}\
        tx_destroy(&txt);\
} while (0)

#define MIN_INFO(__format__,__args__ ...)\
do{\
        Text *txt = tx_create(" ");\
	MinLogger *logger = mnl_get_logger_instance();\
	if (logger!=INITPTR) {\
		tx_c_append(txt,"    ");\
                tx_c_append(txt,mnl_get_component_name());\
		tx_c_append(txt," ");\
		tx_int_append(txt,"06",getpid());\
		tx_c_append(txt," ");\
		tx_c_append(txt,__FILE__);\
		tx_c_append(txt,":");\
		tx_int_append(txt,"04",__LINE__);\
		tx_c_append(txt,":");\
		tx_c_append(txt,__FUNCTION__);\
		tx_c_append(txt,"() ");\
		tx_c_append(txt,__format__);\
		mnl_log(logger,ESInfo,tx_share_buf(txt),##__args__);\
	}\
        tx_destroy(&txt);\
} while (0)

#define MIN_NOTICE(__format__,__args__ ...)\
do{\
        Text *txt = tx_create(" ");\
	MinLogger *logger = mnl_get_logger_instance();\
	if (logger!=INITPTR) {\
		tx_c_append(txt,"  ");\
                tx_c_append(txt,mnl_get_component_name());\
		tx_c_append(txt," ");\
		tx_int_append(txt,"06",getpid());\
		tx_c_append(txt," ");\
		tx_c_append(txt,__FILE__);\
		tx_c_append(txt,":");\
		tx_int_append(txt,"04",__LINE__);\
		tx_c_append(txt,":");\
		tx_c_append(txt,__FUNCTION__);\
		tx_c_append(txt,"() ");\
		tx_c_append(txt,__format__);\
		mnl_log(logger,ESNotice,tx_share_buf(txt),##__args__);\
	}\
        tx_destroy(&txt);\
} while (0)

#define MIN_DEBUG(__format__,__args__ ...)\
do{\
        Text *txt = tx_create(" ");\
	MinLogger *logger = mnl_get_logger_instance();\
	if (logger!=INITPTR) {\
		tx_c_append(txt,"   ");\
                tx_c_append(txt,mnl_get_component_name());\
		tx_c_append(txt," ");\
		tx_int_append(txt,"06",getpid());\
		tx_c_append(txt," ");\
		tx_c_append(txt,__FILE__);\
		tx_c_append(txt,":");\
		tx_int_append(txt,"04",__LINE__);\
		tx_c_append(txt,":");\
		tx_c_append(txt,__FUNCTION__);\
		tx_c_append(txt,"() ");\
		tx_c_append(txt,__format__);\
		mnl_log(logger,ESDebug,tx_share_buf(txt),##__args__);\
	}\
        tx_destroy(&txt);\
} while (0)

#define MIN_TRACE(__format__,__args__ ...)\
do{\
        Text *txt = tx_create(" ");\
	MinLogger *logger = mnl_get_logger_instance();\
	if (logger!=INITPTR) {\
		tx_c_append(txt,"   ");\
                tx_c_append(txt,mnl_get_component_name());\
		tx_c_append(txt," ");\
		tx_int_append(txt,"06",getpid());\
		tx_c_append(txt," ");\
		tx_c_append(txt,__FILE__);\
		tx_c_append(txt,":");\
		tx_int_append(txt,"04",__LINE__);\
		tx_c_append(txt,":");\
		tx_c_append(txt,__FUNCTION__);\
		tx_c_append(txt,"() ");\
		tx_c_append(txt,__format__);\
		mnl_log(logger,ESTrace,tx_share_buf(txt),##__args__);\
	}\
        tx_destroy(&txt);\
} while (0)
/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* ------------------------------------------------------------------------- */
typedef struct min_logger_t MinLogger;
typedef struct min_txt_logger_t MinTxtLogger;
typedef struct min_html_logger_t MinHtmlLogger;
typedef struct min_data_logger_t MinDataLogger;
typedef int     (*ptr2send) (struct logger_typeinfo_t *, int, const TSChar *);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* Structures */
/* ------------------------------------------------------------------------- */
struct min_logger_t {
        /** Keeps pointers to the endpoints. */
        DLList         *endpoint_;

        /** LogLevel for current logger. */
        TLogLevel       loglevel_;
};
/* ------------------------------------------------------------------------- */
struct logger_typeinfo_t {
        /** Type of the logger used. */
        TSLoggerType    type_;

        /** Pointer to send function */
        ptr2send        send_;

        /** Output plugins that are in use */
        DLList         *output_;
};
/* ------------------------------------------------------------------------- */
struct min_txt_logger_t {
        /** Things that we have to have because this is 'derrived' from
         *  logger_typeinfo_t @{*/

        /** Type of the output plugin. */
        TSLoggerType    type_;

        /** Pointer to send function */
        ptr2send        send_;

        /** Output plugins that are in use */
        DLList         *output_;

        /**@}*/
};
/* ------------------------------------------------------------------------- */
struct min_html_logger_t {
        /** Things that we have to have because this is 'derrived' from
         *  logger_typeinfo_t @{*/

        /** Type of the output plugin. */
        TSLoggerType    type_;

        /** Pointer to send function */
        ptr2send        send_;

        /** Output plugins that are in use */
        DLList         *output_;

        /**@}*/
};
/* ------------------------------------------------------------------------- */
struct min_data_logger_t {
        /** Things that we have to have because this is 'derrived' from
         *  logger_typeinfo_t @{*/

        /** Type of the output plugin. */
        TSLoggerType    type_;

        /** Pointer to send function */
        ptr2send        send_;

        /** Output plugins that are in use */
        DLList         *output_;

        /**@}*/
};
/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
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
                            unsigned int staticbuffersize, TSBool unicode);
/* ------------------------------------------------------------------------- */
void            mnl_destroy (MinLogger ** mnl);
/* ------------------------------------------------------------------------- */
int             mnl_log (MinLogger * mnl,
                         TSStyle style, const TSChar * format, ...);
/* ------------------------------------------------------------------------- */
int             vmnl_log (MinLogger * mnl,
			  TSStyle style, const TSChar * format, va_list vl);
/* ------------------------------------------------------------------------- */
void            mnl_write_delimiter (MinLogger * mnl);
/* ------------------------------------------------------------------------- */
void            mnl_write_own_delimiter (MinLogger * mnl,
                                         const TSChar c, unsigned int t);
/* ------------------------------------------------------------------------- */
unsigned int    mnl_output_type (MinLogger * mnl);
/* ------------------------------------------------------------------------- */
unsigned int    mnl_logger_type (MinLogger * mnl);
/* ------------------------------------------------------------------------- */
MinLogger      *mnl_get_logger_instance();
/* ------------------------------------------------------------------------- */
char           *mnl_get_component_name();
/* ------------------------------------------------------------------------- */
#endif                          /* MIN_LOGGER_H */
