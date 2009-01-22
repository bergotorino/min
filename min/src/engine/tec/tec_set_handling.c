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
 *  @file       tec_set_handling.c
 *  @version    0.1
 *  @brief      This file contains implementation of test set 
 *              handling functionality
 */

/* ----------------------------------------------------------------------------
 * INCLUDE FILES
 */
#include <tec.h>
#include <data_api.h>
#include <stdio.h>
/* ----------------------------------------------------------------------------
 * EXTERNAL DATA STRUCTURES
 * None
 */
extern pthread_mutex_t tec_mutex_;
/* --------------------------------------------------------------------------- 
 * EXTERNAL FUNCTION PROTOTYPES
 */
/* None */

/* ----------------------------------------------------------------------------
 * CONSTANTS
 */

/* ----------------------------------------------------------------------------
 * MACROS
 */
/* ----------------------------------------------------------------------------
 * LOCAL CONSTANTS AND MACROS
 */
/* None */

/* ----------------------------------------------------------------------------
 * MODULE DATA STRUCTURES
 */
/* None */

/* ----------------------------------------------------------------------------
 * LOCAL FUNCTION PROTOTYPES
 */
/* None */

/* ------------------------------------------------------------------------- 
 * FORWARD DECLARATIONS 
 */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */


LOCAL test_case_s *getcase (char *module, char *title)
{
        DLListIterator  work_module_item = dl_list_head (available_modules);
        DLListIterator  work_case_item = DLListNULLIterator;
        char            module_fname[MaxFileName];
        char            case_title[MaxTestCaseName];

        while (work_module_item != DLListNULLIterator) {
                tm_get_module_filename (work_module_item, module_fname);
                if (strcmp (module_fname, module) == 0)
                        break;
                work_module_item = dl_list_next (work_module_item);
        }
        if (work_module_item == DLListNULLIterator)
                return INITPTR;
        work_case_item = dl_list_head (tm_get_tclist (work_module_item));
        while (work_case_item != DLListNULLIterator) {
                tc_get_title (work_case_item, case_title);
                if (strcmp (case_title, title) == 0)
                        break;
                work_case_item = dl_list_next (work_case_item);
        }

        if (work_case_item == DLListNULLIterator)
                return INITPTR;
        else
                return ((test_case_s *) dl_list_data (work_case_item));
}



/* ======================== FUNCTIONS ====================================== */
char           *create_path ()
{
        int             p_lenghth;
        char           *work_path, *c_dir;
        char            name[23], hour[6];
        time_t          now;
        struct tm      *info_now;

        c_dir = getenv ("HOME");
        now = time (NULL);
        info_now = localtime (&now);

        /*create string for filename - current
           date and hour */
        sprintf (hour, "%d:%d", info_now->tm_hour, info_now->tm_min);
        sprintf (name, "%d-%d-%d", info_now->tm_year + 1900,
                 info_now->tm_mon + 1, info_now->tm_mday);
        strcat (name, "_");
        strcat (name, hour);
        p_lenghth = strlen (c_dir) + strlen (name) + 13;
        work_path = NEW2 (char, p_lenghth);
        sprintf (work_path, "%s", c_dir);
        strcat (work_path, "/.min/");
        strcat (work_path, name);
        strcat (work_path, ".set");

        return work_path;
}

int ec_set_write_file (DLList * cases_list, char *filename)
{


        filename_t      module_name;
        title_string_t  case_title;
        DLListIterator  work_case_item;
        DLListIterator  work_module_item;
        FILE           *set_file;
        /*create string for filename - current
           dfate and hour */
        char           *name = strstr (filename, "200");

        MIN_DEBUG ("%d cases in set", dl_list_size (cases_list));
        MIN_DEBUG ("filename : %s", filename);

        set_file = fopen (filename, "w");
        fprintf (set_file, "[TestSetStart]\n");
        fprintf (set_file, "TestSetName=%s\n", name);

        work_case_item = dl_list_head (cases_list);


        /*write data to file */
        while (work_case_item != DLListNULLIterator) {

                fprintf (set_file, "[TestSetCaseStart]\n");
                work_module_item = tc_get_test_module_ptr (work_case_item);
                tm_get_module_filename (work_module_item, module_name);
                fprintf (set_file, "ModuleName=%s\n", module_name);
                tc_get_title (work_case_item, case_title);
                fprintf (set_file, "Title=%s\n", case_title);
                fprintf (set_file, "[TestSetCaseEnd]\n");
                work_case_item = dl_list_next (work_case_item);

        }

        fprintf (set_file, "[TestSetEnd]\n");
        fclose (set_file);

        return 1;
}

/** Function called to save current test set to file. File will be saved
in current working directory, its name will be current date and hour.
@param cases_list list of cases in set
*/
int ec_set_write (DLList * cases_list)
{
        char           *set_path = create_path ();
        int             result = -1;

        result = ec_set_write_file (cases_list, set_path);
        DELETE (set_path);

        return result;
}

/* ------------------------------------------------------------------------- */



/**Function used to execute cases from current set in sequence.
@param work_cases_list list of test cases to be executed
*/
int ec_run_set_seq (DLList * work_cases_list)
{
        DLListIterator  work_case_item = dl_list_head (work_cases_list);
        DLListIterator  exec_case = DLListNULLIterator;
        int             group_id = random ();   /* generate unique group id */

        if (work_case_item == DLListNULLIterator) {
                MIN_WARN ("Invalid list of test cases passed !");
                return -1;
        }
        pthread_mutex_lock (&tec_mutex_);

        /*we mark first case of the group to be executed */
        exec_case = ec_select_case (work_case_item, group_id);
        work_case_item = dl_list_next (work_case_item);
        /*now we will iterate through the remaining cases in received list */
        while (work_case_item != DLListNULLIterator) {
                ec_select_case (work_case_item, group_id);
                work_case_item = dl_list_next (work_case_item);
        }
        pthread_mutex_unlock (&tec_mutex_);
        ec_exec_case (exec_case);

        return 0;
}



/** Function used to execute cases from test set in parallel
@param work_cases_list list of test cases to be executed
*/
int ec_run_set_par (DLList * work_cases_list)
{
        DLListIterator  work_case_item = dl_list_head (work_cases_list);
        DLListIterator  exec_case = DLListNULLIterator;
        int             group_id = random ();   /* generate unique group id */

        if (work_case_item == DLListNULLIterator) {
                MIN_WARN ("Invalid list of test cases passed !");
                return -1;
        }
        pthread_mutex_lock (&tec_mutex_);

        /*"select" all cases in received list */
        while (work_case_item != DLListNULLIterator) {
                exec_case = ec_select_case (work_case_item, group_id);
                ec_exec_case (exec_case);
                work_case_item = dl_list_next (work_case_item);
        }
        pthread_mutex_unlock (&tec_mutex_);

        return 0;
}


/**Function used to read selected .set file.
@param setname name of selected .set file
@param set_cases_list list that will store read cases
*/
void ec_set_read (DLList * set_cases_list, char *setname)
{

        char           *work_path;
        char           *c_dir;
        int             p_lenghth;
        int             caseread = 1;
        int             mod_result = 1;
        int             title_result = 1;
        test_case_s    *work_case = INITPTR;
        MinParser     *set_file;
        MinSectionParser *set_section_p;
        MinSectionParser *set_case_p;
        char           *module_name_;
        char           *case_title_;

        MIN_DEBUG ("set name : %s", setname);
        c_dir = getenv ("HOME");
        /*build filename and path to open set file */
        p_lenghth = strlen (c_dir) + strlen (setname) + 9;
        work_path = NEW2 (char, p_lenghth);
        sprintf (work_path, "%s", c_dir);
        strcat (work_path, "/.min/");

        set_file = mp_create (work_path, setname, ENoComments);

        if (set_file == INITPTR) {

                MIN_WARN ("Could not open set file %s%s", work_path,
                             setname);
                return;
        }
        DELETE (work_path);

        set_section_p = mp_section (set_file, "", "", 1);

        set_case_p = mp_section (set_file,
                                 "[TestSetCaseStart]",
                                 "[TestSetCaseEnd]", caseread);

        while (set_case_p != INITPTR) {

                mod_result = mmp_get_line (set_case_p,
                                           "ModuleName=", &module_name_,
                                           ESNoTag);

                title_result = mmp_get_line (set_case_p,
                                             "Title=", &case_title_, ESNoTag);

                if ((module_name_ != INITPTR) && (case_title_ != INITPTR)) {

                        work_case = getcase (module_name_, case_title_);
                        tc_add (set_cases_list, work_case);

                        DELETE (module_name_);
                        DELETE (case_title_);
                }

                mmp_destroy (&set_case_p);
                caseread++;
                set_case_p = mp_section (set_file,
                                         "[TestSetCaseStart]",
                                         "[TestSetCaseEnd]", caseread);
        }

        mp_destroy (&set_file);
}


/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */


/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */


/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "check.h"
#endif                          /* MIN_UNIT_TEST */

/* End of file */
