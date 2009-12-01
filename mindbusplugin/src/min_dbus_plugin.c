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
 *  @file       min_dbus_plugin.c
 *  @version    0.1
 *  @brief      This file contains implementation of the MIN DBus plugin
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <glib.h>
#include <dbus/dbus-glib.h>
#include <stdlib.h> /* exit, EXIT_FAILURE */
#include <string.h> /*memcpy*/
#include "min_plugin_interface.h"
#include "min_dbus_plugin_common.h"

/* We must have prototypes before we include generated server stub. */
GType min_object_get_type(void);

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
GMainLoop *mainloop = NULL;

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* -------------------------------------------------------------------------- */
#define MIN_TYPE_OBJECT              (min_object_get_type())
#define MIN_OBJECT(object) \
        (G_TYPE_CHECK_INSTANCE_CAST ((object), \
         MIN_TYPE_OBJECT, MinObject))
#define MIN_OBJECT_CLASS(klass) \
        (G_TYPE_CHECK_CLASS_CAST ((klass), \
         MIN_TYPE_OBJECT, MinObjectClass))
#define MIN_IS_OBJECT(object) \
        (G_TYPE_CHECK_INSTANCE_TYPE ((object), \
         MIN_TYPE_OBJECT))
#define MIN_IS_OBJECT_CLASS(klass) \
        (G_TYPE_CHECK_CLASS_TYPE ((klass), \
         MIN_TYPE_OBJECT))
#define MIN_OBJECT_GET_CLASS(obj) \
        (G_TYPE_INSTANCE_GET_CLASS ((obj), \
         MIN_TYPE_OBJECT, MinObjectClass))
/* -------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
eapiIn_t out_clbk;
eapiOut_t min_clbk;
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* ------------------------------------------------------------------------- */
typedef enum {
        E_SIGNAL_NEW_MODULE,
        E_SIGNAL_NO_MODULE,
        E_SIGNAL_MODULE_READY,
        E_SIGNAL_NEW_CASE,
        E_SIGNAL_CASE_STARTED,
        E_SIGNAL_CASE_PAUSED,
        E_SIGNAL_CASE_RESUMED,
        E_SIGNAL_CASE_RESULT,
        E_SIGNAL_MSG_PRINT,
        E_SIGNAL_TEST_MODULES,
        E_SIGNAL_TEST_FILES,
	E_SIGNAL_ERROR_REPORT,
	E_SIGNAL_CASE_DESC,
        E_SIGNAL_COUNT        
} MinSignalId;

typedef struct {
       GObject parent;
} MinObject;

typedef struct {
        GObjectClass parent;
        guint signals[E_SIGNAL_COUNT];
} MinObjectClass;

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* ------------------------------------------------------------------------- */
gboolean min_object_min_add_test_module(MinObject *obj, gchar *modulepatch);
/* ------------------------------------------------------------------------- */
gboolean min_object_min_add_test_case_file(MinObject *obj,
					   gint moduleid,
					   gchar *testcasefile);
/* ------------------------------------------------------------------------- */
gboolean min_object_min_start_case(MinObject *obj,
				   gint moduleid,
				   gint caseid,
                                   gint groupid);
/* ------------------------------------------------------------------------- */
gboolean min_object_min_pause_case(MinObject *obj,
				   gint testrunid);
/* ------------------------------------------------------------------------- */
gboolean min_object_min_resume_case(MinObject *obj,
				    gint testrunid);
/* ------------------------------------------------------------------------- */
gboolean min_object_min_abort_case(MinObject *obj,
				   gint testrunid);
/* ------------------------------------------------------------------------- */
gboolean min_object_min_fatal_error(MinObject *obj,
				    const char *what,
				    const char *errorstring); 
/* ------------------------------------------------------------------------- */
gboolean min_object_min_open(MinObject *obj);
/* ------------------------------------------------------------------------- */
gboolean min_object_min_close(MinObject *obj);
/* ------------------------------------------------------------------------- */
gboolean min_object_min_query_test_modules(MinObject *obj);
/* ------------------------------------------------------------------------- */
gboolean min_object_min_query_test_files(MinObject *obj);
/* ------------------------------------------------------------------------- */
gboolean min_object_min_register_slave(MinObject *obj,
				       gchar *host,
				       gchar *slavetype);
/* ------------------------------------------------------------------------- */
G_DEFINE_TYPE(MinObject, min_object, G_TYPE_OBJECT);
/* ------------------------------------------------------------------------- */
#include "min_dbus_plugin.h"
MinObject *global_obj=NULL;
/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
static void min_object_init (MinObject *obj);
/* ------------------------------------------------------------------------- */
static void min_object_class_init (MinObjectClass *klass);
/* ------------------------------------------------------------------------- */
static void handle_error (const char* msg,const char* reason,gboolean fatal);
/* ------------------------------------------------------------------------- */
static void pl_case_result (long testrunid, int result, char *desc,
			     long starttime, long endtime);
/* ------------------------------------------------------------------------- */
static void pl_case_started (unsigned moduleid,
                        unsigned caseid,
                        long testrunid);
/* ------------------------------------------------------------------------- */
static void pl_case_paused (long testrunid);
/* ------------------------------------------------------------------------- */
static void pl_case_resumed (long testrunid);
/* ------------------------------------------------------------------------- */
static void pl_msg_print (long testrunid, char *message);
/* ------------------------------------------------------------------------- */
static void pl_new_module (char *modulename, unsigned moduleid);
/* ------------------------------------------------------------------------- */
static void pl_no_module (char *modulename);
/* ------------------------------------------------------------------------- */
static void pl_new_case (unsigned moduleid, unsigned caseid, char *casetitle);
/* ------------------------------------------------------------------------- */
static void pl_case_desc (unsigned moduleid, unsigned caseid, char *casedesc);
/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/** Initialize GLib object.
 */
static void min_object_init (MinObject *obj)
{
        /* Init GLib object, we do not need this currently. */
}
/* ------------------------------------------------------------------------- */
/** Initialize MINObject clas
 */
static void min_object_class_init (MinObjectClass *klass)
{
        /* Init our class object.*/

        /* 1. Register signals: */

        /* new_module */
        klass->signals[0] = g_signal_new (SIGNAL_NEW_MODULE,
					  G_OBJECT_CLASS_TYPE(klass),
					  G_SIGNAL_RUN_LAST,
					  0,NULL,NULL,
					  g_cclosure_marshal_VOID__STRING,
					  G_TYPE_NONE,2,G_TYPE_STRING,
					  G_TYPE_UINT);
        /* no_module */
        klass->signals[1] = g_signal_new (SIGNAL_NO_MODULE,
					  G_OBJECT_CLASS_TYPE(klass),
					  G_SIGNAL_RUN_LAST,
					  0,NULL,NULL,
					  g_cclosure_marshal_VOID__STRING,
					  G_TYPE_NONE,1,G_TYPE_STRING);
        /* module_ready */
        klass->signals[2] = g_signal_new (SIGNAL_MODULE_READY,
					  G_OBJECT_CLASS_TYPE(klass),
					  G_SIGNAL_RUN_LAST,
					  0,NULL,NULL,
					  g_cclosure_marshal_VOID__STRING,
					  G_TYPE_NONE,1,G_TYPE_UINT);
        /* new_case */
        klass->signals[3] = g_signal_new (SIGNAL_NEW_TEST_CASE,
					  G_OBJECT_CLASS_TYPE(klass),
					  G_SIGNAL_RUN_LAST,
					  0,NULL,NULL,
					  g_cclosure_marshal_VOID__STRING,
					  G_TYPE_NONE,3,
					  G_TYPE_UINT,G_TYPE_UINT,G_TYPE_STRING);
        /* case_started */
        klass->signals[4] = g_signal_new (SIGNAL_CASE_STARTED,
					  G_OBJECT_CLASS_TYPE(klass),
					  G_SIGNAL_RUN_LAST,
					  0,NULL,NULL,
					  g_cclosure_marshal_VOID__STRING,
					  G_TYPE_NONE,3,
					  G_TYPE_UINT,G_TYPE_UINT,G_TYPE_LONG);
        /* case_paused */
        klass->signals[5] = g_signal_new (SIGNAL_CASE_PAUSED,
					  G_OBJECT_CLASS_TYPE(klass),
					  G_SIGNAL_RUN_LAST,
					  0,NULL,NULL,
					  g_cclosure_marshal_VOID__STRING,
					  G_TYPE_NONE,1,
					  G_TYPE_LONG);
        /* case_resumed */
        klass->signals[6] = g_signal_new (SIGNAL_CASE_RESUMED,
					  G_OBJECT_CLASS_TYPE(klass),
					  G_SIGNAL_RUN_LAST,
					  0,NULL,NULL,
					  g_cclosure_marshal_VOID__STRING,
					  G_TYPE_NONE,1,
					  G_TYPE_LONG);
        /* case_result */
        klass->signals[7] = g_signal_new (SIGNAL_CASE_RESULT,
					  G_OBJECT_CLASS_TYPE(klass),
					  G_SIGNAL_RUN_LAST,
					  0,NULL,NULL,
					  g_cclosure_marshal_VOID__STRING,
					  G_TYPE_NONE,5,
					  G_TYPE_LONG,G_TYPE_INT,G_TYPE_STRING,
					  G_TYPE_LONG,G_TYPE_LONG);
        /* msg_print */
        klass->signals[8] = g_signal_new (SIGNAL_MSG_PRINT,
					  G_OBJECT_CLASS_TYPE(klass),
					  G_SIGNAL_RUN_LAST,
					  0,NULL,NULL,
					  g_cclosure_marshal_VOID__STRING,
					  G_TYPE_NONE,2,
					  G_TYPE_LONG,G_TYPE_STRING);
        /* test_modules */
        klass->signals[9] = g_signal_new (SIGNAL_TEST_MODULES,
					  G_OBJECT_CLASS_TYPE(klass),
					  G_SIGNAL_RUN_LAST,
					  0,NULL,NULL,
					  g_cclosure_marshal_VOID__STRING,
					  G_TYPE_NONE,1,
					  G_TYPE_STRING);
        /* test_files */
        klass->signals[10] = g_signal_new (SIGNAL_TEST_FILES,
					   G_OBJECT_CLASS_TYPE(klass),
					   G_SIGNAL_RUN_LAST,
					   0,NULL,NULL,
					   g_cclosure_marshal_VOID__STRING,
					   G_TYPE_NONE,1,
					   G_TYPE_STRING);
        /* error_report */
        klass->signals[11] = g_signal_new (SIGNAL_ERROR_REPORT,
					   G_OBJECT_CLASS_TYPE(klass),
					   G_SIGNAL_RUN_LAST,
					   0,NULL,NULL,
					   g_cclosure_marshal_VOID__STRING,
					   G_TYPE_NONE,1,G_TYPE_STRING);
        /* new_case */
        klass->signals[12] = g_signal_new (SIGNAL_CASE_DESC,
					   G_OBJECT_CLASS_TYPE(klass),
					   G_SIGNAL_RUN_LAST,
					   0,NULL,NULL,
					   g_cclosure_marshal_VOID__STRING,
					   G_TYPE_NONE,3,
					   G_TYPE_UINT,G_TYPE_UINT,
					   G_TYPE_STRING);

        dbus_g_object_type_install_info (MIN_TYPE_OBJECT,
                                        &dbus_glib_min_object_object_info);
}
/* ------------------------------------------------------------------------- */
/** calls fatal error callback from MIN Engine API */
static void handle_error(const char* msg,const char* reason,gboolean fatal)
{
        g_print(": ERROR: %s (%s)\n", msg, reason);
        if (fatal) {
                min_clbk.fatal_error (msg,reason);
        }
}
/* -------------------------------------------------------------------------- */
/** MIN Engine calls this when test case has finnished */ 
static void pl_case_result (long testrunid, int result, char *desc,
			    long starttime, long endtime)
{
        /* emit signal */
        if (!global_obj) return;
        MinObjectClass *klass = MIN_OBJECT_GET_CLASS(global_obj);
        g_signal_emit (global_obj,
                        klass->signals[E_SIGNAL_CASE_RESULT],
                        0,
                        testrunid,
                        result,
                        desc,
                        starttime,
                        endtime);
}
/* ------------------------------------------------------------------------- */
/** MIN Engine calls this when test case has started */
static void pl_case_started (unsigned moduleid,
			     unsigned caseid,
			     long testrunid)
{
        /* emit signal */
        if (!global_obj) return;
        MinObjectClass *klass = MIN_OBJECT_GET_CLASS(global_obj);
        g_signal_emit (global_obj,
                        klass->signals[E_SIGNAL_CASE_STARTED],
                        0,
                        moduleid,
                        caseid,
                        testrunid);
}
/* ------------------------------------------------------------------------- */
/** MIN Engine calls this when test case is paused.
 */
static void pl_case_paused (long testrunid)
{
        /* emit signal */
        if (!global_obj) return;
        MinObjectClass *klass = MIN_OBJECT_GET_CLASS(global_obj);
        g_signal_emit (global_obj,
                        klass->signals[E_SIGNAL_CASE_PAUSED],
                        0,
                        testrunid);
}
/* ------------------------------------------------------------------------- */
/** MIN Engine calls this when test case is resumed 
 */
static void pl_case_resumed (long testrunid)
{
        /* emit signal */
        if (!global_obj) return;
        MinObjectClass *klass = MIN_OBJECT_GET_CLASS(global_obj);
        g_signal_emit (global_obj,
                        klass->signals[E_SIGNAL_CASE_RESUMED],
                        0,
                        testrunid);
}
/* ------------------------------------------------------------------------- */
/** MIN Engine calls this for printouts coming from test cases
 */
static void pl_msg_print (long testrunid, char *message)
{
        /* emit signal */
        if (!global_obj) return;
        MinObjectClass *klass = MIN_OBJECT_GET_CLASS(global_obj);
        g_signal_emit (global_obj,
		       klass->signals[E_SIGNAL_MSG_PRINT],
		       0,
		       testrunid,message);

}
/* ------------------------------------------------------------------------- */
/** MIN Engine calls this for error reports from engine.
 */
static void pl_error_report (char *message)
{
        /* emit signal */
        if (!global_obj) return;
        MinObjectClass *klass = MIN_OBJECT_GET_CLASS(global_obj);
        g_signal_emit (global_obj,
		       klass->signals[E_SIGNAL_ERROR_REPORT],
		       0,
		       message);

}

/* ------------------------------------------------------------------------- */
/** MIN Engine calls this when new module has been succesfully added
 */
static void pl_new_module (char *modulename, unsigned moduleid)
{
        /* emit signal */
        if (!global_obj) return;
        MinObjectClass *klass = MIN_OBJECT_GET_CLASS(global_obj);
        g_signal_emit (global_obj,
                        klass->signals[E_SIGNAL_NEW_MODULE],
                        0,
                        modulename,moduleid);
}
/* ------------------------------------------------------------------------- */
/** MIN Engine calls this when module adding fails
 */
static void pl_no_module (char *modulename)
{
        /* emit signal */
        if (!global_obj) return;
        MinObjectClass *klass = MIN_OBJECT_GET_CLASS(global_obj);
        g_signal_emit (global_obj,
                        klass->signals[E_SIGNAL_NO_MODULE],
                        0,
                        modulename);
}
/* -------------------------------------------------------------------------- */
/** MIN Engine calls this for each new test case 
 */
static void pl_new_case (unsigned moduleid, unsigned caseid, char *casetitle)
{
        /* emit signal */
        if (!global_obj) return;
        MinObjectClass *klass = MIN_OBJECT_GET_CLASS(global_obj);
        g_signal_emit (global_obj,
                        klass->signals[E_SIGNAL_NEW_CASE],
                        0,
                        moduleid,caseid,casetitle);
}
/* -------------------------------------------------------------------------- */
/** MIN Engine calls this for each test case description
 */
static void pl_case_desc (unsigned moduleid, unsigned caseid, char *casedesc)
{
        /* emit signal */
        if (!global_obj) return;
        MinObjectClass *klass = MIN_OBJECT_GET_CLASS(global_obj);
        g_signal_emit (global_obj,
		       klass->signals[E_SIGNAL_CASE_DESC],
		       0,
		       moduleid,caseid,casedesc);
}
/* -------------------------------------------------------------------------- */
/** MIN Engine calls this when all test cases belonging to test module
 *  have been reported by pl_new_case
 */
static void pl_module_ready (unsigned moduleid)
{
        /* emit signal */
        if (!global_obj) return;
        MinObjectClass *klass = MIN_OBJECT_GET_CLASS(global_obj);
        g_signal_emit (global_obj,
                        klass->signals[E_SIGNAL_MODULE_READY],
                        0,
                        moduleid);
}
/* -------------------------------------------------------------------------- */
/** MIN engine calls this as a reply to query_test_modules.
 */
static void pl_test_modules (char *modules)
{
        /* emit signal */
        if (!global_obj) return;
        MinObjectClass *klass = MIN_OBJECT_GET_CLASS(global_obj);
        g_signal_emit (global_obj,
                        klass->signals[E_SIGNAL_TEST_MODULES],
                        0,
                        modules);
}
/* -------------------------------------------------------------------------- */
/** MIN engine calls this as a reply to query_test_files.
 */
static void pl_test_files (char *files)
{
        /* emit signal */
        if (!global_obj) return;
        MinObjectClass *klass = MIN_OBJECT_GET_CLASS(global_obj);
        g_signal_emit (global_obj,
                        klass->signals[E_SIGNAL_TEST_FILES],
                        0,
                        files);
}
/* -------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Attach dbus plugin to MIN Test Framework
 */
void pl_attach_plugin (eapiIn_t **out_callback, eapiOut_t *in_callback)
{
        /* Binds the callbacks */
        memcpy (&min_clbk,in_callback,sizeof(eapiOut_t));

        (*out_callback)->case_result            = pl_case_result;
        (*out_callback)->case_started           = pl_case_started;
        (*out_callback)->case_paused            = pl_case_paused;
        (*out_callback)->case_resumed           = pl_case_resumed;
        (*out_callback)->module_prints          = pl_msg_print;
        (*out_callback)->error_report           = pl_error_report;
        (*out_callback)->new_module             = pl_new_module;
        (*out_callback)->no_module              = pl_no_module;
        (*out_callback)->module_ready           = pl_module_ready;
        (*out_callback)->new_case               = pl_new_case;
        (*out_callback)->case_desc              = pl_case_desc;
        (*out_callback)->test_modules           = pl_test_modules;
        (*out_callback)->test_files             = pl_test_files;
}
/* ------------------------------------------------------------------------- */
/** Opens the plugin. Enters g_main_loop()
 */
void pl_open_plugin ()
{
        DBusGConnection *bus = NULL;
        DBusGProxy *busProxy = NULL;
        MinObject *minObj = NULL;
        guint result = 0;
        GError *error = NULL;
        gboolean tmp = FALSE;

        g_type_init();

        mainloop = g_main_loop_new (NULL,FALSE);
        if (!mainloop) {
                handle_error ("Could not create GMainLoop","OOM?",TRUE);
        }

        bus = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
        if (error) {
                handle_error("Couldn't connect to session bus",
                        error->message,
                        TRUE);

        }

        busProxy = dbus_g_proxy_new_for_name(bus,
                                        DBUS_SERVICE_DBUS,
                                        DBUS_PATH_DBUS,
                                        DBUS_INTERFACE_DBUS);
        if (!busProxy) {
                handle_error("Failed to get a proxy for D-Bus",
                        "Unknown(dbus_g_proxy_new_for_name)",TRUE);

        }

        tmp = dbus_g_proxy_call(busProxy,
                                "RequestName",
                                &error,
                                G_TYPE_STRING,
                                MIN_SERVICE_NAME,
                                G_TYPE_UINT,
                                0,
                                G_TYPE_INVALID,
                                G_TYPE_UINT,
                                &result,
                                G_TYPE_INVALID);
        if (!tmp) {
                handle_error("D-Bus.RequestName RPC failed",
                                error->message,
                                TRUE);
        }
        if (result != 1) {
                handle_error("Failed to get the primary well-known name.",
                        "RequestName result != 1", TRUE);
        }

        minObj = g_object_new(MIN_TYPE_OBJECT, NULL);
        if (!minObj) {
                handle_error("Failed to create one Min instance.",
                        "Unknown(OOM?)", TRUE);
        }
        global_obj = minObj;

        dbus_g_connection_register_g_object(bus,
                                        MIN_SERVICE_OBJECT_PATH,
                                        G_OBJECT(minObj));

        g_main_loop_run(mainloop);
}
/* ------------------------------------------------------------------------- */
/** Close plugin */
void pl_close_plugin ()
{
        /* Deinit DBus */
}
/* ------------------------------------------------------------------------- */
/** Detach plugin */
void pl_detach_plugin (eapiIn_t **out_callback, eapiOut_t *in_callback)
{
        /* Unbinds the callbacks */
}
/* ------------------------------------------------------------------------- */
/** Calls add_test_module from MIN Engine API */
gboolean min_object_min_add_test_module(MinObject *obj, gchar *modulepatch)
{
        /* Calls callback from MIN */
        if (min_clbk.add_test_module) {
                min_clbk.add_test_module (modulepatch);
                return TRUE;
        }
        return FALSE;
}
/* ------------------------------------------------------------------------- */
/** Calls add_test_case_file from MIN Engine API */
gboolean min_object_min_add_test_case_file(MinObject *obj,
                                        gint moduleid,
                                        gchar *testcasefile)
{
        /* Calls callback from MIN */
        if (min_clbk.add_test_case_file) {
                min_clbk.add_test_case_file (moduleid,testcasefile);
                return TRUE;
        }
        return FALSE;
}
/* ------------------------------------------------------------------------- */
/** Calls start_case from MIN Engine API */
gboolean min_object_min_start_case(MinObject *obj,
                                gint moduleid,
                                gint caseid,
                                gint groupid)
{
        /* Calls callback from MIN */
        if (min_clbk.start_case) {
                min_clbk.start_case (moduleid,caseid,0);
                return TRUE;
        }
        return FALSE;
}
/* ------------------------------------------------------------------------- */
/** Calls pause_case from MIN Engine API */
gboolean min_object_min_pause_case(MinObject *obj,
				   gint testrunid)
{
        /* Calls callback from MIN */
        if (min_clbk.pause_case) {
                min_clbk.pause_case (testrunid);
                return TRUE;
        }
        return FALSE;
}
/* ------------------------------------------------------------------------- */
/** Calls resume_case from MIN Engine API */
gboolean min_object_min_resume_case(MinObject *obj,
				    gint testrunid)
{
        /* Calls callback from MIN */
        if (min_clbk.resume_case) {
                min_clbk.resume_case (testrunid);
                return TRUE;
        }
        return FALSE;

}
/* ------------------------------------------------------------------------- */
/** Calls abort_case from MIN Engine API */
gboolean min_object_min_abort_case(MinObject *obj,
				   gint testrunid)
{
        /* Calls callback from MIN */
        if (min_clbk.abort_case) {
                min_clbk.abort_case (testrunid);
                return TRUE;
        }
        return FALSE;
}
/* ------------------------------------------------------------------------- */
/** Calls fatal_error from MIN Engine API */
gboolean min_object_min_fatal_error (MinObject *obj,
				     const char *what,
				     const char *errorstring)
{
        /* Calls callback from MIN */
        if (min_clbk.fatal_error) {
                min_clbk.fatal_error(what,errorstring);
                return TRUE;
        }
        return FALSE;
}
/* ------------------------------------------------------------------------- */
/** Calls min_open from MIN Engine API */
gboolean min_object_min_open(MinObject *obj)
{
        /* Calls callback from MIN */
        if (min_clbk.min_open) {
                min_clbk.min_open();
                return TRUE;
        }
        return FALSE;
}
/* ------------------------------------------------------------------------- */
/** Calls min_close from MIN Engine API */
gboolean min_object_min_close(MinObject *obj)
{
        /* Calls callback from MIN */
        if (min_clbk.min_close) {
                min_clbk.min_close();
		g_main_loop_quit (mainloop);
                return TRUE;
        }
        g_main_loop_quit (mainloop);

        return FALSE;
}
/* ------------------------------------------------------------------------- */
/** Calls query_test_modules from MIN Engine API */
gboolean min_object_min_query_test_modules(MinObject *obj)
{
        /* Calls callback from MIN */
        if (min_clbk.query_test_modules) {
                min_clbk.query_test_modules ();
                return TRUE;
        }
        return FALSE;
}
/* ------------------------------------------------------------------------- */
/** Calls query_test_files from MIN Engine API */
gboolean min_object_min_query_test_files(MinObject *obj)
{
        /* Calls callback from MIN */
        if (min_clbk.query_test_files) {
                min_clbk.query_test_files ();
                return TRUE;
        }
        return FALSE;
}
/* ------------------------------------------------------------------------- */
/** Calls register_slave from MIN Engine API */
gboolean min_object_min_register_slave(MinObject *obj,
				       gchar *host,
				       gchar *slavetype)
{
        /* Calls callback from MIN */
        if (min_clbk.register_slave) {
		min_clbk.register_slave (host, slavetype);
                return TRUE;
        }
        return FALSE;

}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */


/* ------------------------------------------------------------------------- */
/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#endif                          /* MIN_UNIT_TEST */
/* ------------------------------------------------------------------------- */
/* End of file */
