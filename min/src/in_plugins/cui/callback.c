/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Juha Perala
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
 *  @file       callback.c
 *  @version    0.1
 *  @brief      This file contains implementation of console UI of MIN
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <string.h>             /* for strlen() */
#include <stdlib.h>             /* for calloc() */
#include <menu.h>               /* for ncurses  */
#include <time.h>               /* for time stuff */

#include <callback.h>
#include <consoleui.h>
#include <data_api.h>
#include <dllist.h>
#include <data_api.h>
#include <dirent.h>
#include <min_common.h>
#include <min_plugin_interface.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
extern bool     continue_;      /* flag indicating whether to continue prog  */
extern MENU    *my_menu;        /* menu itself */
extern WINDOW  *main_window;    /* main window */
extern WINDOW  *menu_window;    /* window including menu */
extern eapiIn_t out_clbk_;      /*  */
extern eapiOut_t min_clbk_;     /*  */
extern DLList *case_list_;
extern DLList *executed_case_list_;      /* */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
LOCAL void      main_menu (void);
/* ------------------------------------------------------------------------- */
LOCAL void      case_menu (void);
/* ------------------------------------------------------------------------- */
LOCAL void      module_menu (void);
/* ------------------------------------------------------------------------- */
LOCAL void      test_set_main_menu (void);
/* ------------------------------------------------------------------------- */
LOCAL void      test_set_menu (void);
/* ------------------------------------------------------------------------- */
LOCAL void      start_new_case_menu (void);
/* ------------------------------------------------------------------------- */
LOCAL void      run_multiple_tests_menu (void);
/* ------------------------------------------------------------------------- */
LOCAL void      ongoing_cases_menu (void);
/* ------------------------------------------------------------------------- */
LOCAL void      executed_cases_menu (void);
/* ------------------------------------------------------------------------- */
LOCAL void      passed_cases_menu (void);
/* ------------------------------------------------------------------------- */
LOCAL void      failed_cases_menu (void);
/* ------------------------------------------------------------------------- */
LOCAL void      aborted_cases_menu (void);
/* ------------------------------------------------------------------------- */
LOCAL void      quit_program (void);
/* ------------------------------------------------------------------------- */
LOCAL void      load_test_set_list (void);
/* ------------------------------------------------------------------------- */
LOCAL void      load_test_set (void *p);
/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
/** main menu structure */
callback_s      cb_main_menu[] = {
        {"Case menu", NULL, case_menu, NULL, NULL, NULL},
        {"Module menu", NULL, module_menu, NULL, NULL, NULL},
        {"Test set menu", NULL, test_set_main_menu, NULL, NULL, NULL},
        {"Exit", NULL, quit_program, NULL, NULL, NULL},
        {NULL, NULL, NULL, NULL, NULL, NULL}
};

DLList         *selected_cases;
DLList         *available_modules;
DLList         *instantiated_modules;

/** linked list of test set files */
DLList         *test_set_files = INITPTR;

/** linked list of test modules */
DLList         *test_modules = INITPTR;

/** linked list of test case files */
DLList         *test_case_files = INITPTR;

/** focus position for main menu */
focus_pos_s    main_menu_focus = { 0,0 };

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
/* test set menu callback structure */
LOCAL callback_s cb_test_set_main_menu[] = {
        {"Create/modify test set", NULL, test_set_menu, main_menu, NULL,
         NULL},
        {"Load test set", NULL, load_test_set_list, main_menu, NULL, NULL},
        {NULL, NULL, NULL, NULL, NULL, NULL}
};

/* case menu callback structure */
LOCAL callback_s cb_case_menu[] = {
        {"Start new case", NULL, start_new_case_menu, main_menu, NULL, NULL},
        {"Run multiple tests", NULL, run_multiple_tests_menu, main_menu,
         NULL, NULL},
        {"Ongoing cases", NULL, ongoing_cases_menu, main_menu, NULL, NULL},
        {"Executed cases", NULL, executed_cases_menu, main_menu, NULL, NULL},
        {"Passed cases", NULL, passed_cases_menu, main_menu, NULL, NULL},
        {"Failed cases", NULL, failed_cases_menu, main_menu, NULL, NULL},
        {"Aborted/Crashed cases", NULL, aborted_cases_menu, main_menu, NULL,
         NULL},
        {NULL, NULL, NULL, NULL, NULL, NULL}
};

/* pointer to module menu callback structure */
LOCAL callback_s *cb_module_menu = INITPTR;

/* pointer to start new case menu callback structure */
LOCAL callback_s *cb_start_new_case_menu = INITPTR;

/* run multiple tests callback structure */
LOCAL callback_s *cb_run_multiple_tests_menu = INITPTR;

/* ongoing cases menu callback structure */
LOCAL callback_s *cb_ongoing_cases_menu = INITPTR;

/* executed cases menu callback structure */
LOCAL callback_s *cb_executed_cases_menu = INITPTR;

/* passed cases menu callback structure */
LOCAL callback_s *cb_passed_cases_menu = INITPTR;

/* failed cases menu callback structure */
LOCAL callback_s *cb_failed_cases_menu = INITPTR;

/* aborted cases menu callback structure */
LOCAL callback_s *cb_aborted_cases_menu = INITPTR;

/* test result menu callback structure */
LOCAL callback_s *cb_test_result_menu = INITPTR;

/* test case control menu callback structure */
LOCAL callback_s *cb_pause_resume_abort_menu = INITPTR;

/* test case output menu callback structure */
LOCAL callback_s *cb_view_output_menu = INITPTR;

/* test set menu callback structure */
LOCAL callback_s *cb_test_set_menu = INITPTR;

/* added test cases to test set menu callback structure */
LOCAL callback_s *cb_add_tcs_to_test_set_menu = INITPTR;

/* remove test cases from test set menu callback structure */
LOCAL callback_s *cb_remove_tcs_from_test_set_menu = INITPTR;

/* load test set menu callback structure */
LOCAL callback_s *cb_load_test_set_menu = INITPTR;

/* add test module menu callback structure */
LOCAL callback_s *cb_add_test_module_menu = INITPTR;

/* add testcase files menu callback structure */
LOCAL callback_s *cb_add_test_case_files_menu = INITPTR;

/* test case title */
LOCAL char      title[MaxTestCaseName] = { '\0' };

/* temporary module name */
LOCAL char      module_name[MaxFileName + 1];

/* focus position information for different menus */
LOCAL focus_pos_s       case_menu_focus = { 0,0 };
LOCAL focus_pos_s       test_set_main_menu_focus = { 0,0 };
LOCAL focus_pos_s       add_test_module_menu_focus = { 0,0 };
LOCAL focus_pos_s       ongoing_cases_menu_focus = { 0,0 };
LOCAL focus_pos_s       executed_cases_menu_focus = { 0,0 };
LOCAL focus_pos_s       passed_cases_menu_focus = { 0,0 };
LOCAL focus_pos_s       failed_cases_menu_focus = { 0,0 };
LOCAL focus_pos_s       aborted_cases_menu_focus = { 0,0 };
LOCAL focus_pos_s       test_set_menu_focus = { 0,0 };
LOCAL focus_pos_s       remove_tcs_from_test_set_menu_focus = { 0,0 };
LOCAL focus_pos_s       add_test_case_files_menu_focus = { 0,0 };
LOCAL focus_pos_s       module_menu_focus = { 0,0 };
LOCAL focus_pos_s       start_new_case_menu_focus = { 0,0 };
LOCAL focus_pos_s       run_multiple_tests_menu_focus = { 0,0 };
LOCAL focus_pos_s       add_tcs_to_test_set_menu_focus = { 0,0 };
LOCAL focus_pos_s       load_test_set_menu_focus = { 0,0 };

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
LOCAL void      start_one_tc (void *p);
/* ------------------------------------------------------------------------- */
LOCAL void      toggle_menu_item (void);
/* ------------------------------------------------------------------------- */
LOCAL void      invert_select_tcs_selection (void);
/* ------------------------------------------------------------------------- */
LOCAL void      start_cases_sequentially (void *p);
/* ------------------------------------------------------------------------- */
LOCAL void      start_cases_parallel (void *p);
/* ------------------------------------------------------------------------- */
LOCAL void      get_selected_cases (void);
/* ------------------------------------------------------------------------- */
LOCAL int       get_loaded_modules (void);
/* ------------------------------------------------------------------------- */
LOCAL int       get_tcs_for_start_new_case (void);
/* ------------------------------------------------------------------------- */
LOCAL int       get_tcs_for_run_multiple_tests (void);
/* ------------------------------------------------------------------------- */
LOCAL int       get_ongoing_cases (void);
/* ------------------------------------------------------------------------- */
LOCAL int       get_cases_by_result_type (callback_s ** s, int result_type);
/* ------------------------------------------------------------------------- */
LOCAL void      set_cbs (callback_s * cb,       /* cb to be filled with data */
                         char *choice,  /* name of the menu item */
                         char *desc,    /* menu item description */
                         ptr_to_fun on_right,   /* ptr to lower level menu */
                         ptr_to_fun on_left,    /* ptr to upper level menu */
                         ptr_to_fun2 on_select, /* ptr to action funtion */
                         void *data,    /* pointer to tm, tc or tr data */
                         int intendation);
/* ------------------------------------------------------------------------- */
LOCAL void      null_cbs (callback_s * cb);
/* ------------------------------------------------------------------------- */
LOCAL void      free_cbs (callback_s * cb);
/* ------------------------------------------------------------------------- */
LOCAL void      test_result_view (void *p);
/* ------------------------------------------------------------------------- */
LOCAL int       test_result_menu (void *p, ptr_to_fun on_left);
/* ------------------------------------------------------------------------- */
LOCAL void      tr_for_executed_case (void *p);
/* ------------------------------------------------------------------------- */
LOCAL void      tr_for_passed_case (void *p);
/* ------------------------------------------------------------------------- */
LOCAL void      tr_for_failed_case (void *p);
/* ------------------------------------------------------------------------- */
LOCAL void      tr_for_aborted_case (void *p);
/* ------------------------------------------------------------------------- */
LOCAL void      pause_resume_abort_menu (void *p);
/* ------------------------------------------------------------------------- */
LOCAL void      pause_tc (void *p);
/* ------------------------------------------------------------------------- */
LOCAL void      resume_tc (void *p);
/* ------------------------------------------------------------------------- */
LOCAL void      abort_tc (void *p);
/* ------------------------------------------------------------------------- */
LOCAL void      view_output (void *p);
/* ------------------------------------------------------------------------- */
LOCAL void      view_output_for_ongoing_cases (void *p);
/* ------------------------------------------------------------------------- */
LOCAL void      back_to_tr_menu (void);
/* ------------------------------------------------------------------------- */
LOCAL void      back_to_control_menu (void);
/* ------------------------------------------------------------------------- */
LOCAL int       create_test_set_menu ();
/* ------------------------------------------------------------------------- */
LOCAL void      start_test_set_sequentially (void);
/* ------------------------------------------------------------------------- */
LOCAL void      start_test_set_parallel (void);
/* ------------------------------------------------------------------------- */
LOCAL void      remove_test_set (void *p);
/* ------------------------------------------------------------------------- */
LOCAL void      save_test_set (void);
/* ------------------------------------------------------------------------- */
LOCAL void      add_tcs_to_test_set_menu (void);
/* ------------------------------------------------------------------------- */
LOCAL int       add_tcs_to_test_set (void);
/* ------------------------------------------------------------------------- */
LOCAL void      invert_add_tcs_selection (void);
/* ------------------------------------------------------------------------- */
LOCAL void      remove_tcs_from_test_set_menu (void);
/* ------------------------------------------------------------------------- */
LOCAL int       remove_tcs_from_test_set (void);
/* ------------------------------------------------------------------------- */
LOCAL int       get_test_sets (void);
/* ------------------------------------------------------------------------- */
LOCAL void      add_cases_to_test_set (void *p);
/* ------------------------------------------------------------------------- */
LOCAL void      remove_cases_from_test_set (void *p);
/* ------------------------------------------------------------------------- */
LOCAL int       compare_items (const void *data1, const void *data2);
/* ------------------------------------------------------------------------- */
LOCAL void      empty_test_set (void);
/* ------------------------------------------------------------------------- */
LOCAL int       get_test_modules (void);
/* ------------------------------------------------------------------------- */
LOCAL int       get_test_case_files (void);
/* ------------------------------------------------------------------------- */
LOCAL void      add_test_module_menu (void);
/* ------------------------------------------------------------------------- */
LOCAL void      add_test_case_files_menu (void);
/* ------------------------------------------------------------------------- */
LOCAL void      save_module_name (void *p);
/* ------------------------------------------------------------------------- */
LOCAL void      add_module (void *p);

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/** Shows main menu
 */
LOCAL void main_menu ()
{
        /* Show new menu */
        update_menu (cb_main_menu, "Main menu", 0, &main_menu_focus);
}

/* ------------------------------------------------------------------------- */
/** Shows case menu
 */
LOCAL void case_menu ()
{
        /* Show new menu */
        update_menu (cb_case_menu, "Case menu", 0, &case_menu_focus);
}

/* ------------------------------------------------------------------------- */
/** Shows test set menu
 */
LOCAL void test_set_main_menu ()
{
        /* Show new menu */
        update_menu (cb_test_set_main_menu, "Test set menu", 0,
                     &test_set_main_menu_focus);
}

/* ------------------------------------------------------------------------- */
/** Sets data for specific callback structure
 *  @param cb pointer to callback structure
 *  @param choice pointer to menu item name
 *  @param desc pointer to menu item description
 *  @param on_right pointer to lower level menu
 *  @param on_left pointer to upper level menu
 *  @param on_select pointer to action function
 *  @param data pointer to user data
 */
LOCAL void set_cbs (callback_s * cb,
                    char *choice,
                    char *desc,
                    ptr_to_fun on_right,
                    ptr_to_fun on_left,
                    ptr_to_fun2 on_select, void *data, int intendation)
{
        char           *text = INITPTR;

        if (cb != INITPTR && cb != NULL) {
                if (intendation) {
                        text =
                            (char *)malloc (sizeof (char) *
                                            (strlen (choice) + 3));
                        strcpy (text, "  ");
                        strcat (text, choice);
                } else {
                        text =
                            (char *)malloc (sizeof (char) *
                                            (strlen (choice) + 1));
                        strcpy (text, choice);
                }
                cb->choice = text;
                cb->desc = desc;
                cb->on_right_ = on_right;
                cb->on_left_ = on_left;
                cb->on_select_ = on_select;
                cb->ptr_data = data;
        }
}

/* ------------------------------------------------------------------------- */
/** Nulls data for specific callback structure
 *  @param cb pointer to callback structure
 */
LOCAL void null_cbs (callback_s * cb)
{
        if (cb != INITPTR && cb != NULL) {
                free (cb->choice);
                cb->choice = NULL;
                cb->desc = NULL;
                cb->on_right_ = NULL;
                cb->on_left_ = NULL;
                cb->on_select_ = NULL;
                cb->ptr_data = NULL;
        }
}

/* ------------------------------------------------------------------------- */
/** Frees memory allocated to callback structure
 *  @param cb pointer to callback structure
 */
LOCAL void free_cbs (callback_s * cb)
{
        if (cb != INITPTR && cb != NULL) {
                free (cb);
                cb = INITPTR;
        }
}

/* ------------------------------------------------------------------------- */
/** Shows add test module menu
 */
LOCAL void add_test_module_menu (void)
{
        if (get_test_modules () != -1)
                /* Show new menu */
                update_menu (cb_add_test_module_menu,
                             "Add test module menu", 0, 
                             &add_test_module_menu_focus);
}

/* ------------------------------------------------------------------------- */
/** Shows add test module menu
 */
LOCAL void add_test_case_files_menu (void)
{
        if (get_test_case_files () != -1)
                /* Show new menu */
                update_menu (cb_add_test_case_files_menu,
                             "Add test case file menu", 0,
                             &add_test_case_files_menu_focus);
}

/* ------------------------------------------------------------------------- */
/** Saves module name for later use
 */
LOCAL void save_module_name (void *p)
{
        char           *text = INITPTR;

        text = (char *)dl_list_data ((DLListIterator) p);
        if (text != INITPTR) {
                STRCPY (module_name, text, MaxFileName);
                /* get rid of .so extension */
                module_name[strlen (module_name) - 3] = '\0';
        }
}

/* ------------------------------------------------------------------------- */
/** Adds test module to test framework
 */
LOCAL void add_module (void *p)
{
        DLList         *conf_list = dl_list_create ();
        int             i = 0;
        ITEM          **items = NULL;
        callback_s     *cb = INITPTR;

        items = menu_items (my_menu);

        /* iterate through menu items to find highlighted ones */
        for (i = 1; i < item_count (my_menu); ++i) {
                /* test if menu item is highlighted */
                if (item_value (items[i]) == TRUE) {
                        /* get user data attached to menu item */
                        cb = (callback_s *) item_userptr (items[i]);
                        /* add test case file to linked list */
                        dl_list_add (conf_list, cb->ptr_data);
                }
        }

        //if (ec_add_module (module_name, conf_list, 0) == 0) {
                popup_window ("Module added", 1);
		//}
}

/* ------------------------------------------------------------------------- */
/** Get test modules in specific directory and creates add test module menu
 *  @return 0 in case of success, returns -1 in case of failure.
 */
LOCAL int get_test_modules (void)
{
        DLListItem     *dl_item = INITPTR;
        char           *test_module = INITPTR;
        struct dirent **namelist;
        int             n = 0;
        int             i = 0;
        char           *dir = INITPTR;
        char           *ptr = INITPTR;

        /* free memory allocated for callback structure */
        free_cbs (cb_add_test_module_menu);
#if 0
        /* remove items from current test sets list */
        if (dl_list_size (test_modules) != -1) {
                dl_item = dl_list_head (test_modules);
                while (dl_item != INITPTR) {
                        free ((char *)dl_list_data (dl_item));
                        dl_list_remove_it (dl_item);
                        dl_item = dl_list_head (test_modules);
                }
        }

        dl_item = dl_list_head (ec_settings.search_dirs);
        while (dl_item != INITPTR) {
                /* get data from list iterator */
                dir = (char *)dl_list_data (dl_item);

                /* return the number of directory entries */
                n = scandir (dir, &namelist, 0, alphasort);

                /* add .so files to test sets list */
                while (n--) {
                        ptr = strrchr (namelist[n]->d_name, '.');
                        if ((ptr != NULL) && (strcmp (ptr, ".so") == 0)) {
                                test_module = (char *)
                                    malloc (sizeof (char) *
                                            (strlen (namelist[n]->d_name) +
                                             1));
                                strcpy (test_module, namelist[n]->d_name);
                                dl_list_add (test_modules,
                                             (void *)test_module);
                                i++;
                        }
                        free (namelist[n]);
                }
                free (namelist);

                /* get next item */
                dl_item = dl_list_next (dl_item);
        }

        n = i;
        i = 0;

        if (n > 0) {
                /* allocate memory for n+1 items */
                cb_add_test_module_menu =
                    (callback_s *) calloc (n + 1, sizeof (callback_s));

                if (cb_add_test_module_menu == NULL)
                        return -1;

                /* get head of test sets list */
                dl_item = dl_list_head (test_modules);

                while (dl_item != INITPTR) {
                        set_cbs (&cb_add_test_module_menu[i],
                                 (char *)dl_list_data (dl_item),
                                 NULL,
                                 add_test_case_files_menu,
                                 module_menu, save_module_name, dl_item, 0);
                        i++;

                        /* next item in list */
                        dl_item = dl_list_next (dl_item);
                }
        } else {
                /* allocate memory for empty menu */
                cb_add_test_module_menu =
                    (callback_s *) calloc (2, sizeof (callback_s));

                if (cb_add_test_module_menu == NULL)
                        return -1;

                set_cbs (&cb_add_test_module_menu[i],
                         "",
                         NULL,
                         add_test_case_files_menu, module_menu, NULL, NULL,
                         0);
                i++;
        }
#endif
        /* last menu item should be NULL one */
        null_cbs (&cb_add_test_module_menu[i]);

        return 0;
}

/* ------------------------------------------------------------------------- */
/** Get test case files in specific dir and creates add test case file menu
 *  @return 0 in case of success, returns -1 in case of failure.
 */
LOCAL int get_test_case_files (void)
{
        DLListItem     *dl_item = INITPTR;
        char           *tc_file = INITPTR;
        struct dirent **namelist;
        int             n = 0;
        int             i = 0;
        char           *dir = INITPTR;
	FILE           *shell_pipe = INITPTR;
	char           *shell_io = INITPTR;
	TSBool          addcasefile = ESTrue;
        /*char* ptr = INITPTR; to remove compiler warning */

        /* free memory allocated for callback structure */
        free_cbs (cb_add_test_case_files_menu);

        /* remove items from current test sets list */
        if (dl_list_size (test_case_files) != -1) {
                dl_item = dl_list_head (test_case_files);
                while (dl_item != INITPTR) {
                        free ((char *)dl_list_data (dl_item));
                        dl_list_remove_it (dl_item);
                        dl_item = dl_list_head (test_case_files);
                }
        }
#if 0
        dl_item = dl_list_head (ec_settings.search_dirs);
        while (dl_item != INITPTR) {
                /* get data from list iterator */
                dir = (char *)dl_list_data (dl_item);

                /* return the number of directory entries */
                n = scandir (dir, &namelist, 0, alphasort);

                /* add files to test sets list */
                while (n--) {
                        /* only normal files, not dots (. and ..) 
			 * and basic filter for non-case-files */
                    if ((strcmp (namelist[n]->d_name, ".") != 0) &&
                        (strcmp (namelist[n]->d_name, "..") != 0) &&
		        (strncmp (namelist[n]->d_name + 
		            strlen(namelist[n]->d_name) - 3
			    , ".la", 3) != 0) &&
			(strncmp (namelist[n]->d_name +
			    strlen(namelist[n]->d_name) - 1
			    , "~", 1) !=0 )) {
			    
			    addcasefile=ESTrue; /* restore flag value */

			    shell_io = malloc (sizeof(char) *
				(strlen ( "which file 2>/dev/null" ) +1) );
			    strcpy(shell_io, "which file 2>/dev/null");
			    if ((shell_pipe=popen(shell_io, "r")) != NULL ) {
			        free(shell_io);
				shell_io = malloc (sizeof(char) * 1);
				*shell_io = (char)fgetc(shell_pipe);
				pclose(shell_pipe);
				if (strncmp (shell_io, "/", 1) == 0){
			    	    free(shell_io);
			    	    shell_io = malloc (sizeof(char) *
					(strlen (namelist[n]->d_name) +
				         strlen (dir) +
				         strlen ("file -i -b ") +
				         2 +
				         strlen (" 2>/dev/null")));
			            strcpy (shell_io, "file -i -b ");
			            strcat (shell_io, dir );
			            strcat (shell_io, "/" );
			            strcat (shell_io, namelist[n]->d_name );
			            strcat (shell_io, " 2>/dev/null" );
			            if ((shell_pipe = popen (shell_io , "r"))
						    != NULL) {
				        free (shell_io);
				        shell_io = malloc (sizeof(char) * 5);
				        fgets (shell_io, 5, shell_pipe);
				        *(shell_io+4)=0x00;
				        if (strcmp(shell_io, "text" ) != 0) {
					    /* don't add file if not text*/
                                            addcasefile=ESFalse;
				        }
				        pclose (shell_pipe);
			            }
				} 
			    } 

			    /* add file to the list if it's not marked to omit
			     * by previous checks */
			    if(addcasefile==ESTrue){
			            tc_file = (char *)
                               	        malloc (sizeof (char) *
                               	        (strlen (namelist[n]->d_name) +
                               	        1));
                                    strcpy (tc_file,
				        namelist[n]->d_name);
                                    dl_list_add (test_case_files,
                               	        (void *)tc_file);
			            i++;
			    }

	                    free (shell_io);			
                    }
                    free (namelist[n]);
                }
                free (namelist);

                /* get next item */
                dl_item = dl_list_next (dl_item);
        }

        n = i;
        i = 0;

        if (n > 0) {
                /* allocate memory for n+1 items */
                cb_add_test_case_files_menu =
                    (callback_s *) calloc (n + 2, sizeof (callback_s));

                if (cb_add_test_case_files_menu == NULL)
                        return -1;

                set_cbs (&cb_add_test_case_files_menu[i],
                         "Add selected test case file(s)",
                         NULL,
                         module_menu,
                         add_test_module_menu, add_module, NULL, 0);
                i++;

                /* get head of test sets list */
                dl_item = dl_list_head (test_case_files);

                while (dl_item != INITPTR) {
                        set_cbs (&cb_add_test_case_files_menu[i],
                                 (char *)dl_list_data (dl_item),
                                 NULL,
                                 toggle_menu_item,
                                 add_test_module_menu,
                                 NULL, (char *)dl_list_data (dl_item), 1);
                        i++;

                        /* next item in list */
                        dl_item = dl_list_next (dl_item);
                }
        } else {
                /* allocate memory for empty menu */
                cb_add_test_case_files_menu =
                    (callback_s *) calloc (2, sizeof (callback_s));

                if (cb_add_test_case_files_menu == NULL)
                        return -1;

                set_cbs (&cb_add_test_case_files_menu[i],
                         "", NULL, NULL, add_test_module_menu, NULL, NULL, 0);
                i++;
        }
#endif
        /* last menu item should be NULL one */
        null_cbs (&cb_add_test_case_files_menu[i]);

        return 0;
}

/* ------------------------------------------------------------------------- */
/** Gets test modules for populating modules menu
 *  @return 0 in case of success, returns -1 in case of failure.
 */
LOCAL int get_loaded_modules ()
{
        DLListItem     *dl_item_tm = INITPTR;
        test_module_info_s *tmi = INITPTR;
        int             n = 0;
        int             i = 0;

        /* free memory allocated for callback structure */
        free_cbs (cb_module_menu);

        if (available_modules != INITPTR && available_modules != NULL)
                /* get a number of available test modules */
                n = dl_list_size (available_modules);

        if (n > 0) {
                /* allocate memory for n+1 items */
                cb_module_menu =
                    (callback_s *) calloc (n + 2, sizeof (callback_s));

                if (cb_module_menu == NULL)
                        return -1;

                set_cbs (&cb_module_menu[i], "Add test module", NULL,
                         add_test_module_menu, main_menu, NULL, NULL, 0);
                i++;
                /* get head of linked list including available modules */
                dl_item_tm = dl_list_head (available_modules);

                while (dl_item_tm != INITPTR) {
                        /* get test_module_info from linked list iterator */
                        tmi =
                            (test_module_info_s *) dl_list_data (dl_item_tm);

                        if (tmi != INITPTR && tmi->module_filename_ != NULL) {
                                /* fill menu structure with item data */
                                set_cbs (&cb_module_menu[i],
                                         tmi->module_filename_,
                                         NULL, NULL, main_menu, NULL, NULL,
                                         1);
                                i++;
                        }
                        /* get next module */
                        dl_item_tm = dl_list_next (dl_item_tm);
                }
        } else {
                /* allocate memory for empty menu */
                cb_module_menu =
                    (callback_s *) calloc (2, sizeof (callback_s));

                if (cb_module_menu == NULL)
                        return -1;

                i = 0;
                set_cbs (&cb_module_menu[i], "Add test module", NULL,
                         add_test_module_menu, main_menu, NULL, NULL, 0);
                i++;
        }

        /* last menu item should be NULL one */
        null_cbs (&cb_module_menu[i]);

        return 0;
}

/* ------------------------------------------------------------------------- */
/** Gets test cases for populating start new case menu
 *  @return 0 in case of success, returns -1 in case of failure.
 */
LOCAL int get_tcs_for_start_new_case ()
{
        DLListItem     *dl_item_tm = INITPTR;
        DLListItem     *dl_item_tc = INITPTR;
        DLList         *dl_list_tc = INITPTR;
        CUICaseData    *tc = INITPTR;
        int             n = 0;
        int             i = 0;

        /* free memory allocated for callback structure */
        free_cbs (cb_start_new_case_menu);

        /* count the number of all test cases */
        n = dl_list_size (case_list_);

        if (n > 0) {
                /* allocate memory for n+1 items */
                cb_start_new_case_menu = NEW2(callback_s,n+1);

                if (cb_start_new_case_menu == NULL) return -1;

                dl_item_tc = dl_list_head(case_list_);
                while (dl_item_tc!=DLListNULLIterator) {
                        /* get tc title and so on */
                        tc = (CUICaseData*)dl_list_data (dl_item_tc);
                        if (tc == INITPTR || tc->casetitle_ == INITPTR) {
                                continue;
                        }
                        /* fill callback structure with data */
                        set_cbs (&cb_start_new_case_menu[i],
                                tx_get_buf(tc->casetitle_),
                                NULL,case_menu, case_menu,start_one_tc,
                                dl_item_tc, 0);
                        i++;
                        dl_item_tc = dl_list_next(dl_item_tc);
                }
        } else {
                /* allocate memory for empty menu */
                cb_start_new_case_menu = NEW2(callback_s,2);
                if (cb_start_new_case_menu == NULL) return -1;
                i = 0;
                set_cbs (&cb_start_new_case_menu[i],
                         "", NULL, NULL, case_menu, NULL, NULL, 0);
                i++;
        }

        /* last menu item should be NULL one */
        null_cbs (&cb_start_new_case_menu[i]);

        return 0;
}

/* ------------------------------------------------------------------------- */
/** Gets test cases for populating run multiple tests menu
 *  @return 0 in case of success, returns -1 in case of failure.
 */
LOCAL int get_tcs_for_run_multiple_tests ()
{
        DLListItem     *dl_item_tm = INITPTR;
        DLListItem     *dl_item_tc = INITPTR;
        DLList         *dl_list_tc = INITPTR;
        test_case_s    *tc = INITPTR;
        int             n = 0;
        int             i = 0;

        /* free memory allocated for callback structure */
        free_cbs (cb_run_multiple_tests_menu);

        /* count the number of all test cases */
        if (available_modules != INITPTR && available_modules != NULL) {
                /* get head of linked list including available modules */
                dl_item_tm = dl_list_head (available_modules);

                while (dl_item_tm != INITPTR) {
                        /* get test case list of current test module */
                        dl_list_tc = tm_get_tclist (dl_item_tm);

                        if (dl_list_tc != NULL && dl_list_tc != INITPTR) {
                                i = dl_list_size (dl_list_tc);
                                if (i != -1)
                                        n += i;
                        }

                        /* get next module */
                        dl_item_tm = dl_list_next (dl_item_tm);
                }
        }
        i = 0;

        if (n > 0) {
                /* allocate memory for n+1 items */
                cb_run_multiple_tests_menu =
                    (callback_s *) calloc (n + 1 + 4, sizeof (callback_s));

                if (cb_run_multiple_tests_menu == NULL)
                        return -1;

                set_cbs (&cb_run_multiple_tests_menu[i],
                         "Run selection sequentially",
                         NULL,
                         case_menu,
                         case_menu, start_cases_sequentially, NULL, 0);
                i++;

                set_cbs (&cb_run_multiple_tests_menu[i],
                         "Run selection parallel",
                         NULL,
                         case_menu, case_menu, start_cases_parallel, NULL, 0);
                i++;

                set_cbs (&cb_run_multiple_tests_menu[i],
                         "Change all",
                         NULL,
                         invert_select_tcs_selection,
                         case_menu, NULL, NULL, 0);
                i++;

                /* process linked list including available modules */
                for (dl_item_tm = dl_list_head (available_modules);
                     dl_item_tm != INITPTR;
                     dl_item_tm = dl_list_next (dl_item_tm)) {
                        /* get test case list of current test module */
                        dl_list_tc = tm_get_tclist (dl_item_tm);

                        if (dl_list_tc == INITPTR || dl_list_tc ==  NULL)
                                continue;
                        /* get head of test cases linked list */
                        for (dl_item_tc = dl_list_head (dl_list_tc);
                             dl_item_tc != INITPTR;
                             dl_item_tc = dl_list_next (dl_item_tc)) {
                                /* get test_case_s from iterator */
                                tc = (test_case_s *)
                                    dl_list_data (dl_item_tc);

                                if (tc == INITPTR || tc->title_ == NULL)
                                        continue;
                                /* fill callback structure */
                                set_cbs (&cb_run_multiple_tests_menu[i],
                                         tc->title_, NULL,
                                         toggle_menu_item,
                                         case_menu, NULL, tc, 1);
                                i++;

                        }
                }
        } else {
                /* allocate memory for empty menu */
                cb_run_multiple_tests_menu =
                    (callback_s *) calloc (1 + 3, sizeof (callback_s));

                if (cb_run_multiple_tests_menu == NULL)
                        return -1;

                i = 0;
                set_cbs (&cb_run_multiple_tests_menu[i],
                         "Run selection sequentially",
                         NULL, NULL, case_menu, NULL, NULL, 0);
                i++;
                set_cbs (&cb_run_multiple_tests_menu[i],
                         "Run selection parallel",
                         NULL, NULL, case_menu, NULL, NULL, 0);
                i++;
                set_cbs (&cb_run_multiple_tests_menu[i],
                         "Change all", NULL, NULL, case_menu, NULL, NULL, 0);
                i++;
        }

        /* last menu item should be NULL one */
        null_cbs (&cb_run_multiple_tests_menu[i]);

        return 0;
}

/* ------------------------------------------------------------------------- */
/** Gets ongoing and paused cases for populating ongoing cases menu
 *  @return 0 in case of success, returns -1 in case of failure.
 */
LOCAL int get_ongoing_cases ()
{
        DLListItem       *dl_item_tc = INITPTR;
        ExecutedTestCase *etc = INITPTR;
        int               n = 0;
        int               i = 0;

        /* free memory allocated for callback structure */
        free_cbs (cb_ongoing_cases_menu);

        /* count the number of ongoing test cases */
        if (executed_case_list_ != INITPTR && executed_case_list_ != NULL) {
                /* get head of linked list including available modules */
                dl_item_tc = dl_list_head (executed_case_list_);

                while (dl_item_tc != INITPTR) {
		        etc = (ExecutedTestCase *)dl_list_data (dl_item_tc);
                        /* check if test case's status is ongoing or paused */
                        if (etc->status_ == TCASE_STATUS_ONGOING ||
                            etc->status_  == TCASE_STATUS_PAUSED)
			      n++;
                        /* get next test case */
                        dl_item_tc = dl_list_next (dl_item_tc);
                }
        }

        if (n > 0) {
                /* allocate memory for n+1 items */
                cb_ongoing_cases_menu =
                    (callback_s *) calloc (n + 1, sizeof (callback_s));

                if (cb_ongoing_cases_menu == NULL)
                        return -1;

                /* process linked list including available modules */
                for (dl_item_tc = dl_list_head (executed_case_list_);
                     dl_item_tc != INITPTR;
                     dl_item_tc = dl_list_next (dl_item_tc)) {
		        etc = (ExecutedTestCase *)dl_list_data (dl_item_tc);
                        if (etc->status_ != TCASE_STATUS_ONGOING &&
                            etc->status_ != TCASE_STATUS_PAUSED)
                                continue;
                        /* get test_case_s from linked list iterator */
                        etc = dl_list_data (dl_item_tc);
                        if (etc == INITPTR || etc->case_->casetitle_ == NULL)
                                continue;

                        switch (etc->status_) {

                        case TCASE_STATUS_ONGOING:
                                /* fill callback structure with data */
                                set_cbs (&cb_ongoing_cases_menu [i], 
					 tx_share_buf (etc->case_->casetitle_),
                                         "(ongoing)", NULL,
                                         case_menu,
                                         pause_resume_abort_menu,
                                         dl_item_tc, 0);
                                i++;
                                break;

                        case TCASE_STATUS_PAUSED:
                                /* fill callback structure with data */
                                set_cbs (&cb_ongoing_cases_menu [i], 
					 tx_share_buf (etc->case_->casetitle_),
                                         "(paused)", NULL,
                                         case_menu,
                                         pause_resume_abort_menu,
                                         dl_item_tc, 0);
                                i++;
                                break;

                        default:
                                break;
                        }
                }
        } else {
                /* allocate memory for empty menu */
                cb_ongoing_cases_menu =
                    (callback_s *) calloc (2, sizeof (callback_s));

                if (cb_ongoing_cases_menu == NULL)
                        return -1;

                i = 0;
                set_cbs (&cb_ongoing_cases_menu[i],
                         "", NULL, NULL, case_menu, NULL, NULL, 0);

                i++;
        }

        /* last menu item should be NULL one */
        null_cbs (&cb_ongoing_cases_menu[i]);

        return 0;
}
/* ------------------------------------------------------------------------- */

LOCAL void _find_case_by_result (const void *a, const void *b)
{
        ExecutedTestCase *tmp1 = (ExecutedTestCase*)a;
        unsigned *tmp2 = (unsigned*)b;

        if (tmp1->result_==(*tmp2)) return 0;
        else return -1;
}

/* ------------------------------------------------------------------------- */
/** Gets specific test results
 *  @param cb pointer to menu callback structure
 *  @param result_type test result type to fetch
 *  @return 0 in case of success, returns -1 in case of failure.
 */
LOCAL int get_cases_by_result_type (callback_s ** cb, int result_type)
{
        int             n = 0;
        int             i = 0;

        DLListItem     *dl_item_tm = INITPTR;
        DLListItem     *dl_item_tc = INITPTR;
        DLListItem     *dl_item_tr = INITPTR;
        DLList         *dl_list_tc = INITPTR;
        DLList         *dl_list_tr = INITPTR;
        test_case_s    *tc = INITPTR;
        test_result_s  *tr = INITPTR;

        /* free memory allocated for callback structure */
        free_cbs (*cb);


        DLListIterator it = DLListNULLIterator;
        DLListIterator begin = dl_list_head (executed_case_list_);

        do {
                it = dl_list_find (begin,
                                dl_list_tail (executed_case_list_),
                                _find_case_by_result,
                                result_type);
                if (it==DLListNULLIterator) break;



                begin = dl_list_next(it);

        } while (it!=DLListNULLIterator);


        if (instantiated_modules == INITPTR || instantiated_modules == NULL)
                goto empty_menu;






        /* process linked list including test modules */
        for (dl_item_tm = dl_list_head (instantiated_modules);
             dl_item_tm != INITPTR; dl_item_tm = dl_list_next (dl_item_tm)) {
                /* get test case list of the test module */
                dl_list_tc = tm_get_tclist (dl_item_tm);

                if (dl_list_tc == INITPTR || dl_list_tc == NULL)
                        continue;

                /* process linked list including test cases */
                for (dl_item_tc = dl_list_head (dl_list_tc);
                     dl_item_tc != INITPTR;
                     dl_item_tc = dl_list_next (dl_item_tc)) {
                        /* get test result list of the test case */
                        dl_list_tr = tc_get_tr_list (dl_item_tc);

                        if (dl_list_tr == INITPTR || dl_list_tr == NULL)
                                continue;
                        /* process linked list including test results */

                        for (dl_item_tr = dl_list_head (dl_list_tr);
                             dl_item_tr != INITPTR;
                             dl_item_tr = dl_list_next (dl_item_tr)) {
                                tc = (test_case_s *)
                                    dl_list_data (dl_item_tc);

                                if (tc == INITPTR || tc->title_ == NULL)
                                        continue;
                                /* for getting all executed cases */
                                if ((result_type == TEST_RESULT_ALL) &&
                                    (tr_get_result_type (dl_item_tr) !=
                                     TEST_RESULT_NOT_RUN))
                                        n++;
                                /* for aborted & crashed cases */
                                else if (result_type == TEST_RESULT_ABORTED &&
                                         (tr_get_result_type (dl_item_tr) ==
                                          TEST_RESULT_ABORTED ||
                                          tr_get_result_type (dl_item_tr) ==
                                          TEST_RESULT_CRASHED))
                                        n++;
                                /* for passed or failed cases */
                                else if (tr_get_result_type (dl_item_tr) ==
                                         result_type)
                                        n++;
                        }
                }
        }


        if (n == 0)
                goto empty_menu;

        /* allocate memory for n+1 items */
        *cb = (callback_s *) calloc (n + 1, sizeof (callback_s));
        if (*cb == NULL)
                return -1;

        /* process linked list including test modules */
        for (dl_item_tm = dl_list_head (instantiated_modules);
             dl_item_tm != INITPTR; dl_item_tm = dl_list_next (dl_item_tm)) {
                /* get test case list of the test module */
                dl_list_tc = tm_get_tclist (dl_item_tm);

                if (dl_list_tc == INITPTR || dl_list_tc == NULL)
                        continue;

                /* process linked list including test cases */
                for (dl_item_tc = dl_list_head (dl_list_tc);
                     dl_item_tc != INITPTR;
                     dl_item_tc = dl_list_next (dl_item_tc)) {
                        /* get head of linked list including test results */
                        dl_list_tr = tc_get_tr_list (dl_item_tc);

                        if (dl_list_tr == INITPTR || dl_list_tr == NULL)
                                continue;
                        /*process linked list including test results */
                        for (dl_item_tr = dl_list_head (dl_list_tr);
                             dl_item_tr != INITPTR;
                             dl_item_tr = dl_list_next (dl_item_tr)) {
                                tc = (test_case_s *)
                                    dl_list_data (dl_item_tc);
                                tr = (test_result_s *)
                                    dl_list_data (dl_item_tr);

                                if (tc == INITPTR || tr == INITPTR ||
                                    tc->title_ == NULL)
                                        continue;

                                /*  getting all executed cases */
                                if ((result_type == TEST_RESULT_ALL) &&
                                    (tr_get_result_type (dl_item_tr) !=
                                     TEST_RESULT_NOT_RUN)) {
                                        set_cbs (&(*cb)[i],
                                                 tc->title_,
                                                 NULL,
                                                 NULL,
                                                 case_menu,
                                                 tr_for_executed_case, tr, 0);

                                        i++;

                                        /* getting aborted & crashed cases */
                                } else if (result_type == TEST_RESULT_ABORTED
                                           && (tr_get_result_type (dl_item_tr)
                                               == TEST_RESULT_ABORTED
                                               ||
                                               tr_get_result_type (dl_item_tr)
                                               == TEST_RESULT_CRASHED)) {

                                        set_cbs (&(*cb)[i],
                                                 tc->title_,
                                                 NULL,
                                                 NULL,
                                                 case_menu,
                                                 tr_for_aborted_case, tr, 0);

                                        i++;
                                        
                                        /*  getting passed or failed cases */
                                } else if (tr_get_result_type (dl_item_tr) ==
                                           result_type) {
					if (result_type == TEST_RESULT_PASSED) {
                                                set_cbs (&(*cb)[i],
                                                         tc->title_,
                                                         NULL,
                                                         NULL,
                                                         case_menu,
                                                         tr_for_passed_case,
                                                         tr, 0);

                                                i++;
                                        } else if (result_type ==
                                                   TEST_RESULT_FAILED) {

                                                set_cbs (&(*cb)[i],
                                                         tc->title_,
                                                         NULL,
                                                         NULL,
                                                         case_menu,
                                                         tr_for_failed_case,
                                                         tr, 0);
                                                i++;
                                        }
                                }
                        }
                }
        }



        /* last menu item should be NULL one */
        null_cbs (&(*cb)[i]);
        return 0;

      empty_menu:
        /* allocate memory for empty menu */
        *cb = (callback_s *) calloc (2, sizeof (callback_s));
        if (*cb == NULL)
                return -1;

        i = 0;
        set_cbs (&(*cb)[i], "", NULL, NULL, case_menu, NULL, NULL, 0);
        i++;


        /* last menu item should be NULL one */
        null_cbs (&(*cb)[i]);

        return 0;
}

/* ------------------------------------------------------------------------- */
/** Shows modules menu 
 */
LOCAL void module_menu ()
{
        if (get_loaded_modules () != -1)
                /* Show new menu */
                update_menu (cb_module_menu, "Module menu", 0,
                             &module_menu_focus);
}

/* ------------------------------------------------------------------------- */
/** Shows start new case menu
 */
LOCAL void start_new_case_menu ()
{
        if (get_tcs_for_start_new_case () != -1)
                /* Show new menu */
                update_menu (cb_start_new_case_menu, "Start new case", 0,
                             &start_new_case_menu_focus);
}

/* ------------------------------------------------------------------------- */
/** Shows run multiple tests menu
 */
LOCAL void run_multiple_tests_menu ()
{
        if (get_tcs_for_run_multiple_tests () != -1)
                /* Show new menu */
                update_menu (cb_run_multiple_tests_menu,
                             "Run multiple tests", 0,
                             &run_multiple_tests_menu_focus);
}

/* ------------------------------------------------------------------------- */
/** Shows ongoing cases menu
 */
LOCAL void ongoing_cases_menu ()
{
        if (get_ongoing_cases () != -1)
                /* Show new menu */
                update_menu (cb_ongoing_cases_menu, "Ongoing cases", 1,
                             &ongoing_cases_menu_focus);
}

/* ------------------------------------------------------------------------- */
/** Shows executed cases menu
 */
LOCAL void executed_cases_menu ()
{
        if (get_cases_by_result_type (&cb_executed_cases_menu,
                                      TEST_RESULT_ALL) != -1)
                /* Show new menu */
                update_menu (cb_executed_cases_menu, "Executed cases", 1,
                             &executed_cases_menu_focus);
}

/* ------------------------------------------------------------------------- */
/** Shows passed cases menu
 */
LOCAL void passed_cases_menu ()
{
        if (get_cases_by_result_type (&cb_passed_cases_menu,
                                      TEST_RESULT_PASSED) != -1)
                /* Show new menu */
                update_menu (cb_passed_cases_menu, "Passed cases", 1,
                             &passed_cases_menu_focus);
}

/* ------------------------------------------------------------------------- */
/** Shows failed cases menu
 */
LOCAL void failed_cases_menu ()
{
        if (get_cases_by_result_type (&cb_failed_cases_menu,
                                      TEST_RESULT_FAILED) != -1)
                /* Show new menu */
                update_menu (cb_failed_cases_menu, "Failed cases", 1,
                             &failed_cases_menu_focus);
}

/* ------------------------------------------------------------------------- */
/** Shows aborted cases menu
 */
LOCAL void aborted_cases_menu ()
{
        if (get_cases_by_result_type (&cb_aborted_cases_menu,
                                      TEST_RESULT_ABORTED) != -1)
                /* Show new menu */
                update_menu (cb_aborted_cases_menu,
                             "Aborted/Crashed cases", 1,
                             &aborted_cases_menu_focus);
}

/* ------------------------------------------------------------------------- */
/** Shows test results for executed test case
 *  @param p pointer to test result
 */
LOCAL void tr_for_executed_case (void *p)
{
        test_result_menu (p, executed_cases_menu);
}

/* ------------------------------------------------------------------------- */
/** Shows test results for passed test case
 *  @param p pointer to test result
 */
LOCAL void tr_for_passed_case (void *p)
{
        test_result_menu (p, passed_cases_menu);
}

/* ------------------------------------------------------------------------- */
/** Shows aborted cases menu
 *  @param p pointer to test result
 */
LOCAL void tr_for_failed_case (void *p)
{
        test_result_menu (p, failed_cases_menu);
}

/* ------------------------------------------------------------------------- */
/** Shows test results for aborted test case
 *  @param p pointer to test result
 */
LOCAL void tr_for_aborted_case (void *p)
{
        test_result_menu (p, aborted_cases_menu);
}

/* ------------------------------------------------------------------------- */
/** Shows test result menu
 *  @param p pointer to test result
 *  @param on_left pointer to menu function
 *  @return 0 in case of success, returns -1 in case of failure.
 */
LOCAL int test_result_menu (void *p, ptr_to_fun on_left)
{
        DLListItem     *dl_item_tc = INITPTR;
        test_result_s  *tr = INITPTR;
        test_case_s    *tc = INITPTR;
        static void    *s_p = INITPTR;
        static ptr_to_fun s_on_left = INITPTR;

        if (p != INITPTR)
                s_p = p;
        if (on_left != INITPTR)
                s_on_left = on_left;

        /* check pointers */
        if (s_p != INITPTR) {
                tr = (test_result_s *) s_p;
                if (tr != INITPTR) {
                        dl_item_tc = tr->tc_data_item_;
                        if (dl_item_tc != INITPTR && dl_item_tc != NULL)
                                tc = (test_case_s *)
                                    dl_list_data (dl_item_tc);
                        else
                                return -1;
                } else
                        return -1;
        } else
                return -1;

        /* free memory allocated for callback structure */
        free_cbs (cb_test_result_menu);

        /* allocate memory for menu callback structure */
        cb_test_result_menu = (callback_s *) calloc (2, sizeof (callback_s));

        if (cb_test_result_menu != NULL) {
                set_cbs (&cb_test_result_menu[0],
                         "View output",
                         NULL, NULL, s_on_left, view_output, s_p, 0);

                /* last menu item should be NULL one */
                null_cbs (&cb_test_result_menu[1]);

                /* add title to menu */
                if (tc->title_ != INITPTR && tc->title_ != NULL)
                        /* Show new menu */
                        update_menu (cb_test_result_menu, tc->title_, 1, NULL);
                else
                        /* Show new menu */
                        update_menu (cb_test_result_menu, "", 1, NULL);

                /* show test results */
                test_result_view (s_p);
        } else
                return -1;

        return 0;
}

/* ------------------------------------------------------------------------- */
/** Shows test results on window
 *  @param p pointer to test result
 */
LOCAL void test_result_view (void *p)
{
        test_result_s  *tr = INITPTR;
        struct tm      *timeinfo = INITPTR;
        char            result[20];
        char            buffer_start_time[20];
        char            buffer_end_time[20];

        if (p != INITPTR) {
                tr = (test_result_s *) p;

                if (tr != INITPTR) {
                        switch (tr->result_type_) {
                        case TEST_RESULT_PASSED:
                                strcpy (result, "Passed");
                                break;
                        case TEST_RESULT_FAILED:
                                strcpy (result, "Failed");
                                break;
                        case TEST_RESULT_CRASHED:
                                strcpy (result, "Crashed");
                                break;
                        case TEST_RESULT_ABORTED:
                                strcpy (result, "Aborted");
                                break;
                        case TEST_RESULT_TIMEOUT:
                                strcpy (result, "Timeout");
                                break;
                        default:
                                strcpy (result, "");
                                break;
                        }

                        /* change times to more readible form */
                        timeinfo = localtime ((time_t *) & (tr->start_time_));
                        strftime (buffer_start_time, 20, "%I:%M:%S %p",
                                  timeinfo);

                        if (tr->end_time_ != 0) {
                                timeinfo = localtime ((time_t *)
                                                      & (tr->end_time_));
                                strftime (buffer_end_time, 20,
                                          "%I:%M:%S %p", timeinfo);
                        } else
                                strcpy (buffer_end_time, "");

                        /* print test results to screen */
                        mvwprintw (menu_window, 2, 0,
                                   "Result info: %s", result);
                        mvwprintw (menu_window, 3, 0,
                                   "Result descr: %s",
                                   tr->result_description_);
                        mvwprintw (menu_window, 4, 0, "Started: %s",
                                   buffer_start_time);
                        mvwprintw (menu_window, 5, 0, "Completed: %s",
                                   buffer_end_time);
                }
        }
}

/* ------------------------------------------------------------------------- */
/** Shows pause, resume, abort test case menu
 */
LOCAL void pause_resume_abort_menu (void *p)
{
        DLListItem     *dl_item_tc = INITPTR;
        test_case_s    *tc = INITPTR;
        int             quit = 0;
        static void    *s_p = INITPTR;

        if (p != INITPTR)
                s_p = p;

        /* check pointers */
        if (s_p != INITPTR) {
                dl_item_tc = (DLListItem *) s_p;
                if (dl_item_tc != INITPTR && dl_item_tc != NULL) {
                        tc = (test_case_s *) dl_list_data (dl_item_tc);
                        if (tc == INITPTR || tc == NULL)
                                quit = 1;
                } else
                        quit = 1;
        } else
                quit = 1;

        /* free memory allocated for callback structure */
        free_cbs (cb_pause_resume_abort_menu);

        if (quit) {

                cb_pause_resume_abort_menu =
                    (callback_s *) calloc (2, sizeof (callback_s));

                set_cbs (&cb_pause_resume_abort_menu[0],
                         "View output",
                         NULL,
                         NULL,
                         ongoing_cases_menu,
                         view_output_for_ongoing_cases, s_p, 0);

                /* last menu item should be NULL one */
                null_cbs (&cb_pause_resume_abort_menu[1]);

                /* Show new menu */
                update_menu (cb_pause_resume_abort_menu, title, 0, NULL);

        } else {

                cb_pause_resume_abort_menu =
                    (callback_s *) calloc (4, sizeof (callback_s));

                if (cb_pause_resume_abort_menu != NULL) {

                        set_cbs (&cb_pause_resume_abort_menu[0],
                                 "View output",
                                 NULL,
                                 NULL,
                                 ongoing_cases_menu,
                                 view_output_for_ongoing_cases, s_p, 0);

                        switch (tc_get_status (dl_item_tc)) {
                        case TEST_CASE_ONGOING:
                                set_cbs (&cb_pause_resume_abort_menu[1],
                                         "Pause",
                                         NULL,
                                         ongoing_cases_menu,
                                         ongoing_cases_menu, pause_tc, s_p,
                                         0);
                                break;
                        case TEST_CASE_PAUSED:
                                set_cbs (&cb_pause_resume_abort_menu[1],
                                         "Resume",
                                         NULL,
                                         ongoing_cases_menu,
                                         ongoing_cases_menu,
                                         resume_tc, s_p, 0);
                                break;
                        default:
                                break;
                        }

                        set_cbs (&cb_pause_resume_abort_menu[2],
                                 "Abort case",
                                 NULL,
                                 ongoing_cases_menu,
                                 ongoing_cases_menu, abort_tc, s_p, 0);

                        /* last menu item should be NULL one */
                        null_cbs (&cb_pause_resume_abort_menu[3]);

                        if (tc->title_ != INITPTR && tc->title_ != NULL) {
                                strcpy (title, tc->title_);
                                /* Show new menu */
                                update_menu (cb_pause_resume_abort_menu,
                                             tc->title_, 1, NULL);
                        } else
                                /* Show new menu */
                                update_menu (cb_pause_resume_abort_menu,
                                             title, 1, NULL);
                }
        }
}

/* ------------------------------------------------------------------------- */
/** Updates quit flag
 */
LOCAL void quit_program ()
{
        continue_ = false;
}

/* ------------------------------------------------------------------------- */
/** Selects or deselects menu item
 */
LOCAL void toggle_menu_item ()
{
        menu_driver (my_menu, REQ_TOGGLE_ITEM);
}

/* ------------------------------------------------------------------------- */
/** Inverts menu item selection
 */
LOCAL void invert_select_tcs_selection ()
{
        int             i = 0;
        ITEM          **items = INITPTR;

        items = menu_items (my_menu);

        for (i = 3; i < item_count (my_menu); ++i)
                set_item_value (items[i], !item_value (items[i]));
}

/* ------------------------------------------------------------------------- */
/** Pauses test case execution
 *  @param p pointer to DLListIterator
 */
LOCAL void pause_tc (void *p)
{
        //ec_pause_test_case ((DLListIterator) p);
}

/* ------------------------------------------------------------------------- */
/** Resumes test case execution
 *  @param p pointer to DLListIterator
 */
LOCAL void resume_tc (void *p)
{
        //ec_resume_test_case ((DLListIterator) p);
}

/* ------------------------------------------------------------------------- */
/** Aborts tes case execution
 *  @param p pointer to DLListIterator
 */
LOCAL void abort_tc (void *p)
{
        //ec_abort_test_case ((DLListIterator) p);
}

/* ------------------------------------------------------------------------- */
/** Starts executing of one test case
 */
LOCAL void start_one_tc (void *p)
{
        //ec_exec_test_case ((DLListIterator) p);
        DLListIterator it = (DLListIterator)p;
        CUICaseData *c = (CUICaseData*)dl_list_data(it);
        if (min_clbk_.start_case) min_clbk_.start_case(c->moduleid_,
                                                        c->caseid_);
}

/* ------------------------------------------------------------------------- */
/** Starts executing test cases sequentially
 */
LOCAL void start_cases_sequentially (void *p)
{
        get_selected_cases ();
        //ec_run_cases_seq (user_selected_cases);
}

/* ------------------------------------------------------------------------- */
/** Starts executing test cases parallel
 */
LOCAL void start_cases_parallel (void *p)
{
        get_selected_cases ();
        // ec_run_cases_par (user_selected_cases);
}

/* ------------------------------------------------------------------------- */
/** Collects user selected test cases from menu and adds them to linked list
 */
LOCAL void get_selected_cases ()
{
        int             i = 0;
        ITEM          **items = NULL;
        callback_s     *cb = INITPTR;

        items = menu_items (my_menu);

        /* iterate through menu items to find highlighted ones */
        for (i = 3; i < item_count (my_menu); ++i) {
                /* test if menu item is highlighted */
                if (item_value (items[i]) == TRUE) {
                        /* get user data attached to menu item */
                        cb = (callback_s *) item_userptr (items[i]);
                        /* add menu item (test case) to linked list */
                        dl_list_add (user_selected_cases, cb->ptr_data);
                }
        }
}

/* ------------------------------------------------------------------------- */
/** Goes back to test result menu
 */
LOCAL void back_to_tr_menu ()
{
        test_result_menu (INITPTR, INITPTR);
}

/* ------------------------------------------------------------------------- */
/** Goes back to test case controlling menu
 */
LOCAL void back_to_control_menu ()
{
        pause_resume_abort_menu (INITPTR);
}

/* ------------------------------------------------------------------------- */
/** Views test case output
 *  @param p pointer to test_result_s
 */
LOCAL void view_output (void *p)
{
        DLListItem     *dl_item_tc = INITPTR;
        DLListItem     *dl_item_po = INITPTR;
        test_result_s  *tr = INITPTR;
        test_case_s    *tc = INITPTR;
        test_result_printout_s *printout = INITPTR;
        int             n = 0;

        if (p != INITPTR && p != NULL) {
                tr = (test_result_s *) p;
                if (tr != INITPTR && tr != NULL) {
                        dl_item_tc = tr->tc_data_item_;
                        if (dl_item_tc != INITPTR && dl_item_tc != NULL)
                                tc = (test_case_s *)
                                    dl_list_data (dl_item_tc);
                        else
                                return;
                } else
                        return;
        } else
                return;

        /* free memory allocated for callback structure */
        free_cbs (cb_view_output_menu);

        /* get number of printouts */
        dl_item_po = dl_list_head (tr->printouts_list_);
        while (dl_item_po != INITPTR && dl_item_po != NULL) {
                n++;
                dl_item_po = dl_list_next (dl_item_po);
        }

        if (n > 0) {
                /* allocate memory for menu callback structure */
                cb_view_output_menu =
                    (callback_s *) calloc (n + 1, sizeof (callback_s));
                if (cb_view_output_menu == NULL)
                        return;

                dl_item_po = dl_list_head (tr->printouts_list_);
                n = 0;

                /* iterate through printouts */
                while (dl_item_po != INITPTR && dl_item_po != NULL) {
                        printout = (test_result_printout_s *)
                            dl_list_data (dl_item_po);

                        /* add printout to menu */
                        set_cbs (&cb_view_output_menu[n],
                                 printout->printout_ ? printout->printout_ : "<null>",
                                 NULL, NULL, back_to_tr_menu, NULL, NULL, 0);

                        n++;

                        /* move to next item */
                        dl_item_po = dl_list_next (dl_item_po);
                }
        } else {
                cb_view_output_menu =
                    (callback_s *) calloc (2, sizeof (callback_s));
                if (cb_view_output_menu == NULL)
                        return;

                n = 0;
                set_cbs (&cb_view_output_menu[n],
                         "", NULL, NULL, back_to_tr_menu, NULL, NULL, 0);

                n++;
        }

        /* last menu item should be NULL one */
        null_cbs (&cb_view_output_menu[n]);

        /* Show new menu */
        update_menu (cb_view_output_menu, tc->title_, 1, NULL);
}

/* ------------------------------------------------------------------------- */
/** Views test case output for ongoing test case
 *  @param p pointer to DLListItem
 */
LOCAL void view_output_for_ongoing_cases (void *p)
{
        DLListItem     *dl_item_tc = INITPTR;
        DLListItem     *dl_item_tr = INITPTR;
        DLListItem     *dl_item_po = INITPTR;
        DLList         *dl_list_tr = INITPTR;
        test_result_s  *tr = INITPTR;
        test_case_s    *tc = INITPTR;
        int             n = 0;
        test_result_printout_s *printout = INITPTR;

        /* check pointers */
        if (p != INITPTR) {
                dl_item_tc = (DLListItem *) p;
                if (dl_item_tc != INITPTR && dl_item_tc != NULL) {
                        dl_list_tr = tc_get_tr_list (dl_item_tc);
                        if (dl_list_tr != INITPTR && dl_list_tr != NULL) {
                                dl_item_tr = dl_list_head (dl_list_tr);
                                tr = (test_result_s *)
                                    dl_list_data (dl_item_tr);
                                tc = (test_case_s *)
                                    dl_list_data (dl_item_tc);
                        } else
                                return;
                } else
                        return;
        } else
                return;

        /* free memory allocated for callback structure */
        free_cbs (cb_view_output_menu);

        /* count number of printouts */
        dl_item_tr = dl_list_head (dl_list_tr);
        while (dl_item_tr != INITPTR) {
                tr = (test_result_s *) dl_list_data (dl_item_tr);
                if (tr->result_type_ == TEST_RESULT_NOT_RUN) {
                        dl_item_po = dl_list_head (tr->printouts_list_);
                        while (dl_item_po != INITPTR && dl_item_po != NULL) {
                                n++;
                                dl_item_po = dl_list_next (dl_item_po);
                        }
                }
                dl_item_tr = dl_list_next (dl_item_tr);
        }

        if (n > 0) {
                cb_view_output_menu = (callback_s *)
                    calloc (n + 1, sizeof (callback_s));
                if (cb_view_output_menu == NULL)
                        return;
                n = 0;

                /* get head of the test result list */
                dl_item_tr = dl_list_head (dl_list_tr);
                while (dl_item_tr != INITPTR) {
                        /* get test result */
                        tr = (test_result_s *) dl_list_data (dl_item_tr);
                        if (tr->result_type_ == TEST_RESULT_NOT_RUN) {
                                /* get head of prinouts list */
                                dl_item_po =
                                    dl_list_head (tr->printouts_list_);

                                /* iterate through list of printouts */
                                while (dl_item_po != INITPTR &&
                                       dl_item_po != NULL) {

                                        printout = (test_result_printout_s *)
                                            dl_list_data (dl_item_po);

                                        set_cbs (&cb_view_output_menu[n],
                                                 printout->printout_,
                                                 NULL,
                                                 NULL,
                                                 back_to_control_menu,
                                                 NULL, NULL, 0);

                                        n++;

                                        /* move to next printout item */
                                        dl_item_po =
                                            dl_list_next (dl_item_po);
                                }
                        }
                        /* move to next test result */
                        dl_item_tr = dl_list_next (dl_item_tr);
                }
        } else {
                cb_view_output_menu =
                    (callback_s *) calloc (2, sizeof (callback_s));

                if (cb_view_output_menu == NULL)
                        return;

                n = 0;
                set_cbs (&cb_view_output_menu[n],
                         "", NULL, NULL, back_to_control_menu, NULL, NULL, 0);

                n++;
        }

        /* last menu item should be NULL one */
        null_cbs (&cb_view_output_menu[n]);

        /* Show new menu */
        update_menu (cb_view_output_menu, tc->title_, 1, NULL);
}

/* ------------------------------------------------------------------------- */
/** Shows test set menu
 */
LOCAL void test_set_menu ()
{
        if (create_test_set_menu () != -1)
                /* Show new menu */
                update_menu (cb_test_set_menu,
                             "Create/modify test set menu", 0,
                             &test_set_menu_focus);
}

/* ------------------------------------------------------------------------- */
/** Creates test set menu
 *  @return 0 in case of success, returns -1 in case of failure.
 */
LOCAL int create_test_set_menu ()
{
        DLListItem     *dl_item_tc = INITPTR;
        test_case_s    *tc = INITPTR;
        int             n = 0;
        int             i = 0;

        /* free memory allocated for callback structure */
        free_cbs (cb_test_set_menu);

        /* count the number of test cases in test set */
        n = dl_list_size (test_set);

        if (n > 0) {
                /* allocate memory for n+1+7 items */
                cb_test_set_menu =
                    (callback_s *) calloc (n + 1 + 6, sizeof (callback_s));

                if (cb_test_set_menu == NULL)
                        return -1;

                set_cbs (&cb_test_set_menu[i],
                         "Start sequential test set execution",
                         NULL,
                         start_test_set_sequentially,
                         test_set_main_menu, NULL, NULL, 0);
                i++;
                set_cbs (&cb_test_set_menu[i],
                         "Start parallel test set execution",
                         NULL,
                         start_test_set_parallel,
                         test_set_main_menu, NULL, NULL, 0);
                i++;
                set_cbs (&cb_test_set_menu[i],
                         "Remove test set",
                         NULL,
                         test_set_menu,
                         test_set_main_menu, remove_test_set, NULL, 0);
                i++;
                set_cbs (&cb_test_set_menu[i],
                         "Save test set",
                         NULL,
                         save_test_set, test_set_main_menu, NULL, NULL, 0);
                i++;
                set_cbs (&cb_test_set_menu[i],
                         "Add test case to test set",
                         NULL,
                         add_tcs_to_test_set_menu,
                         test_set_main_menu, NULL, NULL, 0);
                i++;
                set_cbs (&cb_test_set_menu[i],
                         "Remove test case from test set",
                         NULL,
                         remove_tcs_from_test_set_menu,
                         test_set_main_menu, NULL, NULL, 0);
                i++;

                /* get head of test set linked list including test cases */
                dl_item_tc = dl_list_head (test_set);

                while (dl_item_tc != INITPTR) {
                        /* get test_case_s from iterator */
                        tc = (test_case_s *) dl_list_data (dl_item_tc);

                        if (tc != INITPTR && tc->title_ != NULL) {
                                /* fill callback structure */
                                set_cbs (&cb_test_set_menu[i],
                                         tc->title_,
                                         NULL, NULL, test_set_main_menu, NULL,
                                         NULL, 1);

                                i++;
                        }

                        /* get next test case */
                        dl_item_tc = dl_list_next (dl_item_tc);
                }
        } else {
                /* allocate memory for empty menu */
                cb_test_set_menu =
                    (callback_s *) calloc (1 + 6, sizeof (callback_s));

                if (cb_test_set_menu == NULL)
                        return -1;

                i = 0;
                set_cbs (&cb_test_set_menu[i],
                         "Start sequential test set execution",
                         NULL,
                         start_test_set_sequentially,
                         test_set_main_menu, NULL, NULL, 0);
                i++;
                set_cbs (&cb_test_set_menu[i],
                         "Start parallel test set execution",
                         NULL,
                         start_test_set_parallel,
                         test_set_main_menu, NULL, NULL, 0);
                i++;
                set_cbs (&cb_test_set_menu[i],
                         "Remove test set",
                         NULL,
                         test_set_menu,
                         test_set_main_menu, remove_test_set, NULL, 0);
                i++;
                set_cbs (&cb_test_set_menu[i],
                         "Save test set",
                         NULL,
                         save_test_set, test_set_main_menu, NULL, NULL, 0);
                i++;
                set_cbs (&cb_test_set_menu[i],
                         "Add test case to test set",
                         NULL,
                         add_tcs_to_test_set_menu,
                         test_set_main_menu, NULL, NULL, 0);
                i++;
                set_cbs (&cb_test_set_menu[i],
                         "Remove test case from test set",
                         NULL,
                         remove_tcs_from_test_set_menu,
                         test_set_main_menu, NULL, NULL, 0);
                i++;
        }

        /* last menu item should be NULL one */
        null_cbs (&cb_test_set_menu[i]);

        return 0;
}

/* ------------------------------------------------------------------------- */
/** Starts test set sequentially
 */
LOCAL void start_test_set_sequentially ()
{
        //if (ec_run_set_seq (test_set) == 0)
                popup_window ("Start sequential test set execution", 1);
}

/* ------------------------------------------------------------------------- */
/** Starts test set parallel
 */
LOCAL void start_test_set_parallel ()
{
        //if (ec_run_set_par (test_set) == 0)
                popup_window ("Start parallel test set execution", 1);
}

/* ------------------------------------------------------------------------- */
/** Removes test set
 */
LOCAL void remove_test_set (void *p)
{
        empty_test_set ();
}

/* ------------------------------------------------------------------------- */
/** Empties test set
 */
LOCAL void empty_test_set ()
{
        DLListItem     *dl_item = INITPTR;

        /* remove all test cases from test set */
        if (dl_list_size (test_set) != -1) {
                dl_item = dl_list_head (test_set);
                while (dl_item != INITPTR) {
                        dl_list_remove_it (dl_item);
                        dl_item = dl_list_head (test_set);
                }
        }
}

/* ------------------------------------------------------------------------- */
/** Saves test set
 */
LOCAL void save_test_set ()
{
        char           *filename = INITPTR;
        char           *str = "Save test set: ";
        char           *string = INITPTR;
        int             length = 0;

        //filename = create_path ();
        // ec_set_write_file (test_set, filename);

        length = strlen (str) + strlen (filename) + 1;
        string = NEW2 (char, length);
        strcpy (string, str);
        strcat (string, filename);

        popup_window (string, -1);
        DELETE (string);
}

/* ------------------------------------------------------------------------- */
/** Loads test set
 */
LOCAL void load_test_set (void *p)
{
        char           *filename = INITPTR;

        empty_test_set ();

        filename = (char *)dl_list_data ((DLListIterator) p);

        // ec_set_read (test_set, filename);
}

/* ------------------------------------------------------------------------- */
/** Shows add test cases to test set menu
 */
LOCAL void add_tcs_to_test_set_menu ()
{
        if (add_tcs_to_test_set () != -1)
                /* Show new menu */
                update_menu (cb_add_tcs_to_test_set_menu,
                             "Add test cases to test set menu", 0,
                             &add_tcs_to_test_set_menu_focus);
}

/* ------------------------------------------------------------------------- */
/** Creates add test cases to test set menu
 *  @return 0 in case of success, returns -1 in case of failure.
 */
LOCAL int add_tcs_to_test_set ()
{
        DLListItem     *dl_item_tm = INITPTR;
        DLListItem     *dl_item_tc = INITPTR;
        DLList         *dl_list_tc = INITPTR;
        test_case_s    *tc = INITPTR;
        int             n = 0;
        int             i = 0;

        /* free memory allocated for callback structure */
        free_cbs (cb_add_tcs_to_test_set_menu);

        /* count the number of all test cases */
        if (available_modules != INITPTR && available_modules != NULL) {
                /* get head of linked list including available modules */
                dl_item_tm = dl_list_head (available_modules);

                while (dl_item_tm != INITPTR) {
                        /* get test case list of current test module */
                        dl_list_tc = tm_get_tclist (dl_item_tm);

                        if (dl_list_tc != NULL && dl_list_tc != INITPTR) {
                                i = dl_list_size (dl_list_tc);
                                if (i != -1)
                                        n += i;
                        }
                        /* get next module */
                        dl_item_tm = dl_list_next (dl_item_tm);
                }
        }
        i = 0;

        if (n > 0) {
                /* allocate memory for n+1+2 items */
                cb_add_tcs_to_test_set_menu =
                    (callback_s *) calloc (n + 1 + 2, sizeof (callback_s));

                if (cb_add_tcs_to_test_set_menu == NULL)
                        return -1;

                set_cbs (&cb_add_tcs_to_test_set_menu[i],
                         "Add selected cases",
                         NULL,
                         test_set_menu,
                         test_set_menu, add_cases_to_test_set, NULL, 0);
                i++;
                set_cbs (&cb_add_tcs_to_test_set_menu[i],
                         "Change all",
                         NULL,
                         invert_add_tcs_selection,
                         test_set_menu, NULL, NULL, 0);
                i++;

                /* process linked list including available modules */
                for (dl_item_tm = dl_list_head (available_modules);
                     dl_item_tm != INITPTR;
                     dl_item_tm = dl_list_next (dl_item_tm)) {
                        /* get test case list of current test module */
                        dl_list_tc = tm_get_tclist (dl_item_tm);

                        if (dl_list_tc == INITPTR || dl_list_tc == NULL)
                                continue;
                        /* get head of test cases linked list */
                        for (dl_item_tc = dl_list_head (dl_list_tc);
                             dl_item_tc != INITPTR;
                             dl_item_tc = dl_list_next (dl_item_tc)) {
                                /* get test_case_s from iterator */
                                tc = (test_case_s *)
                                    dl_list_data (dl_item_tc);

                                if (tc == INITPTR || tc->title_ == NULL)
                                        continue;
                                /* fill callback structure */
                                set_cbs (&cb_add_tcs_to_test_set_menu[i],
                                         tc->title_, NULL,
                                         toggle_menu_item,
                                         test_set_menu, NULL, tc, 1);
                                i++;
                        }
                }
        } else {
                /* allocate memory for empty menu */
                cb_add_tcs_to_test_set_menu =
                    (callback_s *) calloc (1 + 2, sizeof (callback_s));

                if (cb_add_tcs_to_test_set_menu == NULL)
                        return -1;

                i = 0;
                set_cbs (&cb_add_tcs_to_test_set_menu[i],
                         "Add selected cases",
                         NULL, test_set_menu, test_set_menu, NULL, NULL, 0);
                i++;
                set_cbs (&cb_add_tcs_to_test_set_menu[i],
                         "Change all",
                         NULL,
                         invert_add_tcs_selection,
                         test_set_menu, NULL, NULL, 0);
                i++;
        }

        /* last menu item should be NULL one */
        null_cbs (&cb_add_tcs_to_test_set_menu[i]);

        return 0;
}

/* ------------------------------------------------------------------------- */
/** Inverts add test cases to test set selection
 */
LOCAL void invert_add_tcs_selection ()
{
        int             i = 0;
        ITEM          **items = INITPTR;

        items = menu_items (my_menu);

        for (i = 2; i < item_count (my_menu); ++i)
                set_item_value (items[i], !item_value (items[i]));
}

/* ------------------------------------------------------------------------- */
/** Shows remove test cases from test set menu
 */
LOCAL void remove_tcs_from_test_set_menu ()
{
        if (remove_tcs_from_test_set () != -1)
                /* Show new menu */
                update_menu (cb_remove_tcs_from_test_set_menu,
                             "Remove test cases from test set menu", 0,
                             &remove_tcs_from_test_set_menu_focus);
}

/* ------------------------------------------------------------------------- */
/** Creates remove test cases from test set menu structure
 *  @return 0 in case of success, returns -1 in case of failure.
 */
LOCAL int remove_tcs_from_test_set ()
{
        DLListItem     *dl_item_tc = INITPTR;
        test_case_s    *tc = INITPTR;
        int             n = 0;
        int             i = 0;

        /* free memory allocated for callback structure */
        free_cbs (cb_remove_tcs_from_test_set_menu);

        /* number of test cases in test set */
        n = dl_list_size (test_set);

        if (n > 0) {
                /* allocate memory for n+1+2 items */
                cb_remove_tcs_from_test_set_menu =
                    (callback_s *) calloc (n + 1 + 2, sizeof (callback_s));

                if (cb_remove_tcs_from_test_set_menu == NULL)
                        return -1;

                set_cbs (&cb_remove_tcs_from_test_set_menu[i],
                         "Remove selected cases",
                         NULL,
                         test_set_menu,
                         test_set_menu, remove_cases_from_test_set, NULL, 0);
                i++;
                set_cbs (&cb_remove_tcs_from_test_set_menu[i],
                         "Change all",
                         NULL,
                         invert_add_tcs_selection,
                         test_set_menu, NULL, NULL, 0);
                i++;

                /* get head of linked list including test cases in test set */
                dl_item_tc = dl_list_head (test_set);

                while (dl_item_tc != INITPTR) {
                        /* get test_case_s from iterator */
                        tc = (test_case_s *) dl_list_data (dl_item_tc);

                        if (tc != INITPTR && tc->title_ != NULL) {
                                /* fill callback structure */
                                set_cbs (&cb_remove_tcs_from_test_set_menu[i],
                                         tc->title_,
                                         NULL,
                                         toggle_menu_item,
                                         test_set_menu, NULL, tc, 1);

                                i++;
                        }
                        /* get next test case */
                        dl_item_tc = dl_list_next (dl_item_tc);
                }
        } else {
                /* allocate memory for empty menu */
                cb_remove_tcs_from_test_set_menu =
                    (callback_s *) calloc (1 + 2, sizeof (callback_s));

                if (cb_remove_tcs_from_test_set_menu == NULL)
                        return -1;

                i = 0;
                set_cbs (&cb_remove_tcs_from_test_set_menu[i],
                         "Remove selected cases",
                         NULL, test_set_menu, test_set_menu, NULL, NULL, 0);
                i++;
                set_cbs (&cb_remove_tcs_from_test_set_menu[i],
                         "Change all",
                         NULL,
                         invert_add_tcs_selection,
                         test_set_menu, NULL, NULL, 0);
                i++;
        }

        /* last menu item should be NULL one */
        null_cbs (&cb_remove_tcs_from_test_set_menu[i]);

        return 0;
}

/* ------------------------------------------------------------------------- */
/** Shows load test set menu
 */
LOCAL void load_test_set_list (void)
{
        if (get_test_sets () != -1)
                /* Show new menu */
                update_menu (cb_load_test_set_menu, "Load test set menu", 0, 
                             &load_test_set_menu_focus);

}

/* ------------------------------------------------------------------------- */
/** Get test set files in specific directory and creates load test set menu
 *  @return 0 in case of success, returns -1 in case of failure.
 */
LOCAL int get_test_sets (void)
{
        DLListItem     *dl_item = INITPTR;
        char           *test_set = INITPTR;
        struct dirent **namelist;
        int             n = 0;
        int             i = 0;
        char           *path;
        char           *c_dir;

        /* free memory allocated for callback structure */
        free_cbs (cb_load_test_set_menu);

        /* remove items from current test sets list */
        if (dl_list_size (test_set_files) != -1) {
                dl_item = dl_list_head (test_set_files);
                while (dl_item != INITPTR) {
                        free ((char *)dl_list_data (dl_item));
                        dl_list_remove_it (dl_item);
                        dl_item = dl_list_head (test_set_files);
                }
        }

        c_dir = getenv ("HOME");
        path = NEW2 (char, strlen (c_dir) + 8);
        sprintf (path, "%s", c_dir);
        strcat (path, "/.min/");

        /* return the number of directory entries */
        n = scandir (path, &namelist, 0, alphasort);

        DELETE (path);

        /* add .set files to test sets list */
        while (n--) {
                if (strstr (namelist[n]->d_name, ".set")) {
                        test_set = (char *)
                            malloc (sizeof (char) *
                                    (strlen (namelist[n]->d_name) + 1));
                        strcpy (test_set, namelist[n]->d_name);
                        dl_list_add (test_set_files, (void *)test_set);
                        i++;
                }
                free (namelist[n]);
        }
        free (namelist);

        n = i;
        i = 0;

        if (n > 0) {
                /* allocate memory for n+1 items */
                cb_load_test_set_menu =
                    (callback_s *) calloc (n + 1, sizeof (callback_s));

                if (cb_load_test_set_menu == NULL)
                        return -1;

                /* get head of test sets list */
                dl_item = dl_list_head (test_set_files);

                while (dl_item != INITPTR) {
                        set_cbs (&cb_load_test_set_menu[i],
                                 (char *)dl_list_data (dl_item),
                                 NULL,
                                 test_set_menu,
                                 test_set_main_menu,
                                 load_test_set, dl_item, 0);
                        i++;

                        /* next item in list */
                        dl_item = dl_list_next (dl_item);
                }
        } else {
                /* allocate memory for empty menu */
                cb_load_test_set_menu =
                    (callback_s *) calloc (1 + 1, sizeof (callback_s));

                if (cb_load_test_set_menu == NULL)
                        return -1;

                set_cbs (&cb_load_test_set_menu[i],
                         "", NULL, NULL, test_set_main_menu, NULL, NULL, 0);
                i++;
        }

        /* last menu item should be NULL one */
        null_cbs (&cb_load_test_set_menu[i]);

        return 0;
}

/* ------------------------------------------------------------------------- */
/** Adds user selected test cases to test set list
 */
LOCAL void add_cases_to_test_set (void *p)
{
        int             i = 0;
        ITEM          **items = NULL;
        callback_s     *cb = INITPTR;

        items = menu_items (my_menu);

        /* iterate through menu items to find highlighted ones */
        for (i = 2; i < item_count (my_menu); ++i) {
                /* test if menu item is highlighted */
                if (item_value (items[i]) == TRUE) {
                        /* get user data attached to menu item */
                        cb = (callback_s *) item_userptr (items[i]);
                        /* add menu item (test case) to linked list */
                        dl_list_add (test_set, cb->ptr_data);
                }
        }
}

/* ------------------------------------------------------------------------- */
/** Removes user selected test cases from test set list
 */
LOCAL void remove_cases_from_test_set (void *p)
{
        int             i = 0;
        ITEM          **items = NULL;
        callback_s     *cb = INITPTR;
        test_case_s    *tc = INITPTR;

        items = menu_items (my_menu);

        /* iterate through menu items to find highlighted ones */
        for (i = 2; i < item_count (my_menu); ++i) {
                /* test if menu item is highlighted */
                if (item_value (items[i]) == TRUE) {
                        /* get user data attached to menu item */
                        cb = (callback_s *) item_userptr (items[i]);
                        tc = (test_case_s *) cb->ptr_data;
                        /* find selected 
                         * test case in test set list and remove it */
                        dl_list_remove_it (dl_list_find
                                           (dl_list_head (test_set),
                                            dl_list_tail (test_set),
                                            compare_items, tc)
                            );
                }
        }
}

/* ------------------------------------------------------------------------- */
/** Local comparison function for same test case searching.
    This function is used with DLLIST function dl_list_find().
 *  @param data1 is data from the list
 *  @param data2 value provided by the user.
 *  @return 0 when elements are equal, 1 when first is greater than second,
 *          -1 otherwise.
*/
LOCAL int compare_items (const void *data1, const void *data2)
{
        int             result = -2;
        test_case_s    *tc1 = INITPTR;
        test_case_s    *tc2 = INITPTR;

        if ((data1 != INITPTR) && (data2 != INITPTR)) {
                tc1 = (test_case_s *) data1;
                tc2 = (test_case_s *) data2;

                if (data1 > data2)
                        result = 1;
                else if (data1 == data2)
                        result = 0;
                else
                        result = -1;
        }
        return result;
}

/* ======================== FUNCTIONS ====================================== */
/* None */

/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#if 0
def             MIN_UNIT_TEST
#include "check.h"
/* ------------------------------------------------------------------------- */
int template_tests ()
{
        int             number_failed;

        number_failed = 0;

        return number_failed;
}

/* ------------------------------------------------------------------------- */

#endif                          /* MIN_UNIT_TEST */

/* End of file */
