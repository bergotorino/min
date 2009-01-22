/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Sampo Saaristo
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
 *  @file       testlibrary_fileio.c
 *  @version    0.1
 *  @brief      This file contains implementation of simple file io testcases
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <stdio.h>
#include <unistd.h>
#include <test_module_api.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */

extern FILE    *open_file (const char *);
extern int      delete_file (const char *);
extern int      create_file (const char *);
extern int      read_char_from_file (FILE *, char *);
extern int      write_char_to_file (FILE *, const char);

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
FILE           *file;
char            path[255];

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
int             file_io_open_and_close (TestCaseResult * tcr);
/* ------------------------------------------------------------------------- */
int             file_io_write2nonexist (TestCaseResult * tcr);
/* ------------------------------------------------------------------------- */
int             file_io_write_and_read (TestCaseResult * tcr);
/* ------------------------------------------------------------------------- */
int             file_io_delete_twice (TestCaseResult * tcr);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
int file_io_open_and_close (TestCaseResult * tcr)
{

        sprintf (path, "/tmp/min_io_test.%d", getpid ());
        if (create_file (path))
                goto file_io_open_and_close_err_out;
        file = open_file (path);
        if (!file)
                goto file_io_open_and_close_err_out;
        if (delete_file (path))
                goto file_io_open_and_close_err_out;
        RESULT (tcr, 0, "PASSED");
      file_io_open_and_close_err_out:
        RESULT (tcr, 1, "FAILED");

}

/* ------------------------------------------------------------------------- */

int file_io_open (TestCaseResult * tcr)
{

        sprintf (path, "/tmp/min_io_test.%d", getpid ());
        if (create_file (path))
                goto file_io_open_err_out;
        file = open_file (path);
        if (!file)
                goto file_io_open_err_out;
        RESULT (tcr, 0, "PASSED");
      file_io_open_err_out:
        RESULT (tcr, 1, "FAILED");

}

/* ------------------------------------------------------------------------- */
int file_io_write2nonexist (TestCaseResult * tcr)
{
        tcr->result_ = write_char_to_file (NULL, 'c');
        RESULT (tcr, 0, "PASSED");
}

/* ------------------------------------------------------------------------- */
int file_io_write_and_read (TestCaseResult * tcr)
{
        char            c;

        sprintf (path, "/tmp/min_io_test.%d", getpid ());
        if (create_file (path))
                goto file_io_write_and_read_err_out;
        file = open_file (path);

        if (!file)
                goto file_io_write_and_read_err_out;

        if (write_char_to_file (file, 'f'))
                goto file_io_write_and_read_err_out;

        if (read_char_from_file (file, &c))
                goto file_io_write_and_read_err_out;

        if (delete_file (path))
                goto file_io_write_and_read_err_out;

        if (c == 'f') {
                RESULT (tcr, 0, "PASSED");
        } else {
                RESULT (tcr, 1, "FAILED");
        }
      file_io_write_and_read_err_out:
        tcr->result_ = 1;
        RESULT (tcr, 1, "FAILED");

}

/* ------------------------------------------------------------------------- */
int file_io_delete_twice (TestCaseResult * tcr)
{
        sprintf (path, "/tmp/min_io_test.%d", getpid ());
        if (create_file (path))
                goto file_io_delete_twice_err_out;
        tm_printf (1, "", "%s", "file created");
        if (delete_file (path))
                goto file_io_delete_twice_err_out;
        tm_printf (1, "", "%s", "file deleted");
        if (!delete_file (path))
                goto file_io_delete_twice_err_out;
        tm_printf (1, "", "%s", "file not deleted");
        RESULT (tcr, 0, "PASSED");
      file_io_delete_twice_err_out:
        RESULT (tcr, 1, "FAILED");
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
int tm_get_test_cases (const char *cfg_file, DLList ** cases)
{
        ENTRY (*cases, "io open", file_io_open);
        ENTRY (*cases, "io open & close ", file_io_open_and_close);
        ENTRY (*cases, "io write to nonex", file_io_write2nonexist);
        ENTRY (*cases, "io write & read", file_io_write_and_read);
        ENTRY (*cases, "io del twice", file_io_delete_twice);

        return 0;
}

int tm_run_test_case (unsigned int id, const char *cfg_file,
                      TestCaseResult * result)
{
        int             retval = 0;
        DLList         *list = dl_list_create ();
        DLListIterator  it = DLListNULLIterator;
        TestCaseInfo   *tci = INITPTR;

        if (list != INITPTR) {
                tm_get_test_cases (cfg_file, &list);
                it = dl_list_at (list, id - 1);
                if (it != DLListNULLIterator) {
                        tci = (TestCaseInfo *) dl_list_data (it);
                        if (tci != INITPTR) {
                                if (tci->test_ != (ptr2test) INITPTR) {
                                        tci->test_ (result);
                                }
                        } else
                                retval = -1;
                } else
                        retval = -1;
        } else
                retval = -1;

        dl_list_free (&list);
        return retval;
}


/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
