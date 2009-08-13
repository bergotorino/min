#ifndef MIN_DBUS_PLUGIN_COMMON_H
#define MIN_DBUS_PLUGIN_COMMON_H

/**
 *  @file       min_dbus_plugin_common.h
 *  @version    0.1
 *  @brief      Common definitions for MIN DBus plugin
 */

#define MIN_SERVICE_NAME     "org.maemo.MIN"
#define MIN_SERVICE_OBJECT_PATH "/Min"
#define MIN_SERVICE_INTERFACE "org.maemo.MIN"

#define SIGNAL_MODULE_ADDED     "min_add_test_module_result"
#define SIGNAL_NEW_TEST_CASE    "min_new_test_case"
#define SIGNAL_CASE_RESULT      "min_case_result"
#define SIGNAL_CASE_RESUMED     "min_case_resumed"
#define SIGNAL_CASE_PAUSED      "min_case_paused"
#define SIGNAL_CASE_STARTED     "min_case_started"
#define SIGNAL_MODULE_READY     "min_module_ready"
#define SIGNAL_NEW_MODULE       "min_new_module"
#define SIGNAL_NO_MODULE        "min_no_module"
#define SIGNAL_MSG_PRINT        "min_case_msg"
#define SIGNAL_TEST_MODULES     "min_test_modules"
#define SIGNAL_TEST_FILES       "min_test_files"


#endif /* MIN_DBUS_PLUGIN_COMMON_H */

