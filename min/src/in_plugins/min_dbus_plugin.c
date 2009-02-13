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
/* None */

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
typedef struct {
       GObject parent;
} MinObject;

typedef struct {
        GObjectClass parent;
} MinObjectClass;

gboolean min_object_min_add_test_module(MinObject *obj, gchar *modulepatch);
gboolean min_object_min_add_test_case_file(MinObject *obj,
                                        gint moduleid,
                                        gchar *testcasefile);
gboolean min_object_min_run_test(MinObject *obj,
                                gint moduleid,
                                gint caseid);
G_DEFINE_TYPE(MinObject, min_object, G_TYPE_OBJECT);

#include "min_dbus_plugin.h"
/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
static void min_object_init (MinObject *obj);
/* ------------------------------------------------------------------------- */
static void min_object_class_init (MinObjectClass *klass);
/* ------------------------------------------------------------------------- */
static void handleError(const char* msg,const char* reason,gboolean fatal);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
static void min_object_init (MinObject *obj)
{
        /* Init GLib object, we do not need this currently. */
}
/* ------------------------------------------------------------------------- */
static void min_object_class_init (MinObjectClass *klass)
{
        /* Init our class object.*/

        dbus_g_object_type_install_info (MIN_TYPE_OBJECT,
                                        &dbus_glib_min_object_object_info);
}
/* ------------------------------------------------------------------------- */
static void handleError(const char* msg,const char* reason,gboolean fatal)
{
        g_print(": ERROR: %s (%s)\n", msg, reason);
        if (fatal) {
                /*exit(EXIT_FAILURE);*/
/*                min_clbk.fatal_error (reason);*/
        }
}
/* -------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
void pl_attach_plugin (eapiIn_t **out_callback, eapiOut_t *in_callback)
{
        /* Binds the callbacks */
        memcpy (&min_clbk,in_callback,sizeof(eapiOut_t));
}
/* ------------------------------------------------------------------------- */
void pl_open_plugin ()
{
        DBusGConnection *bus = NULL;
        DBusGProxy *busProxy = NULL;
        MinObject *minObj = NULL;
        GMainLoop *mainloop = NULL;
        guint result = 0;
        GError *error = NULL;
        gboolean tmp = FALSE;

        g_type_init();

        mainloop = g_main_loop_new (NULL,FALSE);
        if (!mainloop) {
                handleError ("Could not create GMainLoop","OOM?",TRUE);
        }

        bus = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
        if (error) {
                handleError("Couldn't connect to session bus",
                        error->message,
                        TRUE);
        }

        busProxy = dbus_g_proxy_new_for_name(bus,
                                        DBUS_SERVICE_DBUS,
                                        DBUS_PATH_DBUS,
                                        DBUS_INTERFACE_DBUS);
        if (!busProxy) {
                handleError("Failed to get a proxy for D-Bus",
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
                handleError("D-Bus.RequestName RPC failed",
                                error->message,
                                TRUE);
        }
        if (result != 1) {
                handleError("Failed to get the primary well-known name.",
                        "RequestName result != 1", TRUE);
        }

        minObj = g_object_new(MIN_TYPE_OBJECT, NULL);
        if (!minObj) {
                handleError("Failed to create one Min instance.",
                        "Unknown(OOM?)", TRUE);
        }

        dbus_g_connection_register_g_object(bus,
                                        MIN_SERVICE_OBJECT_PATH,
                                        G_OBJECT(minObj));

        g_main_loop_run(mainloop);
}
/* ------------------------------------------------------------------------- */
void pl_close_plugin ()
{
        /* Deinit DBus */
}
/* ------------------------------------------------------------------------- */
void pl_detach_plugin (eapiIn_t **out_callback, eapiOut_t *in_callback)
{
        /* Unbinds the callbacks */
}
/* ------------------------------------------------------------------------- */
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
gboolean min_object_min_run_test(MinObject *obj,
                                gint moduleid,
                                gint caseid)
{
        /* Calls callback from MIN */
        if (min_clbk.run_test) {
                min_clbk.run_test (moduleid,caseid);
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
