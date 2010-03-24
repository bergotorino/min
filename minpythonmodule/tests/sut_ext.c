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
 *  @file       sut_ext_template.c
 *  @version    0.1
 *  @brief      This file contains template for S.U.T. extension
 *              used in test cases written in python
 * 
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
/*path to Python.h file may differ, depending on Python 
instlallation*/
#include <python2.6/Python.h>
#include <test_module_api.h>
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
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */

/**Function that uses tested functionality. To be able to cooperate
 * with Python, user must define list of arguments during tuple parsing,
 * and convert return value to "PyObject*".
*/
static PyObject* file_creation(PyObject* self, PyObject* args)
{
        /*declare variables*/
        PyObject* result;
        char* filename;
        FILE* resultfile;
        int result_c = 0;
        /*Modify string in "" according to expected parameters. For details,
        refer to Python/C API documentation*/
        if (!PyArg_ParseTuple(args, /*Format string*/"s", /*adresses of variables*/&filename))
                return NULL;
        /*Actual testing should start here*/
        resultfile = fopen(filename,"w");
        fclose(resultfile);

        result_c = access(filename,F_OK);
        MIN_DEBUG("ACCESS returned : %d",result_c);

        /*Variables that are of interest should now be converted
        into PyObject. For details of fields in next function call 
        refer to Python/C API documentation*/
        result = Py_BuildValue(/*Format string*/"i",/*variable(s)*/result_c);
        return result;
}

static PyObject* fault(PyObject* self, PyObject* args)
{
        /*declare variables*/
        PyObject* result;
        char* filename;
        int result_c = 0;
        /*Modify string in "" according to expected parameters. For details,
        refer to Python/C API documentation*/
        /*Actual testing should start here*/
        filename = 0xdeadbeef;
        free(filename);
        /*Variables that are of interest should now be converted
        into PyObject. For details of fields in next function call 
        refer to Python/C API documentation*/
        result = Py_BuildValue(/*Format string*/"i",/*variable(s)*/result_c);
        return result;
}



/* -------------------------------------------------------------------------- */
static PyMethodDef sut_extMethods[] =
{
        /*Copy the following line for each defined function, changing
        apropriate entries*/
        {"File_creation", file_creation, METH_VARARGS,"Open and close file"},
        {"Segmentation_Fault", fault, METH_VARARGS,"This should segfault"},
        /*do not remove next line, it is mandatory*/
        {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initsut_ext(void)
{
        (void) Py_InitModule("sut_ext", sut_extMethods);
}
