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
 *  @file       py_module.c
 *  @version    0.1
 *  @brief      This file contains implementation of the pyproto.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <test_module_api.h>
#include <min_logger.h>
#include <min_parser.h>
#include <min_test_event_if.h>
#include "py_module.h"
#include <python2.5/Python.h>
#include <sched.h>
/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */
extern char* cut_file_from_path(char* path);
extern char* cutname(const char* file);
extern char* strcasestr (const char *haystack, const char *needle);
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
/* None */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* None */
TSBool check_end_conditions()
{
        int caseid = 0;
        MsgBuffer message;
        int own_addr = getpid();
        int mq_id = mq_open_queue('a');
        int cond_container_id = 0;
        void* cond_container = NULL;/*pointer to shared memory segment
        holding flags to control completion of asynchroneus operations*/
        AsyncOpFlags end_conditions;
        TSBool retval = ESFalse;
        int *saved_caseid;
        DLListIterator it = DLListNULLIterator;
        if (mq_id <= 0) {
                MIN_WARN ("message queue fault");
                return NULL;
        }
        /*connect to shared memory segment and fetch info about status of
        asynchroneous operations.*/

        sched_yield();
        cond_container_id = sm_create('p',sizeof(AsyncOpFlags));
	if (cond_container_id < 0) {
		MIN_WARN ("sm_create() failed");
		return retval;
	}
        cond_container = sm_attach(cond_container_id);
        sm_read(cond_container,&end_conditions,sizeof(AsyncOpFlags));
        sm_detach(cond_container);
        /*go through conditions and check if it's ok to finish testcase
        execution*/
        //retval = !(end_conditions.parallel_test_ongoing_);
       
        if (end_conditions.remote_cases_list_!= INITPTR){
                 if (mq_peek_message(mq_id,own_addr)!= 0 ){
                        mq_read_message(mq_id,own_addr,
                                        &message);
                        if ((message.type_ == MSG_EXTIF)&&
                            (message.extif_msg_type_ == ERemoteSlaveResponse)){
                                caseid = message.special_;
                                it = dl_list_head(end_conditions.remote_cases_list_);
                                while (it != DLListNULLIterator){
                                        saved_caseid = (int*)dl_list_data(it);
                                        if ((*saved_caseid) == caseid){
                                                DELETE(saved_caseid);
                                                break;
                                        }
                                        it = dl_list_next(it);
                                }
                                if ( it!= DLListNULLIterator) dl_list_remove_it(it);
                                if (dl_list_size(end_conditions.remote_cases_list_) == 0){
                                        retval = (!(end_conditions.parallel_test_ongoing_)) & ESTrue;
                                }
                        }
                        else mq_send_message(mq_id,&message);/*return message
                        to queue*/
                }
        }
        else {
                retval = (!(end_conditions.parallel_test_ongoing_)) & ESTrue;
        }
        return retval;
}

/* ======================== FUNCTIONS ====================================== */
// void tm_initialize()
// {
//         Py_Initialize();
// }
// 
// void tm_finalize()
// {
//         Py_Finalize();
//         return;
// }
// 




/* ------------------------------------------------------------------------- */
int tm_run_test_case( unsigned int      id
                    , const char     *  cfg_file
                    , TestCaseResult *  result )
{

        PyObject* scriptname;
        PyObject* p_module;
        PyObject* dictionary;
        PyObject* p_key;
        PyObject* p_value;
        PyObject* pErrType;
        PyObject* pErrValue;
        PyObject* pErrTraceback;
        PyObject* pString;
        PyObject* pRetVal;
        char* workstring = NULL;
        char* workstring2 = NULL;
        int pos = 0;
        int count = 1;
        TSBool ok_to_finish = ESFalse;
        int cond_container_id = 0;
        void* cond_container = NULL;/*pointer to shared memory segment
        holding flags to control completion of asynchroneus operations*/
        AsyncOpFlags end_conditions;

        long cRetVal = 0;

        workstring = NEW2(char,strlen(cfg_file)+1);
        sprintf(workstring,"%s",cfg_file);
        workstring2 = cut_file_from_path(workstring);
        DELETE(workstring);
        workstring = cutname(workstring2);
        DELETE(workstring2);

        scriptname = PyString_FromString(workstring);

        DELETE(workstring);

        p_module = PyImport_Import(scriptname);

        Py_XDECREF(scriptname);
        dictionary = PyModule_GetDict(p_module);
        while (PyDict_Next(dictionary,&pos,&p_key,&p_value) != 0){
                if ((strcmp((char*)p_value->ob_type->tp_name,"function") == 0)
                     && PyCallable_Check(p_value)
                     && strcasestr(PyString_AS_STRING(p_key),"case_") != NULL)  {
                        if (count != id) count ++;
                        else{
                                /*testcase was found*/
                                cond_container_id = sm_create('p',sizeof(AsyncOpFlags));
				if (cond_container_id < 0) {
					MIN_WARN ("sm_create() failed");
					return -1;
				}
                                cond_container = sm_attach(cond_container_id);
                                end_conditions.remote_cases_list_ = INITPTR;
                                end_conditions.parallel_test_ongoing_ = ESFalse;
                                sm_write(cond_container,
                                         (void*)&end_conditions,
                                         sizeof(AsyncOpFlags));
                                sm_detach(cond_container);
                                /*Execute the testcase*/
                                pRetVal = PyObject_CallObject(p_value,NULL);
                                /*interpret the results*/
                                if (PyErr_Occurred()){
                                        PyErr_Fetch(&pErrType, &pErrValue, &pErrTraceback);
                                              pString = PyObject_Str(pErrType);
                                        MIN_WARN("ERROR TYPE : %s",PyString_AsString(pString));
                                        Py_XDECREF(pString);
                                        Py_XDECREF(pErrType);
                                        pString = PyObject_Str(pErrValue);
                                        MIN_WARN("ERROR VALUE: %s",PyString_AsString(pString));
                                        sprintf(result->desc_,"%s",PyString_AsString(pString));
                                        result->result_ = TP_CRASHED;
                                        Py_XDECREF(pString);
                                        Py_XDECREF(pErrValue);
                                }
                                else{
                                        if (pRetVal == NULL){
                                                result->result_ = TP_LEAVE;
                                                sprintf(result->desc_,"Reason unspecified");
                                                Py_XDECREF(pRetVal);
                                        }
                                        else {
                                                cRetVal = PyInt_AsLong(pRetVal);
                                                MIN_INFO ("executed PYTHON case, result = %d",cRetVal);
                                                if (cRetVal == 0) result->result_ = TP_PASSED;
                                                else result->result_ = TP_FAILED;
                                                sprintf(result->desc_,"Python testcase ended");
                                                Py_XDECREF(pRetVal);
                                        }
                                }
                                /*now let's see if there are still operations pending*/
                                do{
                                        ok_to_finish = check_end_conditions();
                                        /*if end conditions not met, wait a little bit, usleep to prevent
                                        tight cpu-hungry loop*/
                                        if (ok_to_finish == ESFalse ){ usleep(200000);MIN_WARN ("still nok");};
                                } while(ok_to_finish != ESTrue);

                                break;
                        }
                }
        }
        Py_XDECREF(p_module);
	return 0;
}
/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
