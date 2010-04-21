/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Robert Galkowski
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
 *  @file       py_moduleCases.c
 *  @version    0.1
 *  @brief      This file contains implementation of the test cases for
 *              pyproto.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <py_module.h>
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

/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
extern char *strcasestr (const char *haystack,const char *needle);
/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
void* python_lib_handle;
Text* orig_python_path;         /** holds original value of PYTHONPATH variable
				    it will be read during module start and 
				    restored when module is unloaded */
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
LOCAL char* fetch_title(char* input);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */
/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/** Function used to fetch title of testcase from python's function's docstring
 * @param input docstring extracted from python script
 * @return extracted title - it is allocated inside the function, so it
 * is mandatory to free it afterwards to prevent memory leak
 */
LOCAL char* fetch_title(char* input)
{
        int length;
        char* retval;
        char* pos = strchr(input,'\n');
	MIN_DEBUG ("PYTHON input: %s", input);
        if (pos != NULL) length = pos - input;
        else length = strlen(input);
        retval = NEW2(char,length+2);
        snprintf(retval,length+2,"%s",input);
        return retval;
}
/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Remove the .py ending from filename
 *  @return new string with the .py ending stripped
 */
char* cutname(char* file)
{
        char* output = NEW2(char,strlen(file)-2);
        sprintf(output,"%s","\0");
        strncat(output,file,strlen(file)-3);
        return output;
}
/* ------------------------------------------------------------------------- */
/** Extract the a filename from path. 
 *  Add ".py" ending if the file does not contain one.
 *  @param path absolute or relative path to file
 */
char* cut_file_from_path(char* path)
{
        char* out = NULL;

        char* temp = path;
        if (*path != '/') {
                if (strcasestr(path,".py")!= NULL) {
                        out = NEW2(char,strlen(path)+1);
                        sprintf(out,"%s",path);
                        return out;
                }
                else {
                        out = NEW2(char,strlen(path)+4);
                        sprintf(out,"%s",path);
                        strcat(out,".py");
                        return out;
                }
        }

        do {
                out = &temp[1];
                temp = strchr(out,'/');
        } while (temp != NULL);
        temp = NEW2(char,strlen(out)+1);
        sprintf(temp,"%s",out);
        return temp;
}
/* ------------------------------------------------------------------------- */
/** tm_get_test_cases is used to inquire test cases from the Test Module.
 *  Test cases are stored to array of test cases. The Test Framework will be 
 *  the owner of the data in the DLList after tm_get_test_cases return
 *  and it does the memory deallocation. 
 */
int tm_get_test_cases( const char * cfg_file, DLList ** cases )
{

        int pos = 0;
        char* workstring = INITPTR;
        char* workstring2 = INITPTR;
        int count = 0;
        PyObject* scriptname;
        PyObject* p_module;/*python module object*/
        PyObject* dictionary;/*python's dictionary*/
        /*for parsing fetched namespace dictionary*/
        PyObject* p_key; 
        PyObject* p_value;
        PyObject* attrib_name;

        PyObject *python_string2;
        PyObject *pErrType;
        PyObject *pErrValue; 
        PyObject *pErrTraceback;
        PyObject* pString;
        

//         Py_Initialize();


        workstring = NEW2(char,strlen(cfg_file)+1);
        sprintf(workstring,"%s",cfg_file);
        workstring2 = cut_file_from_path(workstring);
	DELETE(workstring);
        workstring = cutname(workstring2);
        DELETE(workstring2);
        scriptname = PyString_FromString(workstring);
        DELETE(workstring);
        p_module = PyImport_Import(scriptname);
        if (p_module == NULL) {
                PyErr_Fetch(&pErrType, &pErrValue, &pErrTraceback);
                pString = PyObject_Str(pErrType);
                MIN_WARN("ERROR TYPE : %s",PyString_AsString(pString));
                Py_XDECREF(pString);
		Py_XDECREF(pErrType);
                pString = PyObject_Str(pErrValue);
                MIN_WARN("ERROR VALUE: %s",PyString_AsString(pString));
                Py_XDECREF(pString);
		Py_XDECREF(pErrValue);
                Py_XDECREF(p_module);
                MIN_WARN("Python module not loaded - check the syntax");
                return 0;
        }

        Py_XDECREF(scriptname);

        /*fetch namespace dictionary, pick proper objects (cases) and
        put them in passed list. Case : is function, is callable, 
        has name starting with case_. Title should be defined
        in docstring, but it's not mandatory*/
        dictionary = PyModule_GetDict(p_module);
        while (PyDict_Next(dictionary,&pos,&p_key,&p_value) != 0){
                if ((strcmp((char*)p_value->ob_type->tp_name,"function") == 0)
		    && PyCallable_Check(p_value)
		    && strcasestr(PyString_AS_STRING(p_key),"case_") != NULL) {
			
                        attrib_name = PyString_FromString("__doc__");
                        python_string2 = PyObject_GetAttr(p_value,attrib_name);
			
                        if (strlen(PyString_AS_STRING(python_string2)) > 0){
                                /*make title from docstring*/
                                workstring = fetch_title
					(PyString_AsString(python_string2));
                                MIN_DEBUG("Adding: %s",
                                           PyString_AsString(p_key));
                                ENTRY2D(*cases,PyString_AsString(p_key),
					dl_list_size(*cases)+1,workstring);
                                DELETE(workstring);
                                Py_XDECREF(attrib_name);
                                Py_XDECREF(python_string2);

                        }
                        else {
                                /*make title from function name*/

                                ENTRY2(*cases,PyString_AsString(p_key),
				       dl_list_size(*cases)+1);
                                MIN_DEBUG("Adding: %s",
					  PyString_AsString(p_key));
                        }
                }
        }
	
        if(dl_list_size(*cases) == 0){
                MIN_WARN ("No valid MIN python cases found in file %s",
                           cfg_file);
        }
        Py_XDECREF(dictionary);
        Py_XDECREF(p_module);
        //Py_Finalize();

        return count;
}
/* ------------------------------------------------------------------------- */
/** Initiliaze python module, called by TMC
 */
void tm_initialize()
{
        void *sh_mem_handle;
        int sh_mem_id;
        void *tmp_ptr;
        unsigned int data_size;
        char *paths;
        Text *py_paths;

        sh_mem_id = sm_create('a',sizeof(struct logger_settings_t));
	if (sh_mem_id < 0) {
		MIN_WARN ("sm_create() failed");
		return;
	}
        sh_mem_handle = sm_attach(sh_mem_id);
        tmp_ptr = sh_mem_handle + sizeof(struct logger_settings_t);
        data_size = strlen(tmp_ptr)+1;
        paths = NEW2(char,data_size);
        sm_read(tmp_ptr,paths,data_size);
        sm_detach(sh_mem_handle);
        MIN_DEBUG("initializing python in %d",getpid());
        orig_python_path = tx_create(getenv("PYTHONPATH"));
        py_paths = tx_create(paths);
        DELETE(paths);
        /*append mod search paths to existing pythonpaths.*/
        if (orig_python_path != NULL) {
                tx_c_append(py_paths,":");
                tx_append(py_paths,orig_python_path);
        }
        setenv("PYTHONPATH",tx_share_buf(py_paths),1);
        MIN_DEBUG("PYTHONPATH is %s",getenv("PYTHONPATH"));
        Py_Initialize();
	tx_destroy (&py_paths);
        MIN_DEBUG("python initialized in %d",getpid());
}
/* ------------------------------------------------------------------------- */
/** Deinitiliaze python module, called by TMC
 */
void tm_finalize()
{
        MIN_DEBUG("finalizing python in %d",getpid());
        Py_Finalize();
        setenv("PYTHONPATH",tx_share_buf(orig_python_path),1);
        tx_destroy(&orig_python_path);
        MIN_DEBUG("python finalized in %d",getpid());

}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
