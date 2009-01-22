/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Robert Galkowski
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
 *  @file       min_ext.c
 *  @version    0.1
 *  @brief      This file contains template for S.U.T. extension
 *              used in test cases written in python
 * 
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
/*path to Python.h file may differ, depending on Python 
instlallation*/
#include <python2.5/Python.h>
#include <test_module_api.h>
#include <min_test_event_if.h>
#include <min_ipc_mechanism.h>
#include <pthread.h>
#include <dlfcn.h>

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
DLList         *events_list = INITPTR;
DLList         *remotes_list = INITPTR;
int             testflag;
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
struct _case_args {
        int             id_;
        char           *file_;
        ptr2run         exec_;
        void           *mod_;
};

typedef struct _case_args case_args;

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */


/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/** function used to find test module (library) in search paths configured
for system.
@param l_name name of testmodule (not modified inside function)
@return full path and name of test module - remember that it is allocated 
 within function and needs to be freed when it's not needed anymore.
*/
char           *find_lib (char *l_name)
{
        void           *sh_mem_handle;
        int             sh_mem_id;
        void           *tmp_ptr;
        unsigned int    data_size;
        char           *paths;
        char           *path_to_check = NULL;
        char           *start_pos = NULL;
        char           *end_pos = NULL;

        sh_mem_id = sm_create ('a', sizeof (struct logger_settings_t));
        sh_mem_handle = sm_attach (sh_mem_id);
        tmp_ptr = sh_mem_handle + sizeof (struct logger_settings_t);
        data_size = strlen (tmp_ptr) + 1;
        paths = NEW2 (char, data_size);
        sm_read (tmp_ptr, paths, data_size);
        sm_detach (sh_mem_handle);
        start_pos = paths;
        do {
                end_pos = strchr (start_pos, ':');
                if (end_pos != NULL) {
                        *end_pos = '\0';
                }
                path_to_check = NEW2 (char, 
                                      strlen (start_pos) + strlen (l_name) + 5);
                /*check if it's library */
                sprintf (path_to_check, "%s/%s.so", start_pos, l_name);
                if (access (path_to_check, F_OK) == 0)
                        break;
                DELETE (path_to_check);
                path_to_check = NEW2 (char, 
                                      strlen (start_pos) + strlen (l_name) + 2);
                /*check if it's regular file (test case file ?) */
                sprintf (path_to_check, "%s/%s", start_pos, l_name);
                if (access (path_to_check, F_OK) == 0)
                        break;
                path_to_check = NULL;
                start_pos = end_pos + 1;
        } while (end_pos != NULL);
        DELETE (paths);
        if (path_to_check == NULL)
                MIN_WARN ("file not found");
        return path_to_check;
}
/* ------------------------------------------------------------------------- */
int find_case_by_title (char *case_name, DLList * cases_list)
{
        DLListIterator  work_case_item = dl_list_head (cases_list);
        int             index = -1;
        TestCaseInfo   *work_case;

        while (work_case_item != DLListNULLIterator) {
                work_case = (TestCaseInfo *) dl_list_data (work_case_item);
                if (strcasecmp (work_case->name_, case_name) == 0) {
                        index = work_case->id_;
                        break;
                }
                work_case_item = dl_list_next (work_case_item);
        }

        return index;
}
/* ------------------------------------------------------------------------- */
void           *thread_exec (void *args)
{
        TestCaseResult  min_result;
        int             id;
        char           *testcase_file;
        ptr2run         run_case_func;
        int             end_flags_id = 0;
        /* pointer to shared memory segment holding flags to control 
         * completion of asynchroneus operations */
        void           *end_flags_cont = NULL;          
        AsyncOpFlags    end_flags;

        id = ((case_args *) args)->id_;
        testcase_file = ((case_args *) args)->file_;
        run_case_func = ((case_args *) args)->exec_;

        end_flags_id = sm_create ('p', sizeof (AsyncOpFlags));
        end_flags_cont = sm_attach (end_flags_id);
        sm_read (end_flags_cont, (void *)&end_flags, sizeof (AsyncOpFlags));
        end_flags.parallel_test_ongoing_ = ESTrue;
        sm_write (end_flags_cont, (void *)&end_flags, sizeof (AsyncOpFlags));
        sm_detach (end_flags_cont);

        MIN_DEBUG ("external testcase starting");
        run_case_func (id, testcase_file, &min_result);
        MIN_DEBUG ("external testcase finished");

        dlclose (((case_args *) args)->mod_);
        end_flags_cont = sm_attach (end_flags_id);
        sm_read (end_flags_cont, (void *)&end_flags, sizeof (AsyncOpFlags));
        end_flags.parallel_test_ongoing_ = ESFalse;
        sm_write (end_flags_cont, (void *)&end_flags, sizeof (AsyncOpFlags));
        sm_detach (end_flags_cont);

        MIN_DEBUG ("end thread here");
        return NULL;
}
/* ------------------------------------------------------------------------- */
int wait_response (int mq_id, TMSCommand re_type)
{
        int             i, result_c = 0;
        int             end_flags_id = 0;
        /* pointer to shared memory segment holding flags to control 
         * completion of asynchroneus operations */
        void           *end_flags_cont = NULL; 
        AsyncOpFlags    end_flags;
        int            *caseid;
        MsgBuffer       message;
        int             ownid = getpid ();

        for (i = 0; i < 50; i++) {
                result_c = mq_peek_message (mq_id, ownid);
                if (result_c == 0) {
                        usleep (200000);
                        continue;
                }
                mq_read_message (mq_id, ownid, &message);
                if ((message.type_ == MSG_EXTIF) &&
                    (message.extif_msg_type_ == re_type)) {
                        /*for debug purposes */
                        MIN_DEBUG ("response received");
                        if (message.special_ == 0)
                                return 0;
                        
                        /* this is response for remote run */
                        MIN_DEBUG ("remote case started");
                        if (remotes_list == INITPTR)
                                remotes_list = dl_list_create ();
                        caseid = NEW (int);
                        *caseid = message.special_;
                        dl_list_add (remotes_list, caseid);
                        MIN_DEBUG ("adding caseid");
                        
                        end_flags_id = sm_create ('p', sizeof 
                                                  (AsyncOpFlags));
                        end_flags_cont = sm_attach (end_flags_id);
                        sm_read (end_flags_cont, &end_flags,
                                 sizeof (AsyncOpFlags));
                        
                        end_flags.remote_cases_list_ = remotes_list;
                        MIN_DEBUG ("sm data: %x,otig = %x",
                                    end_flags.remote_cases_list_,
                                    remotes_list);
                        sm_write (end_flags_cont, (void *)&end_flags,
                                  sizeof (AsyncOpFlags));
                        sm_detach (end_flags_cont);
                        return 0;
                } else {
                        /*return this message to queue */
                        mq_send_message (mq_id, &message);
                }
                usleep (200000);
        }
        /*we got so far, so message did not come. Assume protocol 
          timeout */
        MIN_WARN ("assume timeout");
        return -1;
}
/* ------------------------------------------------------------------------- */
static PyObject *p_tm_printf (PyObject * self, PyObject * args)
{
        /*declare variables */
        PyObject       *result;
        int             result_c = 0;
        char           *printout;

        /*Modify string in "" according to expected parameters. For details,
           refer to Python/C API documentation */
        if (!PyArg_ParseTuple
            (args, /*Format string */ "s", /*adresses of variables */
             &printout))
                return NULL;

        /*Actual testing should start here */
        tm_printf (1, "Python printout", printout);

        /*Variables that are of interest should now be converted
           into PyObject. For details of fields in next function call 
           refer to Python/C API documentation */
        result =
            Py_BuildValue ( /*Format string */ "i", /*variable(s) */
                           result_c);
        return result;
}

/* -------------------------------------------------------------------------- */
static PyObject *p_tm_indic_set (PyObject * self, PyObject * Args)
{
        minEventIf    *event;
        char           *event_name;
        PyObject       *result;
        int             result_c = 0;

        if (!PyArg_ParseTuple
            (Args, /*Format string */ "s", /*adresses of variables */
             &event_name))
                return NULL;
        event = min_event_create (event_name, EIndication);
        event->SetType (event, ESetEvent);
        Event (event);
        min_event_destroy (event);
        result =
            Py_BuildValue ( /*Format string */ "i", /*variable(s) */
                           result_c);
        return result;
}
/* -------------------------------------------------------------------------- */
static PyObject *p_tm_state_set (PyObject * self, PyObject * Args)
{
        minEventIf    *event;
        char           *event_name;
        PyObject       *result;
        int             result_c = 0;

        if (!PyArg_ParseTuple
            (Args, /*Format string */ "s", /*adresses of variables */
             &event_name))
                return NULL;
        event = min_event_create (event_name, EState);
        event->SetEventType (event, EState);
        event->SetType (event, ESetEvent);
        Event (event);
        min_event_destroy (event);
        result =
            Py_BuildValue ( /*Format string */ "i", /*variable(s) */
                           result_c);
        return result;
}
/* -------------------------------------------------------------------------- */
static PyObject *p_tm_state_unset (PyObject * self, PyObject * Args)
{
        minEventIf    *event = NULL;
        char           *event_name;
        PyObject       *result;
        int             result_c = 0;

        if (!PyArg_ParseTuple
            (Args, /*Format string */ "s", /*adresses of variables */
             &event_name))
                return NULL;
        event = min_event_create (event_name, EState);
        event->SetEventType (event, EState);
        event->SetType (event, EUnsetEvent);
        Event (event);
        min_event_destroy (event);
        result =
            Py_BuildValue ( /*Format string */ "i", /*variable(s) */
                           result_c);
        return result;
}
/* -------------------------------------------------------------------------- */
static PyObject *p_tm_event_request (PyObject * self, PyObject * Args)
{

        minEventIf    *event = NULL;
        char           *event_name;
        char           *state_flag = NULL;
        PyObject       *result;
        int             result_c = 0;
        int             arg_size = 0;
        TEventType_t    event_type = EIndication;
        arg_size = PyTuple_Size (Args);
        if (arg_size == 2) {
                if (!PyArg_ParseTuple
                    (Args, /*Format string */ "ss", /*adresses of variables */
                     &event_name, &state_flag))
                        return NULL;
        } else {
                if (arg_size == 1) {
                        if (!PyArg_ParseTuple
                            (Args, /*Format string */ "s",
                             /*adresses of variables */ &event_name))
                                return NULL;
                }
        }

        if (state_flag != NULL) {
                if (strcasecmp (state_flag, "state") == 0)
                        event_type = EState;
        }
        event = min_event_create (event_name, event_type);
        event->SetEventType (event, event_type);
        event->SetType (event, EReqEvent);
        Event (event);


        /*if events list doesn't exist, create it, then add event */
        if (events_list == INITPTR) {
                events_list = dl_list_create ();
        }
        dl_list_add (events_list, (void *)event);
        result =
            Py_BuildValue ( /*Format string */ "i", /*variable(s) */
                           result_c);
        return result;
}
/* -------------------------------------------------------------------------- */
static PyObject *p_tm_event_release (PyObject * self, PyObject * Args)
{

        minEventIf    *event = NULL;
        char           *event_name;
        PyObject       *result;
        DLListIterator  event_item = DLListNULLIterator;
        int             result_c = 0;
        if (!PyArg_ParseTuple
            (Args, /*Format string */ "s", /*adresses of variables */
             &event_name))
                return NULL;

        event_item = dl_list_head (events_list);
        while (event_item != INITPTR) {
                event = (minEventIf *) dl_list_data (event_item);
                if (strcmp (event->event_name_, event_name) == 0)
                        break;
                /*event found */
                event_item = dl_list_next (event_item);
        }
        if (event_item == DLListNULLIterator) {
                /*event was not found */
                result_c = -1;
                MIN_WARN ("Trying to release event : %s "
                           "that was not requested", event_name);
                result = Py_BuildValue ( /*Format string */ "i", 
                                         /*variable(s) */ result_c);
                return result;
        }

        event->SetType (event, ERelEvent);
        Event (event);
        min_event_destroy (event);
        dl_list_remove_it (event_item);
        result = Py_BuildValue ( /*Format string */ "i", /*variable(s) */
                                 result_c);
        return result;
}
/* -------------------------------------------------------------------------- */
static PyObject *p_tm_event_wait (PyObject * self, PyObject * Args)
{
        minEventIf    *event = NULL;
        char           *event_name;
        PyObject       *result;
        DLListIterator  event_item = DLListNULLIterator;
        int             result_c = 0;

        MIN_DEBUG ("waiting event in pyext");

        if (!PyArg_ParseTuple
            (Args, /*Format string */ "s", /*adresses of variables */
             &event_name))
                return NULL;

        event_item = dl_list_head (events_list);
        while (event_item != INITPTR) {
                event = (minEventIf *) dl_list_data (event_item);
                if (strcmp (event->event_name_, event_name) == 0)
                        break;
                /*event found */
                event_item = dl_list_next (event_item);
        }
        if (event_item == DLListNULLIterator) {
                /*event was not found */
                result_c = -1;
                MIN_WARN ("event : %s was not requested", event_name);
                result =
                    Py_BuildValue ( /*Format string */ "i", /*variable(s) */
                                   result_c);
                return result;
        }

        event->SetType (event, EWaitEvent);
        Event (event);
        MIN_DEBUG ("waiting complete?");
        result =
            Py_BuildValue ( /*Format string */ "i", /*variable(s) */
                           result_c);
        return result;
}
/* -------------------------------------------------------------------------- */
static PyObject *p_tm_complete_case (PyObject * self, PyObject * Args)
{
        int             result_c = 0;
        PyObject       *result;
        DLList         *min_cases = dl_list_create ();
        ptr2case        get_cases_func = NULL;
        ptr2run         run_cases_func = NULL;
        char           *module_name = NULL;
        char           *testcase_file = NULL;
        char           *testcase_title = NULL;
        TestCaseResult  min_result;
        char           *mod_path;
        char           *tc_file = NULL;
        int             arg_size = PyTuple_Size (Args);
        void           *lib_ptr = NULL;
        int             case_index = 0;

        switch (arg_size) {
        case 2:
                if (!PyArg_ParseTuple
                    (Args, /*Format string */ "ss", /*adresses of variables */
                     &module_name, &testcase_title))
                        return NULL;
                break;
        case 3:
                if (!PyArg_ParseTuple (Args, /*Format string */ "sss",
                                       /*adresses of variables */
                                       &module_name, &testcase_file,
                                       &testcase_title))
                        return NULL;
                break;
        default:
                MIN_WARN ("Arguments not specified correctly");
                return NULL;
                break;
        }

        mod_path = find_lib (module_name);

        lib_ptr = dlopen (mod_path, RTLD_LOCAL|RTLD_LAZY);

        if (lib_ptr == NULL) {
                MIN_WARN ("Could not load test module library");
                return NULL;
        }

        get_cases_func = (ptr2case) dlsym (lib_ptr, "tm_get_test_cases");
        if (get_cases_func == NULL) {
                MIN_WARN ("Faulty library (no \"tm_get_test_cases\")");
                return NULL;
        }

        run_cases_func = (ptr2run) dlsym (lib_ptr, "tm_run_test_case");
        if (run_cases_func == NULL) {
                MIN_WARN ("Faulty library (no \"tm_run_test_case\")");
                return NULL;
        }

        if (testcase_file != NULL)
                tc_file = find_lib (testcase_file);
        get_cases_func (tc_file, &min_cases);
        MIN_DEBUG ("cases fetched : %d", dl_list_size (min_cases));

        case_index = find_case_by_title (testcase_title, min_cases);
        MIN_DEBUG ("case : %s has index %d", testcase_title, case_index);

        if (!(case_index > 0)) {
                MIN_DEBUG ("could not find testcase : %s in module: %s",
                            testcase_title, module_name);
                return NULL;
        }

        result_c = run_cases_func (case_index, testcase_file, &min_result);

        result_c = min_result.result_;

        dlclose (lib_ptr);
        DELETE (mod_path);
        if (tc_file != NULL)
                DELETE (tc_file);


        result =
            Py_BuildValue ( /*Format string */ "i", /*variable(s) */
                           result_c);
        return result;

}
/* -------------------------------------------------------------------------- */
static PyObject *p_tm_start_case (PyObject * self, PyObject * Args)
{
        int             result_c = 0;
        PyObject       *result;

        int             thread_creation_result;
        pthread_t       exec_thread;

        DLList         *min_cases = dl_list_create ();
        ptr2case        get_cases_func = NULL;
        ptr2run         run_cases_func = NULL;
        char           *module_name = NULL;
        char           *testcase_file = NULL;
        char           *testcase_title = NULL;

        /*struct needed as an argument for thread func */
        case_args      *execution_params = NEW (case_args);
/*use pythonpath for now, switch to ModSearchPath later*/
        char           *mod_path = NULL;
        char           *tc_file = NULL;
        int             arg_size = PyTuple_Size (Args);
        void           *lib_ptr = NULL;
        int             case_index = 0;

        MIN_DEBUG ("start extension func.");
        switch (arg_size) {
        case 2:
                if (!PyArg_ParseTuple
                    (Args, /*Format string */ "ss", /*adresses of variables */
                     &module_name, &testcase_title))
                        return NULL;
                break;
        case 3:
                if (!PyArg_ParseTuple (Args, /*Format string */ "sss",
                                       /*adresses of variables */
                                       &module_name, &testcase_file,
                                       &testcase_title))
                        return NULL;
                break;
        default:
                MIN_WARN ("Arguments not specified correctly");
                return NULL;
                break;
        }

        mod_path = find_lib (module_name);

        lib_ptr = dlopen (mod_path, RTLD_LOCAL|RTLD_LAZY);

        if (lib_ptr == NULL) {
                MIN_WARN ("Could not load test module library: %s", mod_path);
                return NULL;
        }

        get_cases_func = (ptr2case) dlsym (lib_ptr, "tm_get_test_cases");
        if (get_cases_func == NULL) {
                MIN_WARN ("Faulty library (no \"tm_get_test_cases\")");
                return NULL;
        }

        run_cases_func = (ptr2run) dlsym (lib_ptr, "tm_run_test_case");
        if (run_cases_func == NULL) {
                MIN_WARN ("Faulty library (no \"tm_run_test_case\")");
                return NULL;
        }
        if (testcase_file != NULL)
                tc_file = find_lib (testcase_file);
        get_cases_func (tc_file, &min_cases);
        MIN_DEBUG ("cases fetched : %d", dl_list_size (min_cases));

        case_index = find_case_by_title (testcase_title, min_cases);
        MIN_DEBUG ("case : %s has index %d", testcase_title, case_index);

        if (!(case_index > 0)) {
                MIN_WARN ("could not find testcase : %s in module: %s",
                           testcase_title, module_name);
                return NULL;
        }

        execution_params->id_ = case_index;
        if (testcase_file != NULL) {
                execution_params->file_ =
                    NEW2 (char, strlen (testcase_file) + 1);
                sprintf (execution_params->file_, "%s", testcase_file);
        }

        else
                execution_params->file_ = NULL;
        MIN_DEBUG ("testcase_file = %s", testcase_file);
        execution_params->exec_ = run_cases_func;
        execution_params->mod_ = lib_ptr;
        MIN_DEBUG ("run cases = %x", run_cases_func);
        MIN_DEBUG ("try to start thread");
        MIN_DEBUG ("argument: %x", &execution_params);
        thread_creation_result =
            pthread_create (&exec_thread, NULL, thread_exec,
                            (void *)execution_params);

        MIN_DEBUG ("thread started ? - %d", thread_creation_result);
        DELETE (mod_path);
        if (tc_file != NULL)
                DELETE (tc_file);
        result =
            Py_BuildValue ( /*Format string */ "i", /*variable(s) */
                           result_c);
        return result;
}
/* -------------------------------------------------------------------------- */
static PyObject *p_tm_create_logger (PyObject * self, PyObject * Args)
{
        MinLogger     *created_logger;
        char           *dir = NULL;
        char           *filename = NULL;
        char           *format = NULL;
        TSLoggerType    logger_type;

        if (!PyArg_ParseTuple
            (Args, /*Format string */ "sss", /*adresses of variables */ &dir,
             &filename, &format))
                return NULL;
        if (strcasestr (format, "html") != NULL)
                logger_type = ESHtml;
        else
                logger_type = ESTxt;

        created_logger = mnl_create (dir,
                                     filename,
                                     logger_type,
                                     ESFile,
                                     0,
                                     ESTrue,
                                     ESTrue,
                                     ESTrue, ESFalse, ESTrue, 1000, ESFalse);
        MIN_DEBUG ("created logfile");
        if (created_logger == INITPTR)
                return NULL;
        return Py_BuildValue ( /*Format string */ "i", /*variable(s) */ 
                              (unsigned int)created_logger);
}
/* -------------------------------------------------------------------------- */
static PyObject *p_tm_log (PyObject * self, PyObject * Args)
{
        unsigned int    logger_handle;
        char            style = ESNoStyle;
        char           *text;
        int             arg_size = PyTuple_Size (Args);
        switch (arg_size) {
        case (3):
                if (!PyArg_ParseTuple
                    (Args, /*Format string */ "ics",
                     /*adresses of variables */ &logger_handle, &style,
                     &text))
                        return NULL;
                break;
        case (2):
                if (!PyArg_ParseTuple
                    (Args, /*Format string */ "is", /*adresses of variables */
                     &logger_handle, &style, &text))
                        return NULL;
                break;

        default:
                MIN_WARN ("Wrongly specified arguments for log write ");
                return NULL;
        }
        switch (style) {
        case ('b'):
        case ('B'):
                style = ESBold;
                break;
        case ('i'):
        case ('I'):
                style = ESCursive;
                break;
        case ('u'):
        case ('U'):
                style = ESUnderline;
                break;
        default:

                style = ESNoStyle;
        }

        mnl_log ((MinLogger *) logger_handle, style, text);
        return Py_BuildValue ( /*Format string */ "i", /*variable(s) */ 0);
}
/* -------------------------------------------------------------------------- */
static PyObject *p_tm_destroy_logger (PyObject * self, PyObject * Args)
{
        unsigned int    logger_handle;
        MinLogger     *instance;
        if (!PyArg_ParseTuple
            (Args, /*Format string */ "i", /*adresses of variables */
             &logger_handle))
                return NULL;
        instance = (MinLogger *) logger_handle;
        mnl_destroy (&instance);
        return Py_BuildValue ( /*Format string */ "i", /*variable(s) */ 0);
}
/* ------------------------------------------------------------------------- */
static PyObject *p_tm_allocate_slave (PyObject * self, PyObject * args)
{
        int             result_c = 0;
        PyObject       *result;
        char           *slavename;
        int             mq_id = 0;
        MsgBuffer       message;

        MIN_DEBUG ("python slave allocation");
        if (!PyArg_ParseTuple
            (args, /*Format string */ "s", /*adresses of variables */
             &slavename))
                return NULL;
        mq_id = mq_open_queue ('a');
        if (mq_id == 0) {
                MIN_WARN ("message queue fault");
                return NULL;
        }
        sprintf (message.message_, "%s", slavename);
        message.type_ = MSG_EXTIF;
        message.sender_ = getpid ();
        message.receiver_ = getppid ();
        sprintf (message.desc_, "Python slave allocation");
        message.extif_msg_type_ = EAllocateSlave;
        result_c = mq_send_message (mq_id, &message);
        if (result_c != 0) {
                MIN_WARN ("message sending fault");
                return NULL;
        }
        result_c = wait_response (mq_id, EResponseSlave);
        if (result_c != 0) {
                MIN_WARN ("response not received - protocol timeout");
                return NULL;
        }
        result =
            Py_BuildValue ( /*Format string */ "i", /*variable(s) */
                           result_c);
        return result;
}
/* -------------------------------------------------------------------------- */
static PyObject *p_tm_release_slave (PyObject * self, PyObject * args)
{
        int             result_c = 0;
        PyObject       *result;
        char           *slavename;
        int             mq_id = 0;
        MsgBuffer       message;

        if (!PyArg_ParseTuple
            (args, /*Format string */ "s", /*adresses of variables */
             &slavename))
                return NULL;
        mq_id = mq_open_queue ('a');
        if (mq_id == 0) {
                MIN_WARN ("message queue fault");
                return NULL;
        }
        sprintf (message.message_, "%s", slavename);
        message.type_ = MSG_EXTIF;
        message.sender_ = getpid ();
        message.receiver_ = getppid ();
        sprintf (message.desc_, "Python slave releasing");
        message.extif_msg_type_ = EFreeSlave;
        result_c = mq_send_message (mq_id, &message);
        if (result_c != 0) {
                MIN_WARN ("message sending fault");
                return NULL;
        }
        result_c = wait_response (mq_id, EResponseSlave);
        if (result_c != 0) {
                MIN_WARN ("response not received - protocol timeout");
                return NULL;
        }
        result =
            Py_BuildValue ( /*Format string */ "i", /*variable(s) */
                           result_c);
        return result;
}
/* -------------------------------------------------------------------------- */
static PyObject *p_tm_request_remote_event (PyObject * self, PyObject * Args)
{
        minEventIf    *event = NULL;
        char           *event_name;
        MsgBuffer       message;
        char           *slave_name;
        char           *state_flag = NULL;
        PyObject       *result;
        int             result_c = 0;
        int             arg_size = 0;
        long            mq_id = 0;
        TEventType_t    event_type = EIndication;
        arg_size = PyTuple_Size (Args);

        MIN_DEBUG ("ARGS: %d", arg_size);
        MIN_DEBUG ("ENTER REMOTE REQUEST FUNC");
        switch (arg_size) {
        case 2:
                if (!PyArg_ParseTuple
                    (Args, /*Format string */ "ss", /*adresses of variables */
                     &slave_name, &event_name))
                        return NULL;
                break;
        case 3:
                if (!PyArg_ParseTuple
                    (Args, /*Format string */ "sss",
                     /*adresses of variables */ &slave_name, &event_name,
                     &state_flag))
                        return NULL;
                break;
        default:
                MIN_WARN ("missing arguments for remote event request!");
                return NULL;
        }

        if (state_flag != NULL) {
                if (strcasecmp (state_flag, "state") == 0)
                        event_type = EState;
        }
        event = min_event_create (event_name, event_type);
        event->SetEventType (event, event_type);
        event->SetType (event, EReqEvent);
        Event (event);
        /*if events list doesn't exist, create it, then add event */
        if (events_list == INITPTR) {
                events_list = dl_list_create ();
        }
        mq_id = mq_open_queue ('a');
        sprintf (message.message_, "%s", slave_name);
        message.type_ = MSG_EXTIF;
        message.sender_ = getpid ();
        message.receiver_ = getppid ();
        if (event_type == EState)
                sprintf (message.desc_, "request %s state", event_name);
        else
                sprintf (message.desc_, "request %s", event_name);
        message.extif_msg_type_ = ERemoteSlave;
        result_c = mq_send_message (mq_id, &message);
        dl_list_add (events_list, (void *)event);
        result_c = wait_response (mq_id, EResponseSlave);

        MIN_DEBUG ("got answer for remote event request");
        result =
            Py_BuildValue ( /*Format string */ "i", /*variable(s) */
                           result_c);
        return result;
}
/* ------------------------------------------------------------------------- */
static PyObject *p_tm_remote_event_release (PyObject * self, PyObject * Args)
{

        minEventIf    *event = NULL;
        char           *event_name;
        char           *slave_name;

        MsgBuffer       message;
        PyObject       *result;
        long            mq_id = 0;
        DLListIterator  event_item = DLListNULLIterator;

        int             result_c = 0;
        if (!PyArg_ParseTuple
            (Args, /*Format string */ "ss", /*adresses of variables */
             &slave_name, &event_name))
                return NULL;

        event_item = dl_list_head (events_list);
        while (event_item != INITPTR) {
                event = (minEventIf *) dl_list_data (event_item);
                if (strcmp (event->event_name_, event_name) == 0)
                        break;
                /*event found */
                event_item = dl_list_next (event_item);
        }
        if (event_item == DLListNULLIterator) {
                /*event was not found */
                result_c = -1;
                MIN_WARN ("Trying to release event : %s that "
                           "was not requested", event_name);
                result =
                    Py_BuildValue ( /*Format string */ "i", /*variable(s) */
                                   result_c);
                return result;
        }

        mq_id = mq_open_queue ('a');
        sprintf (message.message_, "%s", slave_name);
        message.type_ = MSG_EXTIF;
        message.sender_ = getpid ();
        message.receiver_ = getppid ();
        sprintf (message.desc_, "release %s", event_name);
        message.extif_msg_type_ = ERemoteSlave;
        result_c = mq_send_message (mq_id, &message);

        event->SetType (event, ERelEvent);
        Event (event);
        min_event_destroy (event);
        dl_list_remove_it (event_item);
        result_c = wait_response (mq_id, EResponseSlave);
        result =
            Py_BuildValue ( /*Format string */ "i", /*variable(s) */
                           result_c);
        return result;
}
/* ------------------------------------------------------------------------- */
static PyObject *p_tm_remote_run_case (PyObject * self, PyObject * Args)
{
        MsgBuffer       message;
        char           *slave_name;
        char           *module_name;
        char           *testcase_file = NULL;
        int             case_num;
        PyObject       *result;
        int             result_c = 0;
        int             arg_size = 0;
        long            mq_id = 0;
        arg_size = PyTuple_Size (Args);

        MIN_DEBUG ("ARGS: %d", arg_size);
        MIN_DEBUG ("ENTER REMOTE RUN CASE");
        switch (arg_size) {
        case 3:
                if (!PyArg_ParseTuple
                    (Args, /*Format string */ "ssi",
                     /*adresses of variables */ &slave_name, &module_name,
                     &case_num))
                        return NULL;
                break;
        case 4:
                if (!PyArg_ParseTuple
                    (Args, /*Format string */ "sssi",
                     /*adresses of variables */ &slave_name, &module_name,
                     &testcase_file, &case_num))
                        return NULL;
                break;
        default:
                MIN_WARN ("missing or wrong arguments for remote run");
                return NULL;

        }

        mq_id = mq_open_queue ('a');
        sprintf (message.message_, "%s", slave_name);
        message.type_ = MSG_EXTIF;
        message.sender_ = getpid ();
        message.receiver_ = getppid ();
        message.extif_msg_type_ = ERemoteSlave;
        if (testcase_file == NULL) {
                sprintf (message.desc_, "run %s dummy_tc_file %d",
                         module_name, case_num);
        } else {
                sprintf (message.desc_, "run %s %s %d", module_name,
                         testcase_file, case_num);
        }
        result_c = mq_send_message (mq_id, &message);
        result_c = wait_response (mq_id, EResponseSlave);
        MIN_DEBUG ("got answer for remote run");
        result =
            Py_BuildValue ( /*Format string */ "i", /*variable(s) */
                           result_c);
        return result;
}
/* -------------------------------------------------------------------------- */
static PyMethodDef min_extMethods[] = {
        /*Copy the following line for each defined function, changing
           apropriate entries */
        /*Basic MIN functionality */
        {"Print_to_cui", p_tm_printf, METH_VARARGS, "Print to MIN cui"},
        {"Set_indication_event", p_tm_indic_set, METH_VARARGS,
         "Set Indication Event"},
        {"Set_state_event", p_tm_state_set, METH_VARARGS, "Set State Event"},
        {"Unset_event", p_tm_state_unset, METH_VARARGS, "Unset State Event"},
        {"Request_event", p_tm_event_request, METH_VARARGS, "Request Event"},
        {"Release_event", p_tm_event_release, METH_VARARGS, "Release Event"},
        {"Wait_event", p_tm_event_wait, METH_VARARGS, "Wait for Event"},

        /*local tetcase execution */
        {"Complete_case", p_tm_complete_case, METH_VARARGS,
         "Execute other module's testcase - blocking"},
        {"Start_case", p_tm_start_case, METH_VARARGS,
         "Execute other module's testcase - asynchronous"},

        /*logger */
        {"Create_logger", p_tm_create_logger, METH_VARARGS, ""},
        {"Log", p_tm_log, METH_VARARGS, ""},
        {"Destroy_logger", p_tm_destroy_logger, METH_VARARGS, ""},

        /*master - slave related */
        {"Allocate_slave", p_tm_allocate_slave, METH_VARARGS,
         "Slave Allocation"},
        {"Free_slave", p_tm_release_slave, METH_VARARGS, "Slave Releasing"},
        {"Request_remote_event", p_tm_request_remote_event, METH_VARARGS, ""},
        {"Release_remote_event", p_tm_remote_event_release, METH_VARARGS, ""},
        {"Run_remote_case", p_tm_remote_run_case, METH_VARARGS, ""},
        /*do not remove next line, it is mandatory */
        {NULL, NULL, 0, NULL}
};
/* -------------------------------------------------------------------------- */
PyMODINIT_FUNC initmin_ext (void)
{
        (void)Py_InitModule ("min_ext", min_extMethods);
}
