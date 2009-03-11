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
#include <pthread.h>            /* for mutex */

#include <callback.h>
#include <consoleui.h>
#include <dllist.h>
#include <dirent.h>
#include <data_api.h>
#include <min_common.h>
#include <min_parser.h>
#include <min_plugin_interface.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
extern bool     continue_;      /* flag indicating whether to continue prog  */
extern MENU    *my_menu;        /* menu itself */
extern WINDOW  *main_window;    /* main window */
extern WINDOW  *menu_window;    /* window including menu */
extern WINDOW  *log_window;     /* log window */

extern eapiIn_t out_clbk_;      /*  */
extern eapiOut_t min_clbk_;     /*  */
extern DLList *case_list_;
extern DLList *executed_case_list_;      /* */
extern DLList *available_modules;
extern DLList *found_tcase_files;
extern DLList *error_list_;

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
LOCAL void      log_menu (void);
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
LOCAL char     *create_path ();
/* ------------------------------------------------------------------------- */
LOCAL void      flush_log (void *p);
/* ------------------------------------------------------------------------- */
LOCAL void      side_scroll_log_item (void *p);
/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
/** main menu structure */
callback_s      cb_main_menu[] = {
        {"Case menu", NULL, case_menu, NULL, NULL, NULL},
        {"Module menu", NULL, module_menu, NULL, NULL, NULL},
        {"Test set menu", NULL, test_set_main_menu, NULL, NULL, NULL},
	{"Log menu", NULL, log_menu, NULL, NULL, NULL},
        {"Exit", NULL, quit_program, NULL, NULL, NULL},
        {NULL, NULL, NULL, NULL, NULL, NULL}
};

DLList         *selected_cases;

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

/* log menu callback structure */
LOCAL callback_s *cb_log_menu = INITPTR;

/* test case title */
LOCAL char      title[MaxTestCaseName] = { '\0' };

/* temporary module name */
LOCAL char      module_name[MaxFileName + 1];

/* focus position information for different menus */
LOCAL focus_pos_s       case_menu_focus = { 0,0 };
LOCAL focus_pos_s       log_menu_focus = { 0,0 };
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

LOCAL unsigned groupid = 0;

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
LOCAL int       get_log_messages (void);
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
/** Shows log menu
 */
LOCAL void log_menu ()
{
        if (get_log_messages () != -1)
                /* Show new menu */
		update_menu (cb_log_menu, "Log menu", 1, &log_menu_focus);
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
	callback_s *p = cb;

	while (p != INITPTR && p && p->choice != NULL) {
		free (p->choice);
		p++;
	}

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
        char *text = INITPTR;
        text = (char *)p;
        STRCPY (module_name, text, MaxFileName);
        /* get rid of .so extension */
        module_name[strlen (module_name) - 3] = '\0';
        free(p);
}

/* ------------------------------------------------------------------------- */
/** Adds test module to test framework
 */
LOCAL void add_module (void *p)
{
        int             i = 0;
        ITEM          **items = NULL;
        callback_s     *cb = INITPTR;

        found_tcase_files = dl_list_create();

        items = menu_items (my_menu);

        /* iterate through menu items to find highlighted ones */
        for (i = 1; i < item_count (my_menu); ++i) {
                /* test if menu item is highlighted */
                if (item_value (items[i]) == TRUE) {
                        /* get user data attached to menu item */
                        cb = (callback_s *) item_userptr (items[i]);
                        /* add test case file to linked list */
                        dl_list_add (found_tcase_files, cb->ptr_data);
                }
        }

        min_clbk_.add_test_module (module_name);
}

/* ------------------------------------------------------------------------- */
/** Get test modules in specific directory and creates add test module menu
 *  @return 0 in case of success, returns -1 in case of failure.
 */
LOCAL int get_test_modules (void)
{
        char *modules = INITPTR;
        char *c = INITPTR, *p;
        int i = 0;
        int howmany = 0;
        Text *tx = INITPTR;

        /* free memory allocated for callback structure */
        free_cbs (cb_add_test_module_menu);

        min_clbk_.query_test_modules(&modules);

        if (modules!=INITPTR) {

                /* Count number of items */
                c = &modules[0];
                while (*c != '\0') {
                        howmany++;
                        c += strlen(c);
                        c++;
                }
        }

        if (howmany>0) {
                /* Allocate memory for menu */
                cb_add_test_module_menu = NEW2(callback_s,howmany+1);
                if (cb_add_test_module_menu==NULL) return -1;
                memset(cb_add_test_module_menu,0x0,
                        sizeof(callback_s)*(howmany+1));
                i = 0;
                howmany = 0;
                /* Create menu */
                c = &modules[i];
                while (*(c+1) != '\0') {
                        if ((*c)=='\0') c++;
                        i = strlen(c);
                        tx = tx_create(c);
			p = tx_get_buf (tx);
                        set_cbs (&cb_add_test_module_menu[howmany],
				 p,NULL,add_test_case_files_menu,
				 module_menu, save_module_name, p, 0);
                        tx_destroy(&tx);
                        c+=(i);
                        howmany++;
                }
		DELETE (modules);
        } else {
                /* Allocate memory for empty menu */
                cb_add_test_module_menu = NEW2(callback_s,2);
                if (cb_add_test_module_menu==NULL) return -1;
                memset(cb_add_test_module_menu,0x0,sizeof(callback_s)*2);

                set_cbs (&cb_add_test_module_menu[howmany],
                        "",NULL,add_test_case_files_menu,
                        module_menu, NULL, NULL, 0);
                howmany++;
        }

        /* last menu item should be NULL one */
        null_cbs (&cb_add_test_module_menu[howmany]);

        return 0;
}

/* ------------------------------------------------------------------------- */
/** Load log messages into callback structure
 *  @return 0 in case of success, returns -1 in case of failure.
 */
LOCAL int get_log_messages (void)
{
        char *p;
	DLListIterator it;
        int howmany = 0;
        Text *tx = INITPTR;
	

        /* free memory allocated for callback structure */
        free_cbs (cb_log_menu);

	howmany = dl_list_size (error_list_);
        if (howmany > 0) {
                /* Allocate and clear memory for menu */
                cb_log_menu = NEW2 (callback_s, howmany + 2);
                memset (cb_log_menu, 0x0,
                        sizeof (callback_s) * (howmany + 2));
		howmany = 0;
                set_cbs (&cb_log_menu[howmany],
                         "Flush log",
                         NULL,
                         log_menu,
                         main_menu, flush_log, NULL, 0);
                howmany++;
                if (cb_log_menu == NULL) return -1;
                /* Create menu */
		for (it = dl_list_tail (error_list_); it != INITPTR;
		     it = dl_list_prev (it)) {
			tx = dl_list_data (it);
			p = tx_get_buf (tx);
                        set_cbs (&cb_log_menu [howmany],
				 p, NULL, log_menu,
				 main_menu, side_scroll_log_item, it, 1);
                        howmany ++;
                }
        } else {
                /* Allocate memory for empty menu */
                cb_log_menu = NEW2(callback_s,2);
                if (cb_log_menu == NULL) return -1;
                memset (cb_log_menu, 0x0, sizeof (callback_s) * 2);

                set_cbs (&cb_log_menu [howmany],
			 "", NULL, log_menu,
			 main_menu, NULL, NULL, 0);
                howmany++;
        }

        /* last menu item should be NULL one */
        null_cbs (&cb_log_menu [howmany]);

        return 0;
}

/* ------------------------------------------------------------------------- */
/** Get test case files in specific dir and creates add test case file menu
 *  @return 0 in case of success, returns -1 in case of failure.
 */
LOCAL int get_test_case_files (void)
{
        char *files = INITPTR;
        int i = 0;
        int howmany = 0;
        char *c = INITPTR, *p;
        Text *tx = INITPTR;

        /* free memory allocated for callback structure */
        free_cbs (cb_add_test_case_files_menu);

        min_clbk_.query_test_files(&files);

        if (files!=INITPTR) {
                /* Count number of items */
                c = &files[0];
                while (*c != '\0') {
                        howmany++;
                        c += strlen(c);
                        c++;
                }
        }

        if (howmany>0) {
                /* allocate memory for n+1 items */
                cb_add_test_case_files_menu = NEW2(callback_s,howmany+2);
                if (cb_add_test_case_files_menu == NULL) return -1;
                memset(cb_add_test_case_files_menu,0x0,
                        sizeof(callback_s)*(howmany+2));
                i = 0;
                howmany = 0;
                set_cbs (&cb_add_test_case_files_menu[howmany],
                         "Add selected test case file(s)",
                         NULL,
                         module_menu,
                         add_test_module_menu, add_module, NULL, 0);
                howmany++;
                /* Create menu */
                c = &files[i];
                while (*(c+1) != '\0') {
                        if ((*c)=='\0') c++;
                        i = strlen(c);
                        tx = tx_create(c);
			p = tx_get_buf (tx);
                        set_cbs (&cb_add_test_case_files_menu[howmany],
                                 p,NULL,toggle_menu_item,
                                 add_test_module_menu,
                                 NULL, p, 1);
			tx_destroy (&tx);
                        c+=(i);
                        howmany++;
                }
		DELETE (files);
        } else {
                /* Allocate memory for empty menu */
                cb_add_test_case_files_menu = NEW2(callback_s,2);
                if (cb_add_test_case_files_menu==NULL) return -1;
                memset(cb_add_test_case_files_menu,0x0,sizeof(callback_s)*2);

                set_cbs (&cb_add_test_case_files_menu[howmany],
                        "",NULL, NULL, add_test_module_menu,
                        NULL, NULL, 0);
                howmany++;
        }

        /* last menu item should be NULL one */
        null_cbs (&cb_add_test_case_files_menu[howmany]);

        return 0;
}

/* ------------------------------------------------------------------------- */
/** Gets test modules for populating modules menu
 *  @return 0 in case of success, returns -1 in case of failure.
 */
LOCAL int get_loaded_modules ()
{
        DLListItem     *dl_item_tm = INITPTR;
        CUIModuleData  *tmi = INITPTR;
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
		memset (cb_module_menu, 0x0, (n + 2) * sizeof (callback_s));

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
                            (CUIModuleData *) dl_list_data (dl_item_tm);

                        if (tmi != INITPTR && 
			    tx_share_buf (tmi->modulename_) != NULL) {
                                /* fill menu structure with item data */
                                set_cbs (&cb_module_menu[i],
                                         tx_get_buf (tmi->modulename_),
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
        DLListItem     *dl_item_tc = INITPTR;
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
		memset (cb_start_new_case_menu, 0x0, 
			(n+1)*sizeof (callback_s));

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
                                tx_get_buf (tc->casetitle_),
                                NULL,case_menu, case_menu, start_one_tc,
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
        DLListItem     *dl_item_tc = INITPTR;
        CUICaseData    *tc = INITPTR;
        int             n = 0;
        int             i = 0;

        /* free memory allocated for callback structure */
        free_cbs (cb_run_multiple_tests_menu);

        /* count the number of all test cases */
	n = dl_list_size (case_list_);

        if (n > 0) {
                /* allocate memory for n+1 items */
                cb_run_multiple_tests_menu =
                    (callback_s *) calloc (n + 1 + 4, sizeof (callback_s));
		memset (cb_run_multiple_tests_menu, 0x0, (n + 1 + 4) *
			sizeof (callback_s));
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

                /* process linked list of available test cases */
                for (dl_item_tc = dl_list_head (case_list_);
                     dl_item_tc != INITPTR;
                     dl_item_tc = dl_list_next (dl_item_tc)) {
                        /* get test case list of current test module */
                        tc = dl_list_data (dl_item_tc);

			if (tc == INITPTR || tc->casetitle_ == NULL)
				continue;
			/* fill callback structure */
			set_cbs (&cb_run_multiple_tests_menu[i],
				 tx_get_buf (tc->casetitle_), NULL,
				 toggle_menu_item,
				 case_menu, NULL, tc, 1);
			i++;
			
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
		memset (cb_ongoing_cases_menu, 0x0, (n + 1) *
			sizeof (callback_s));
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
                        /* get from linked list iterator */
                        etc = dl_list_data (dl_item_tc);
                        if (etc == INITPTR || etc->case_->casetitle_ == NULL)
                                continue;

                        switch (etc->status_) {

                        case TCASE_STATUS_ONGOING:
                                /* fill callback structure with data */
                                set_cbs (&cb_ongoing_cases_menu [i], 
					 tx_get_buf (etc->case_->casetitle_),
                                         "(ongoing)", NULL,
                                         case_menu,
                                         pause_resume_abort_menu,
                                         dl_item_tc, 0);
                                i++;
                                break;

                        case TCASE_STATUS_PAUSED:
                                /* fill callback structure with data */
                                set_cbs (&cb_ongoing_cases_menu [i], 
					 tx_get_buf (etc->case_->casetitle_),
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
#if 0
/* Not used ? */
LOCAL int _find_case_by_result (const void *a, const void *b)
{
        ExecutedTestCase *tmp1 = (ExecutedTestCase*)a;
        int *tmp2 = (int*)b;

        if (tmp1->result_==(*tmp2)) return 0;
        else return -1;
}
#endif
/* ------------------------------------------------------------------------- */
LOCAL int _find_case_by_status (const void *a, const void *b)
{
        ExecutedTestCase *tmp1 = (ExecutedTestCase*)a;
        unsigned *tmp2 = (unsigned*)b;

        if (tmp1->status_==(*tmp2)) return 0;
        else return -1;
}
/* ------------------------------------------------------------------------- */
LOCAL int _find_mod_by_id (const void *a, const void *b)
{
        CUIModuleData *tmp1 = (CUIModuleData*)a;
        int *tmp2 = (int*)b;

        if (tmp1->moduleid_ ==(*tmp2)) return 0;
        else return -1;
}
/* ------------------------------------------------------------------------- */
LOCAL int _find_mod_by_name (const void *a, const void *b)
{
        CUIModuleData *tmp1 = (CUIModuleData*)a;

	return strcmp (tx_share_buf (tmp1->modulename_), (const char *)b); 
}


/* ------------------------------------------------------------------------- */
/** Gets specific test results
 *  @param cb pointer to menu callback structure
 *  @param result_type test result type to fetch
 *  @return 0 in case of success, returns -1 in case of failure.
 */
LOCAL int get_cases_by_result_type (callback_s ** cb, int result_type)
{
        DLListIterator it = DLListNULLIterator;
        DLListIterator begin = dl_list_head (executed_case_list_);
        DLList *passed_cases = dl_list_create(); /* for passed cases */
        DLList *failed_cases = dl_list_create(); /* for failed cases */
        DLList *abocra_cases = dl_list_create(); /* for aborted/crashed cases */
        int n = 0;
        int i = 0;
        ExecutedTestCase *etc = INITPTR;
        unsigned result_to_find = TCASE_STATUS_FINNISHED;

        /* free memory allocated for callback structure */
        free_cbs (*cb);

        /* The UI currently shows cases that are: Executed, Passed, Failed,
         * Aborted/Crashed. First of all we check status to find Finished
         * cases and then we narrow the results according to filter. In order
         * not to get lost bewteen IFs and ELSEs test cases will be filtered
         * and links to them will be grouped in lists.
         * There will be three lists, for: Passed, Failed and Aborted/Crashed
         * cases (they will contain pointers to iterators from
         * executed_case_list_) and final view will combine them.
         * For future this sorting can be done in result reporting handler,
         * but this would require lists to be global in scope of CUI.
         */
        
        /* 1. Find finished cases and group them on lists. */
        begin = dl_list_head(executed_case_list_);
        while (begin!=DLListNULLIterator) {
                it = dl_list_find (begin,
                                dl_list_tail (executed_case_list_),
                                _find_case_by_status,
                                &result_to_find);
                if (it==DLListNULLIterator) break;
                etc = (ExecutedTestCase*)dl_list_data(it);

                /* Values map 100% to those set by TMC, see min_common for
                 * reference. Passed=0, Failed=1, Crashed=-2,Not Completed=2
                 * Timeouted=-1
                 */
                if (etc->result_ == 0) {
                        /* passed */
                        dl_list_add(passed_cases,(void*)it);
                } else if (etc->result_ == 1) {
                        /* failed */
                        dl_list_add(failed_cases,(void*)it);
                } else if (etc->result_ == -2 || etc->result_ == 2) {
                        /* aborted/crashed: -2 = crashed, 2 = NC */
                        dl_list_add(abocra_cases,(void*)it);
                } else {
                        MIN_ERROR ("Case of result %d has not beed filtered",
                                etc->result_);
                }

                begin = dl_list_next (it);
        }

        /* 2. Guess what user want to see and count items to be presented. */
        if (result_type==TEST_RESULT_ALL) {
                /* User want to see all test cases */
                n = dl_list_size(passed_cases);
                n += dl_list_size(failed_cases);
                n += dl_list_size(abocra_cases);
        } else if (result_type==TEST_RESULT_FAILED) {
                /* User want to see failed test cases */
                n = dl_list_size(failed_cases);
        } else if (result_type==TEST_RESULT_PASSED) {
                /* User want to see passed test cases */
                n = dl_list_size(passed_cases);
        } else if (result_type==TEST_RESULT_CRASHED ||
                        result_type==TEST_RESULT_ABORTED) {
                /* User want to see aborted/crashed test cases */
                n = dl_list_size(abocra_cases);
        } else {
                n = 0;
                MIN_ERROR ("Not handled view option %d",result_type);
        }
        if (n==0) goto empty_menu;

        /* 3. Construct the view. */        
        /* 3.1 Allocate memory for menu items. */
        (*cb) = NEW2(callback_s,n+1);
        if (!(*cb)) {
                /* clean up */
                dl_list_free(&passed_cases);
                dl_list_free(&failed_cases);
                dl_list_free(&abocra_cases);
                return -1;
        }
        memset (*cb,0x0,sizeof(callback_s)*(n+1));
        
        /* 3.2 Create items to be displayed. */
        if (result_type==TEST_RESULT_ALL) {
                /* User want to see all test cases */
                it = dl_list_head (failed_cases);
                while (it!=DLListNULLIterator) {
                        begin = (DLListIterator)dl_list_data(it);
                        etc = (ExecutedTestCase*)dl_list_data(begin);
                        set_cbs (&(*cb)[i],
                                tx_get_buf(etc->case_->casetitle_),
                                NULL,
                                NULL,
                                case_menu,
                                tr_for_executed_case, etc, 0);
                        i++;
                        it = dl_list_next(it);
                }
                it = dl_list_head (passed_cases);
                while (it!=DLListNULLIterator) {
                        begin = (DLListIterator)dl_list_data(it);
                        etc = (ExecutedTestCase*)dl_list_data(begin);
                        set_cbs (&(*cb)[i],
                                tx_get_buf(etc->case_->casetitle_),
                                NULL,
                                NULL,
                                case_menu,
                                tr_for_executed_case, etc, 0);
                        i++;
                        it = dl_list_next(it);
                }
                it = dl_list_head (abocra_cases);
                while (it!=DLListNULLIterator) {
                        begin = (DLListIterator)dl_list_data(it);
                        etc = (ExecutedTestCase*)dl_list_data(begin);
                        set_cbs (&(*cb)[i],
                                tx_get_buf(etc->case_->casetitle_),
                                NULL,
                                NULL,
                                case_menu,
                                tr_for_executed_case, etc, 0);
                        i++;
                        it = dl_list_next(it);
                }
        } else if (result_type==TEST_RESULT_FAILED) {
                /* User want to see failed test cases */
                it = dl_list_head (failed_cases);
                while (it!=DLListNULLIterator) {
                        begin = (DLListIterator)dl_list_data(it);
                        etc = (ExecutedTestCase*)dl_list_data(begin);
                        set_cbs (&(*cb)[i],
                                tx_get_buf(etc->case_->casetitle_),
                                NULL,
                                NULL,
                                case_menu,
                                tr_for_failed_case, etc, 0);
                        i++;
                        it = dl_list_next(it);
                }
        } else if (result_type==TEST_RESULT_PASSED) {
                /* User want to see passed test cases */
                it = dl_list_head (passed_cases);
                while (it!=DLListNULLIterator) {
                        begin = (DLListIterator)dl_list_data(it);
                        etc = (ExecutedTestCase*)dl_list_data(begin);
                        set_cbs (&(*cb)[i],
                                tx_get_buf(etc->case_->casetitle_),
                                NULL,
                                NULL,
                                case_menu,
                                tr_for_passed_case, etc, 0);
                        i++;
                        it = dl_list_next(it);
                }
        } else if (result_type==TEST_RESULT_CRASHED ||
                        result_type==TEST_RESULT_ABORTED) {
                /* User want to see aborted/crashed test cases */
                it = dl_list_head (abocra_cases);
                while (it!=DLListNULLIterator) {
                        begin = (DLListIterator)dl_list_data(it);
                        etc = (ExecutedTestCase*)dl_list_data(begin);
                        set_cbs (&(*cb)[i],
                                tx_get_buf(etc->case_->casetitle_),
                                NULL,
                                NULL,
                                case_menu,
                                tr_for_aborted_case, etc, 0);
                        i++;
                        it = dl_list_next(it);
                }
        } else {
                MIN_ERROR ("Not handled view option %d",result_type);
        }

        /* last menu item should be NULL one */
        null_cbs (&(*cb)[i]);

        /* clean up */
        dl_list_free(&passed_cases);
        dl_list_free(&failed_cases);
        dl_list_free(&abocra_cases);
        return 0;

      empty_menu:
        /* allocate memory for empty menu */
        *cb = NEW2(callback_s,2);
        if (*cb==NULL) return -1;
        memset(*cb,0x0,sizeof(callback_s)*2);

        set_cbs (&(*cb)[0], "", NULL, NULL, case_menu, NULL, NULL, 0);

        /* last menu item should be NULL one */
        null_cbs (&(*cb)[1]);

        /* clean up */
        dl_list_free(&passed_cases);
        dl_list_free(&failed_cases);
        dl_list_free(&abocra_cases);

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
        static void    *s_p = INITPTR;
        static ptr_to_fun s_on_left = INITPTR;

        ExecutedTestCase *etc = INITPTR;
        CUICaseData *ccd = INITPTR;

        /* some asserts */
        if (p != INITPTR) s_p = p;
        if (on_left != INITPTR) s_on_left = on_left;

        /* check pointers */
        if (s_p != INITPTR) {
                etc = (ExecutedTestCase*)s_p;
                if (etc!=INITPTR && etc->case_!=INITPTR) {
                        ccd = etc->case_;
                } else return -1;
        } else return -1;

        /* free memory allocated for callback structure */
        free_cbs (cb_test_result_menu);

        /* allocate memory for menu callback structure */
        cb_test_result_menu = NEW2(callback_s,2);
        if (!cb_test_result_menu) return -1;
        memset (cb_test_result_menu,0x0,2*sizeof(callback_s));

        /* set the view items */
        set_cbs (&cb_test_result_menu[0],
                 "View output",
                 NULL, NULL, s_on_left, view_output, s_p, 0);

        /* last menu item should be NULL one */
        null_cbs (&cb_test_result_menu[1]);

        /* add title to menu */
        if (ccd->casetitle_!=INITPTR) {
                update_menu (cb_test_result_menu,
			     tx_get_buf(ccd->casetitle_),
			     1,
			     NULL);
        } else update_menu (cb_test_result_menu, "", 1, NULL);

        /* show test results */
        test_result_view (s_p);

        return 0;
}

/* ------------------------------------------------------------------------- */
/** Shows test results on window
 *  @param p pointer to test result
 */
LOCAL void test_result_view (void *p)
{
        struct tm      *timeinfo = INITPTR;
        char            result[20];
        char            buffer_start_time[20];
        char            buffer_end_time[20];
        ExecutedTestCase *etc = INITPTR;

        memset(buffer_start_time,0x0,20);
        memset(buffer_end_time,0x0,20);

        if (p != INITPTR) {
                etc = (ExecutedTestCase*) p;

                if (etc != INITPTR) {
                        switch (etc->result_) {
                        case TP_PASSED:
                                strcpy (result, "Passed");
                                break;
                        case TP_FAILED:
                                strcpy (result, "Failed");
                                break;
                        case TP_CRASHED:
                                strcpy (result, "Crashed");
                                break;
                        case TP_NC:
                                strcpy (result, "Aborted");
                                break;
                        case TP_TIMEOUTED:
                                strcpy (result, "Timeout");
                                break;
                        default:
                                strcpy (result, "Unknown");
                                break;
                        }

                        /* change times to more readible form */
                        timeinfo = localtime ((time_t *) & (etc->starttime_));

                        strftime (buffer_start_time, 20, "%I:%M:%S %p",
                                  timeinfo);

                        if (etc->endtime_ != 0) {
                                timeinfo = localtime ((time_t *)
                                                      & (etc->endtime_));
                                strftime (buffer_end_time, 20,
                                          "%I:%M:%S %p", timeinfo);
                        } else {
                                strcpy (buffer_end_time, "");
                        }

                        /* print test results to screen */
                        mvwprintw (menu_window, 2, 0,
                                   "Result info: %s", result);
                        mvwprintw (menu_window, 3, 0,
                                   "Result descr: %s",
                                   tx_share_buf (etc->resultdesc_));
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
        ExecutedTestCase *tc = INITPTR;
        int             quit = 0;
        static void    *s_p = INITPTR;

        if (p != INITPTR)
                s_p = p;

        /* check pointers */
        if (s_p != INITPTR) {
                dl_item_tc = (DLListItem *) s_p;
                if (dl_item_tc != INITPTR && dl_item_tc != NULL) {
                        tc = (ExecutedTestCase *) dl_list_data (dl_item_tc);
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

                        switch (tc->status_) {
                        case TCASE_STATUS_ONGOING:
                                set_cbs (&cb_pause_resume_abort_menu[1],
                                         "Pause",
                                         NULL,
                                         ongoing_cases_menu,
                                         ongoing_cases_menu, pause_tc, s_p,
                                         0);
                                break;
                        case TCASE_STATUS_PAUSED:
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

                        if (tc->case_->casetitle_ != INITPTR 
			    && tc->case_->casetitle_ != NULL) {
                                strcpy (title, 
					tx_share_buf (tc->case_->casetitle_));
                                /* Show new menu */
                                update_menu (cb_pause_resume_abort_menu,
                                             tx_share_buf 
					     (tc->case_->casetitle_), 1, NULL);
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
        DLListIterator it = (DLListIterator)p;
        ExecutedTestCase *e = (ExecutedTestCase *)dl_list_data(it);

        if (min_clbk_.pause_case) min_clbk_.pause_case(e->runid_);
}

/* ------------------------------------------------------------------------- */
/** Resumes test case execution
 *  @param p pointer to DLListIterator
 */
LOCAL void resume_tc (void *p)
{
        DLListIterator it = (DLListIterator)p;
        ExecutedTestCase *e = (ExecutedTestCase *)dl_list_data(it);

        if (min_clbk_.resume_case) min_clbk_.resume_case(e->runid_);
}

/* ------------------------------------------------------------------------- */
/** Aborts tes case execution
 *  @param p pointer to DLListIterator
 */
LOCAL void abort_tc (void *p)
{
        DLListIterator it = (DLListIterator)p;
        ExecutedTestCase *e = (ExecutedTestCase *)dl_list_data(it);

        if (min_clbk_.abort_case) min_clbk_.abort_case(e->runid_);

}

/* ------------------------------------------------------------------------- */
/** Starts executing of one test case
 */
LOCAL void start_one_tc (void *p)
{
        DLListIterator it = (DLListIterator)p;
        CUICaseData *c = (CUICaseData*)dl_list_data(it);
        if (min_clbk_.start_case) min_clbk_.start_case(c->moduleid_,
						       c->caseid_,
						       0);
}

/* ------------------------------------------------------------------------- */
/** Starts executing test cases sequentially
 */
LOCAL void start_cases_sequentially (void *p)
{
	CUICaseData *c;
	DLListIterator it;
	
	groupid ++;
        get_selected_cases ();
	for (it = dl_list_head (user_selected_cases); it != INITPTR;
	     it = dl_list_next (it)) {
		c  = (CUICaseData*)dl_list_data(it);
		if (min_clbk_.start_case) min_clbk_.start_case (c->moduleid_,
							        c->caseid_,
								groupid);
	}
	dl_list_free (&user_selected_cases);
	user_selected_cases = dl_list_create();
}

/* ------------------------------------------------------------------------- */
/** Starts executing test cases parallel
 */
LOCAL void start_cases_parallel (void *p)
{
	CUICaseData *c;
	DLListIterator it;

        get_selected_cases ();
	for (it = dl_list_head (user_selected_cases); it != INITPTR;
	     it = dl_list_next (it)) {
		c  = (CUICaseData*)dl_list_data(it);
		if (min_clbk_.start_case) min_clbk_.start_case (c->moduleid_,
							        c->caseid_,
								0);
	}
	dl_list_free (&user_selected_cases);
	user_selected_cases = dl_list_create();
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
        DLListItem     *dl_item_po = INITPTR;
        ExecutedTestCase   *tc = INITPTR;
        Text *printout = INITPTR;
        int             n = 0;

        if (p != INITPTR && p != NULL) {
                tc = (ExecutedTestCase *) p;
        } else
                return;
        /* free memory allocated for callback structure */
        free_cbs (cb_view_output_menu);

        /* get number of printouts */
        n = dl_list_size (tc->printlist_);

        if (n > 0) {
                /* allocate memory for menu callback structure */
                cb_view_output_menu =
                    (callback_s *) calloc (n + 1, sizeof (callback_s));
                if (cb_view_output_menu == NULL)
                        return;

                dl_item_po = dl_list_head (tc->printlist_);
                n = 0;

                /* iterate through printouts */
                while (dl_item_po != INITPTR && dl_item_po != NULL) {
                        printout = (Text *) dl_list_data (dl_item_po);

                        /* add printout to menu */
                        set_cbs (&cb_view_output_menu[n],
                                 tx_share_buf (printout) ? 
				 tx_get_buf (printout) : "<null>",
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
        update_menu (cb_view_output_menu, tx_share_buf (tc->case_->casetitle_),
		     1, NULL);
}

/* ------------------------------------------------------------------------- */
/** Views test case output for ongoing test case
 *  @param p pointer to DLListItem
 */
LOCAL void view_output_for_ongoing_cases (void *p)
{
        DLListItem     *dl_item_po = INITPTR;
        ExecutedTestCase       *tc = INITPTR;
        int                      n = 0;
        Text           *printout   = INITPTR;

        /* free memory allocated for callback structure */
        free_cbs (cb_view_output_menu);

        if (p != INITPTR && p != NULL) {
                tc = (ExecutedTestCase *) dl_list_data (p);
        } else
                return;

        /* get number of printouts */
        n = dl_list_size (tc->printlist_);

        if (n > 0 && tc->status_ != TCASE_STATUS_FINNISHED) {
                cb_view_output_menu = (callback_s *)
                    calloc (n + 1, sizeof (callback_s));
                if (cb_view_output_menu == NULL)
                        return;
                n = 0;

                /* get head of the test result list */
                dl_item_po = dl_list_head (tc->printlist_);
                while (dl_item_po != INITPTR) {
                        /* get test result */
                        printout = (Text *) dl_list_data (dl_item_po);
			
			set_cbs (&cb_view_output_menu[n],
				 tx_share_buf (printout) ? 
				 tx_get_buf (printout) : "<null>",
				 NULL,
				 NULL,
				 back_to_control_menu,
				 NULL, NULL, 0);
			
			n++;

			/* move to next printout item */
			dl_item_po =
				dl_list_next (dl_item_po);
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
        update_menu (cb_view_output_menu, tx_share_buf (tc->case_->casetitle_),
		     1, NULL);
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
        CUICaseData    *tc = INITPTR;
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
                        /* get CUICaseData from iterator */
                        tc = (CUICaseData *) dl_list_data (dl_item_tc);

                        if (tc != INITPTR && tc->casetitle_ != NULL) {
                                /* fill callback structure */
                                set_cbs (&cb_test_set_menu[i],
                                         tx_share_buf (tc->casetitle_),
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
	CUICaseData *c;
	DLListIterator it;
	
	groupid ++;
	for (it = dl_list_head (test_set); it != INITPTR;
	     it = dl_list_next (it)) {
		c  = (CUICaseData*)dl_list_data(it);
		if (min_clbk_.start_case) min_clbk_.start_case (c->moduleid_,
							        c->caseid_,
								groupid);
	}
	popup_window ("Start sequential test set execution", 1);
}

/* ------------------------------------------------------------------------- */
/** Starts test set parallel
 */
LOCAL void start_test_set_parallel ()
{
	CUICaseData *c;
	DLListIterator it;

	for (it = dl_list_head (test_set); it != INITPTR;
	     it = dl_list_next (it)) {
		c  = (CUICaseData*)dl_list_data(it);
		if (min_clbk_.start_case) min_clbk_.start_case (c->moduleid_,
							        c->caseid_,
								0);
	}
	
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

LOCAL int set_write_file (DLList * cases_list, char *filename)
{


        DLListIterator  case_it;
        DLListIterator  module_it;
        FILE           *set_file;
	CUICaseData    *tc;
	CUIModuleData  *mod;
   
        /*create string for filename - current
           dfate and hour */
        char           *name = strstr (filename, "200");

        MIN_DEBUG ("%d cases in set", dl_list_size (cases_list));
        MIN_DEBUG ("filename : %s", filename);

        set_file = fopen (filename, "w");
        fprintf (set_file, "[TestSetStart]\n");
        fprintf (set_file, "TestSetName=%s\n", name);

        case_it = dl_list_head (cases_list);
        /*write data to file */
        while (case_it != DLListNULLIterator) {
                fprintf (set_file, "[TestSetCaseStart]\n");
		tc = (CUICaseData *) dl_list_data (case_it);
                module_it = dl_list_find (dl_list_head (available_modules),
					  dl_list_tail (available_modules),
					  _find_mod_by_id,
					  &tc->moduleid_);
                if (module_it==DLListNULLIterator) break;

		mod = (CUIModuleData*)dl_list_data (module_it);

                fprintf (set_file, "ModuleName=%s\n", 
			 tx_share_buf (mod->modulename_));
                fprintf (set_file, "Title=%s\n", 
			 tx_share_buf (tc->casetitle_));
                fprintf (set_file, "[TestSetCaseEnd]\n");
                case_it = dl_list_next (case_it);
        }

        fprintf (set_file, "[TestSetEnd]\n");
        fclose (set_file);

        return 1;
}

LOCAL char *create_path ()
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

/* ------------------------------------------------------------------------- */
/** Saves test set
 */
LOCAL void save_test_set ()
{
        char           *filename = INITPTR;
        char           *str = "Save test set: ";
        char           *string = INITPTR;
        int             length = 0;

        filename = create_path ();
        set_write_file (test_set, filename);

        length = strlen (str) + strlen (filename) + 1;
        string = NEW2 (char, length);
        strcpy (string, str);
        strcat (string, filename);

        popup_window (string, -1);
        DELETE (string);
}

LOCAL CUICaseData *setgetcase (char *module, char *title)
{
        DLListIterator  module_it = dl_list_head (available_modules);
        DLListIterator  case_it = DLListNULLIterator;
	CUIModuleData  *mod;
	CUICaseData    *c;

	module_it = dl_list_find (dl_list_head (available_modules),
				  dl_list_tail (available_modules),
				  _find_mod_by_name,
				  module);
	if (module_it == INITPTR)
		return INITPTR;

	mod = dl_list_data (module_it);
	
	for (case_it = dl_list_head (case_list_); case_it != INITPTR;
	     case_it = dl_list_next (case_it)) {
		c = dl_list_data (case_it);
		if (c->moduleid_ ==  mod->moduleid_ &&
		    !strcmp (title, tx_share_buf (c->casetitle_)))
			return c;
	}
	
	return INITPTR;
}

/* ------------------------------------------------------------------------- */
/** Reads test set
 */
void set_read (DLList * set_cases_list, char *setname)
{

        char           *work_path;
        char           *c_dir;
        int             p_lenghth;
        int             caseread = 1;
        int             mod_result = 1;
        int             title_result = 1;
        CUICaseData    *tc = INITPTR;
        MinParser      *set_file;
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

                        tc = setgetcase (module_name_, case_title_);
                        dl_list_add (set_cases_list, tc);

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
/** Loads test set
 */
LOCAL void load_test_set (void *p)
{
        char           *filename = INITPTR;

        empty_test_set ();

        filename = (char *)dl_list_data ((DLListIterator) p);

        set_read (test_set, filename);
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
        DLListItem     *dl_item_tc = INITPTR;
        CUICaseData    *tc = INITPTR;
        int             n = 0;
        int             i = 0;

        /* free memory allocated for callback structure */
        free_cbs (cb_add_tcs_to_test_set_menu);

        /* count the number of all test cases */
	n = dl_list_size (case_list_);

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

                /* process linked list of all cases */
                for (dl_item_tc = dl_list_head (case_list_);
                     dl_item_tc != INITPTR;
                     dl_item_tc = dl_list_next (dl_item_tc)) {
			/* get CUICaseData from iterator */
			tc = (CUICaseData *)
				dl_list_data (dl_item_tc);

			if (tc == INITPTR || tc->casetitle_ == NULL)
				continue;
			/* fill callback structure */
			set_cbs (&cb_add_tcs_to_test_set_menu[i],
				 tx_get_buf (tc->casetitle_), NULL,
				 toggle_menu_item,
				 test_set_menu, NULL, tc, 1);
			i++;
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
        CUICaseData    *tc = INITPTR;
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
                        /* get CUICaseData from iterator */
                        tc = (CUICaseData *) dl_list_data (dl_item_tc);

                        if (tc != INITPTR && tc->casetitle_ != NULL) {
                                /* fill callback structure */
                                set_cbs (&cb_remove_tcs_from_test_set_menu[i],
                                         tx_get_buf(tc->casetitle_),
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
        CUICaseData    *tc = INITPTR;

        items = menu_items (my_menu);

        /* iterate through menu items to find highlighted ones */
        for (i = 2; i < item_count (my_menu); ++i) {
                /* test if menu item is highlighted */
                if (item_value (items[i]) == TRUE) {
                        /* get user data attached to menu item */
                        cb = (callback_s *) item_userptr (items[i]);
                        tc = (CUICaseData *) cb->ptr_data;
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
 *  This function is used with DLLIST function dl_list_find().
 *  @param data1 is data from the list
 *  @param data2 value provided by the user.
 *  @return 0 when elements are equal, 1 when first is greater than second,
 *          -1 otherwise.
 */
LOCAL int compare_items (const void *data1, const void *data2)
{
        int             result = -2;
        CUICaseData    *tc1 = INITPTR;
	CUICaseData    *tc2 = INITPTR;

        if ((data1 != INITPTR) && (data2 != INITPTR)) {
                tc1 = (CUICaseData *) data1;
                tc2 = (CUICaseData *) data2;

                if (data1 > data2)
                        result = 1;
                else if (data1 == data2)
                        result = 0;
                else
                        result = -1;
        }
        return result;
}

/* ------------------------------------------------------------------------- */
/** Clears all log entries 
 */
LOCAL void flush_log (void *p)
{
	DLListIterator it, next_it;
	Text *tx;

	for (it = dl_list_head (error_list_); it != INITPTR; 
	     it = next_it) {
		next_it = dl_list_next (it);
		tx = dl_list_data (it);
		dl_list_remove_it (it);
		tx_destroy (&tx);
	}

	cui_refresh_log_view ();

	return;
}

/** Scrolls a log item horizontally, in case it does not fit the window
 */
LOCAL void side_scroll_log_item (void *p)
{
	Text *tx;

	tx = dl_list_data ((DLListIterator)p);
	side_scroll_line (tx_share_buf (tx), 3);
	
	return;
}

/* ======================== FUNCTIONS ====================================== */
void callbacks_cleanup ()
{
	free_cbs (cb_module_menu);
	free_cbs (cb_start_new_case_menu);
	free_cbs (cb_run_multiple_tests_menu);
	free_cbs (cb_ongoing_cases_menu);
	free_cbs (cb_executed_cases_menu);
	free_cbs (cb_passed_cases_menu);
	free_cbs (cb_failed_cases_menu);
	free_cbs (cb_aborted_cases_menu);
	free_cbs (cb_test_result_menu);
	free_cbs (cb_pause_resume_abort_menu);
	free_cbs (cb_view_output_menu);
	free_cbs (cb_test_set_menu);
	free_cbs (cb_add_tcs_to_test_set_menu);
	free_cbs (cb_remove_tcs_from_test_set_menu);
	free_cbs (cb_load_test_set_menu);
	free_cbs (cb_add_test_module_menu);
	free_cbs (cb_add_test_case_files_menu);
}


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
