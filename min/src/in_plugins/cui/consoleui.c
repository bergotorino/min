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
 *  @file       consoleui.c
 *  @version    0.1
 *  @brief      This file contains implementation of console UI of MIN
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <string.h>             /* for strlen() */
#include <stdlib.h>             /* for calloc() */
#include <menu.h>               /* for ncurses  */
#include <unistd.h>             /* for usleep() */
#include <pthread.h>            /* for mutex */

#include "consoleui.h"
#include <min_system_logger.h>
#include <min_plugin_interface.h>
#include <min_text.h>
#include <min_logger.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
extern callback_s cb_main_menu[];       /* main menu structure */
extern DLList  *test_set_files;
extern focus_pos_s main_menu_focus;
/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
extern int not_in_curses;

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */


/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
/** main window containing the border and title */
WINDOW         *main_window = INITPTR;
/** log window containing  */
WINDOW         *log_window = INITPTR;
/** window on which the user interacts with the menu */
WINDOW         *menu_window = INITPTR;
/** the menu itsef */
MENU           *my_menu = INITPTR;
/** flag indicating whether to continue program */
bool            continue_ = true;

eapiIn_t        out_clbk_;
eapiOut_t       min_clbk_;

/* List available of cases */
DLList *case_list_ = INITPTR;
/* List executed cases */
DLList *executed_case_list_ = INITPTR;
/* List of error prints */
DLList *error_list_ = INITPTR;

/* List of modules */
DLList  *available_modules = INITPTR;
/* List of test case files selected for added module */
DLList *found_tcase_files = INITPTR;
/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
/* list of items on the menu */
LOCAL ITEM    **my_items = INITPTR;
/* number of items on the menu */
LOCAL int       n_choices = 0;
/* flag telling whether menu needs refresh */
LOCAL int       update_ = 0;
/* current selected menu item */
LOCAL int       my_choice = 0;
/* top row of the menu */
LOCAL int       my_row = 0;
/* function to call when refresh is needed */
LOCAL ptr_to_fun func = INITPTR;
/* funtion to call when refresh is needed */
LOCAL ptr_to_fun2 func2 = INITPTR;
/* parameter to data to pass func2 */
LOCAL void     *data = INITPTR;
/* height of the main window */
LOCAL int       maxy = 0;
/* width of the main window */
LOCAL int       maxx = 0;
/* current menu's focus position */
LOCAL focus_pos_s       *focus_pos = &main_menu_focus;
/* consoleui specific mutex to protect data */
LOCAL pthread_mutex_t CUI_MUTEX = PTHREAD_MUTEX_INITIALIZER;
/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
LOCAL void      init_ncurses (void);
/* ------------------------------------------------------------------------- */
LOCAL void      quit (void);
/* ------------------------------------------------------------------------- */
LOCAL void      cui_print_title (WINDOW * pwin, const char *title);
/* ------------------------------------------------------------------------- */
LOCAL void      init_main_window (void);
/* ------------------------------------------------------------------------- */
LOCAL void      create_main_window (int ysize, int xsize);
/* ------------------------------------------------------------------------- */
LOCAL void      delete_menu (void);
/* ------------------------------------------------------------------------- */
LOCAL void      create_menu (callback_s * cb, const char *string);
/* ------------------------------------------------------------------------- */
LOCAL void      cui_clear_from_pos (WINDOW * pwin, int y, int x);
/* ------------------------------------------------------------------------- */
LOCAL void      restore_focus_pos (void);
/* ------------------------------------------------------------------------- */
LOCAL void      save_focus_pos (int index, int top_row);
/* ------------------------------------------------------------------------- */
LOCAL ExecutedTestCase *get_executed_tcase_with_runid (long testrunid);
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_result (long testrunid, int result, char *desc,
			   long starttime, long endtime);
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_started (unsigned moduleid,
			    unsigned caseid,
			    long testrunid);
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_paused (long testrunid);
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_resumed (long testrunid);
/* ------------------------------------------------------------------------- */
LOCAL void pl_msg_print (long testrunid, char *message);
/* ------------------------------------------------------------------------- */
LOCAL void pl_new_module (char *modulename, unsigned moduleid);
/* ------------------------------------------------------------------------- */
LOCAL void pl_no_module (char *modulename);
/* ------------------------------------------------------------------------- */
LOCAL void pl_new_case (unsigned moduleid, unsigned caseid, char *casetitle);
/* ------------------------------------------------------------------------- */
LOCAL void pl_error_report (char *error);
/* ------------------------------------------------------------------------- */
LOCAL void cui_clear_line (WINDOW * pwin, int y);
/* ------------------------------------------------------------------------- */
LOCAL void cui_clear_win (WINDOW * pwin);
/* ------------------------------------------------------------------------- */
LOCAL void free_executed_case (void *p);
/* ------------------------------------------------------------------------- */
LOCAL void free_etc_print (void *p);
/* ------------------------------------------------------------------------- */
LOCAL void free_case (void *);
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/** Initializes ncurses
 */
LOCAL void init_ncurses ()
{
        initscr ();             /* start ncurses mode */
	start_color();
	init_pair(1, COLOR_CYAN, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);

	cbreak ();              /* line buffering disabled, pass on */
        /* everything to me */
        noecho ();              /* don't echo while we do getch */
        keypad (stdscr, TRUE);  /* enables reading arrow keys */
}

/* ------------------------------------------------------------------------- */
/** Ends ncurses and free all the memory taken up
 */
LOCAL void quit ()
{
        int             i = 0;
	void           *tmp = NULL;

	display_info ("Exiting MIN...", 1);

        if (min_clbk_.min_close) min_clbk_.min_close ();

        /* unpost and free all allocated menu and item resources */
        unpost_menu (my_menu);
        free_menu (my_menu);
        for (i = 0; i < n_choices; ++i)
                free_item (my_items[i]);

        /* destroy menu windows and main window */
        delwin (menu_window);
        delwin (main_window);
	/* free the log entries */
	flush_log (tmp);
	/* free data from callbacks */
	callbacks_cleanup ();
	/* free the list of executed test cases */
	dl_list_foreach (dl_list_head (executed_case_list_), 
			 dl_list_tail (executed_case_list_),
			 free_executed_case);
	dl_list_free (&executed_case_list_);
	/* free the list of available cases */
	dl_list_foreach (dl_list_head (case_list_), 
			 dl_list_tail (case_list_),
			 free_case);
	dl_list_free (&case_list_);

        /* make cursor visible again */
        curs_set (1);

        /* update screen contents */
	if (!not_in_curses) {
		touchwin (stdscr);
		wrefresh (stdscr);
	}
        /* end ncurses */
        endwin ();
}


/* ------------------------------------------------------------------------- */
/** Erases the current line from the cursor to the end of the line in the 
 *  specified window
 *  @param pwin pointer to window
 *  @param y y-coordinate of the cursor
 *  @param x x-coordinate of the cursor
 */
LOCAL void cui_clear_from_pos (WINDOW * pwin, int y, int x)
{
        int             maxx = 0;
        int             maxy = 0;
        int             i = 0;

        /* get dimensions of the window */
        getmaxyx (pwin, maxy, maxx);

        for (i = x; i < maxx - 1; i++)
                /* write single character to window */
                mvwaddch (pwin, y, i, ' ');
}

/* ------------------------------------------------------------------------- */
/** Adds centered title to specified window's top line
 *  @param pwin pointer to window
 *  @param title text to add
 */
LOCAL void cui_print_title (WINDOW * pwin, const char *title)
{
        int             maxx;
	unsigned        x;

	maxx = getmaxx (pwin);
        x = (maxx - (strlen (title) + 2)) / 2;

        /* write center title to window's top line */
        mvwaddch (pwin, 0, x, ' ');
        waddstr (pwin, title);
        waddch (pwin, ' ');
}

/* ------------------------------------------------------------------------- */
/** Deletes menu and frees all the memory taken up
 */
LOCAL void delete_menu ()
{
        int             i = 0;

        /* unpost and free all the memory taken up */
        unpost_menu (my_menu);
        free_menu (my_menu);
        for (i = 0; i < n_choices; ++i)
                free_item (my_items[i]);

        /* destroy window and deallocate the memory associated with the window
           structure */
        delwin (menu_window);
}

/* ------------------------------------------------------------------------- */
/** Creates and displays main window
 */
LOCAL void create_main_window (int ysize, int xsize)
{

        /* set up main window */
        main_window = newwin (ysize, xsize, WIN_X, WIN_Y);

        cui_clear_win (main_window);
        keypad (main_window, TRUE);

        /* print a border around the main window and print a title */
        box (main_window, 0, 0);
        cui_print_title (main_window, "MIN TEST FRAMEWORK");

        /* make cursor invisible */
        curs_set (0);
        /* update screen contents */
	if (!not_in_curses) {
		touchwin (main_window);
		wrefresh (main_window);
	}
}

/* ------------------------------------------------------------------------- */
/** Creates and displays log window
 */
LOCAL void create_log_window ()
{
        /* get dimensions of the entire screen */
        getmaxyx (stdscr, maxy, maxx);

        /* set up main window */
        log_window = newwin (LOG_WIN_ROWS, maxx, maxy - LOG_WIN_ROWS, WIN_X);

        cui_clear_win (log_window);
        keypad (log_window, FALSE);

        /* print a border around the log window */
        box (log_window, 0, 0);

        /* update screen contents */
	if (!not_in_curses) {
		touchwin (log_window);
		wrefresh (log_window);
	}
}


/* ------------------------------------------------------------------------- */
/** Creates and displays menu
 *  @param cb pointer to menu callback structure
 *  @param string menu name
 */
LOCAL void create_menu (callback_s * cb, const char *string)
{
        int             i = 0;
        int             menu_win_height = maxy - MENU_WIN_Y - LOG_WIN_ROWS - 1;
        int             menu_win_width = maxx - MENU_WIN_X - 1;

        /* calculate number of menu choices */
        for (n_choices = 0; cb[n_choices].choice; n_choices++);

        /* allocate item array and individual items */
        my_items = (ITEM **) calloc (n_choices + 1, sizeof (ITEM *));
        for (i = 0; i < n_choices; ++i) {
                my_items[i] = new_item (cb[i].choice, cb[i].desc);
                set_item_userptr (my_items[i], &cb[i]);
        }
        my_items[n_choices] = (ITEM *) NULL;

        /* create the menu */
        my_menu = new_menu ((ITEM **) my_items);
	set_menu_fore(my_menu, COLOR_PAIR(1));
	set_menu_back(my_menu, COLOR_PAIR(2));
	set_menu_grey(my_menu, COLOR_PAIR(3));

        /* set up window for the menu's user interface */
        menu_window = derwin (main_window,
                              menu_win_height,
                              menu_win_width, MENU_WIN_Y, MENU_WIN_X);
        /* clears menu window */
        cui_clear_win (menu_window);
	/* associate these windows with the menu */
        set_menu_win (my_menu, main_window);
        set_menu_sub (my_menu, menu_window);

        /* specify number of rows and columns on the menu */
        set_menu_format (my_menu, menu_win_height, MENU_COLUMNS);

        /* make the menu multivalued */
        menu_opts_off (my_menu, O_ONEVALUE);

        /* wrap around next item and previous item, requests to the other */
        /* end of the menu */
        menu_opts_off (my_menu, O_NONCYCLIC);

        /* put '*' to the left on highlighted item */
        set_menu_mark (my_menu, "*");

        /* place string at y,x */
        cui_clear_from_pos (main_window, 1, 1);
        mvwaddstr (main_window, 1, 1, string);

        /* display the menu */
        post_menu (my_menu);
}

/* ------------------------------------------------------------------------- */
/** Initializes main window and menu
 */
LOCAL void init_main_window ()
{
        /* get dimensions of the entire screen */
        getmaxyx (stdscr, maxy, maxx);
	if (maxy > LOG_WIN_ENABLE_LIMIT) {
		create_main_window (maxy - LOG_WIN_ROWS, maxx);
		create_log_window ();
	}
	else
		create_main_window (maxy, maxx);
        create_menu (cb_main_menu, "Main Menu");
}

/* ------------------------------------------------------------------------- */
/** Restores focus position
 *  @param position pointer to focus position to restore
 */
LOCAL void restore_focus_pos ()
{
        /* update current menu focus */
        if (focus_pos != NULL) {
                if (focus_pos->index < n_choices && focus_pos->index >= 0) {
                        if (focus_pos->top_row > 0) {
                                set_top_row (my_menu, focus_pos->top_row);
                        }

                        ITEM *item = my_items[focus_pos->index];

                        if (item != NULL) {
                                set_current_item (my_menu, item);
                        }
                }
        }
}

/* ------------------------------------------------------------------------- */
/** Saves focus position
 *  @param index index of the focused menu item
 *  @param top_row top row of the shown menu
 */
LOCAL void save_focus_pos (int index, int top_row)
{
        if (focus_pos != NULL ) {
                focus_pos->index = index;
                focus_pos->top_row = top_row;
        }
}

/* ------------------------------------------------------------------------- */
/** Searches executed test case with test run id 
 *  @param testrunid search key
 *  @return pointer to ExecutedTestCase structure of INITPTR if not found
 */

LOCAL ExecutedTestCase *get_executed_tcase_with_runid (long testrunid)
{
	DLListIterator it;
	ExecutedTestCase *etc;

	for (it = dl_list_head (executed_case_list_);
	     it != INITPTR;
	     it = dl_list_next (it)) {
		etc = (ExecutedTestCase *)dl_list_data (it);
		if (etc->runid_ == testrunid)
			return etc;
	}
	
	return INITPTR;
}

/* ------------------------------------------------------------------------- */
LOCAL void pl_case_result (long testrunid, int result, char *desc,
			   long starttime, long endtime)
{
	ExecutedTestCase *etc;
	MIN_DEBUG ("Finnished: run id = %ld with result: %d", testrunid, result);
	etc = get_executed_tcase_with_runid (testrunid);
	if (etc == INITPTR) {
		MIN_ERROR ("No test found with run id %ld", testrunid);
		return;
	}
	if (etc->status_ != TCASE_STATUS_ONGOING) {
		MIN_WARN ("Test result for a case with status "
			  "other than ogoing");
	}
	etc->status_ = TCASE_STATUS_FINNISHED;
	etc->resultdesc_ = tx_create (desc);
        etc->result_ = result;
        etc->starttime_ = starttime;
        etc->endtime_ = endtime;
	
	if (not_in_curses) {
		reset_prog_mode();
		refresh();
		touchwin (main_window);
		wrefresh (main_window);
		touchwin (menu_window);
		wrefresh (menu_window);
		touchwin (log_window);
		wrefresh (log_window);
		not_in_curses = 0;
	} 
	cui_refresh_view();
	  
}
/* ------------------------------------------------------------------------- */
LOCAL int _find_case_by_id (const void *a, const void *b)
{
        CUICaseData * tmp1 = (CUICaseData*)a;
        unsigned * tmp2 = (unsigned*)b;

        if (tmp1->caseid_==(*tmp2)) return 0;
        else return -1;
}
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_started (unsigned moduleid,
			    unsigned caseid,
			    long testrunid)
{
        ExecutedTestCase *tmp = INITPTR;
        DLListIterator it = DLListNULLIterator;
        DLListIterator begin = DLListNULLIterator;

	MIN_DEBUG ("run id = %ld caseid %d", testrunid, caseid);

        /* Case has been started, add it to the executed cases list and set
           its status to ongoing.
        */
        if (executed_case_list_==INITPTR) {
                executed_case_list_ = dl_list_create();
        }

        /* Running test case details, also pointer to test case details should
           be added here.
        */
        tmp = NEW2(ExecutedTestCase,1);
        tmp->status_ = 1;
        tmp->result_ = -1;
        tmp->resultdesc_ = INITPTR;
        tmp->runid_ = testrunid;
	tmp->printlist_ = dl_list_create();
        tmp->starttime_ = 0;
        tmp->endtime_ = 0;

        begin = dl_list_head (case_list_);
        do {
                it = dl_list_find (begin,
				   dl_list_tail (case_list_),
				   _find_case_by_id,
				   (void *)&caseid);
                if (it==DLListNULLIterator) break;
                CUICaseData *ccd = (CUICaseData*)dl_list_data(it);
                if (ccd == INITPTR) break;
                if (ccd->moduleid_!=moduleid) begin = dl_list_next(it);
                else break;
        } while (it!=DLListNULLIterator);
        if (it==DLListNULLIterator) {
                DELETE (tmp);
        }
        tmp->case_ = dl_list_data(it);

        /* add to list */
        dl_list_add (executed_case_list_,(void*)tmp);
}
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_paused (long testrunid)
{
	ExecutedTestCase *etc;
	
	etc = get_executed_tcase_with_runid (testrunid);
	if (etc != INITPTR) {
		etc->status_ = TCASE_STATUS_PAUSED;
	}
	
}
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_resumed (long testrunid)
{
	ExecutedTestCase *etc;
	
	etc = get_executed_tcase_with_runid (testrunid);
	if (etc != INITPTR) {
		etc->status_ = TCASE_STATUS_ONGOING;
	}
}
/* ------------------------------------------------------------------------- */
LOCAL void pl_msg_print (long testrunid, char *message)
{
	ExecutedTestCase *etc;
	
	etc = get_executed_tcase_with_runid (testrunid);
	if (etc != INITPTR) {
		dl_list_add (etc->printlist_, tx_create (message));
	}

}
/* ------------------------------------------------------------------------- */
LOCAL void pl_new_module (char *modulename, unsigned moduleid)
{
	CUIModuleData *cmd = INITPTR;
        DLListIterator it = DLListNULLIterator;
        char *casefile = INITPTR;

	if (available_modules == INITPTR) available_modules = dl_list_create();
	
	MIN_DEBUG ("modulename = %s, moduleid %d", modulename, moduleid);

	/* add new module to list */
        cmd = NEW(CUIModuleData);
        cmd->moduleid_ = moduleid;
        cmd->modulename_ = tx_create(modulename);
        dl_list_add (available_modules, (void*)cmd);

        /* Module added by add module so we can add test case files for it */
        if (found_tcase_files!=INITPTR) {
                it = dl_list_tail(found_tcase_files);
                while (it!=DLListNULLIterator) {
                        casefile = (char*)dl_list_data(it);
                        MIN_DEBUG ("moduleid = %d, casefile = %s",
				   moduleid,casefile);
                        min_clbk_.add_test_case_file (moduleid,casefile);
                        dl_list_remove_it(it);
                        it = dl_list_tail(found_tcase_files);
                }
                dl_list_free(&found_tcase_files);
		min_clbk_.add_test_case_file (moduleid,"\0");
		display_info ("Module added", 1);

        }
	return;

}
/* ------------------------------------------------------------------------- */
LOCAL void pl_module_ready (unsigned moduleid)
{
	static unsigned int  ready_module_count_ = 0;

        ready_module_count_ ++;
	if (ready_module_count_ == dl_list_size (available_modules)) {
	        display_info ("All cases loaded", 1);
	}
}
/* ------------------------------------------------------------------------- */
LOCAL void pl_no_module (char *modulename)
{
        MIN_WARN ("Module %s has not been loaded",modulename);        
        cui_refresh_view();
}
/* ------------------------------------------------------------------------- */
LOCAL void pl_new_case (unsigned moduleid, unsigned caseid, char *casetitle)
{
        CUICaseData *ccd = INITPTR;        

        /* add new case to some list */
        if (case_list_==INITPTR) case_list_ = dl_list_create();

        MIN_DEBUG ("moduleid = %d, caseid = %d, casetitle = %s",
		   moduleid,caseid,casetitle);

        ccd = NEW(CUICaseData);
        ccd->moduleid_ = moduleid;
        ccd->caseid_  = caseid;
        ccd->casetitle_ = tx_create(casetitle);
        dl_list_add (case_list_,(void*)ccd);

        /* update the screen */
        cui_refresh_view();
}

/* ------------------------------------------------------------------------- */
LOCAL void pl_error_report (char *error) {
	Text *txt;
	
	txt = tx_create(error);
	dl_list_add (error_list_, txt);
	cui_refresh_log_view ();
}

/* ------------------------------------------------------------------------- */
/** Clears one line of the specified window
 *  @param y vertical position of line to clear
 *  @param pwin pointer to window
 */
LOCAL void cui_clear_line (WINDOW * pwin, int y)
{
        int             x = 0;
	for (x = 0; x < getmaxx (pwin); x++)
		mvwaddch (pwin, y, x, ' ');
}

/* ------------------------------------------------------------------------- */
/** Clears the specified window
 *  @param pwin pointer to window
 */
LOCAL void cui_clear_win (WINDOW * pwin)
{
        int             y = 0;

        for (y = 0; y < getmaxy (pwin); y++)
		cui_clear_line (pwin, y);
}
/* ------------------------------------------------------------------------- */
/** Frees the data on print list
 *  @param p pointer to Txt
 */
LOCAL void free_etc_print (void *p)
{
	Text *tx;
	tx = (Text *)p;
	tx_destroy (&tx);
}

/* ------------------------------------------------------------------------- */
/** Frees the data of ExecutedTestCase
 *  @param p pointer to ExecutedTestCase 
 */
LOCAL void free_executed_case (void *p)
{
	ExecutedTestCase *etc;

	etc = (ExecutedTestCase *)p;
	tx_destroy (&etc->resultdesc_);
	dl_list_foreach (dl_list_head (etc->printlist_), 
			 dl_list_tail (etc->printlist_),
			 free_etc_print);
	dl_list_free (&etc->printlist_);
	DELETE (etc);
}

/* ------------------------------------------------------------------------- */
/** Frees the CUICaseData
 *  @param p pointer to CUICaseData
 */
LOCAL void free_case (void *p)
{
	CUICaseData *tc;

	tc = (CUICaseData *)p;
	tx_destroy (&tc->casetitle_);
	DELETE (tc);
}

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
void pl_attach_plugin (eapiIn_t **out_callback, eapiOut_t *in_callback)
{
        /* Binds the callbacks */
        memcpy (&min_clbk_,in_callback,sizeof(eapiOut_t));

        (*out_callback)->case_result            = pl_case_result;
        (*out_callback)->case_started           = pl_case_started;
        (*out_callback)->case_paused            = pl_case_paused;
        (*out_callback)->case_resumed           = pl_case_resumed;
        (*out_callback)->module_prints          = pl_msg_print;
        (*out_callback)->new_module             = pl_new_module;
        (*out_callback)->no_module              = pl_no_module;
        (*out_callback)->module_ready           = pl_module_ready;
        (*out_callback)->new_case               = pl_new_case;
        (*out_callback)->error_report           = pl_error_report;

        return;
}
/* ------------------------------------------------------------------------- */
void pl_open_plugin (void *arg)
{
        if (min_clbk_.min_open) min_clbk_.min_open ();
        cui_exec();
        return;
}
/* ------------------------------------------------------------------------- */
void pl_close_plugin ()
{
        return;
}
/* ------------------------------------------------------------------------- */
void pl_detach_plugin (eapiIn_t **out_callback, eapiOut_t *in_callback)
{
        return;
}
/* ------------------------------------------------------------------------- */
/** Updates menu
 *  @param cb pointer to new menu to show
 *  @param string menu name
 *  @param update refresh flag, 0 menu needs refresh, else no need for refresh
 */
void update_menu (callback_s * cb, char *string, int update,
                  focus_pos_s *position)
{
        update_ = update;
        delete_menu ();
        create_menu (cb, string);

        focus_pos = position;
        restore_focus_pos ();
}

/* ------------------------------------------------------------------------- */
/** Starts the console UI program
 */
void cui_exec ()
{
        int             key = 0;
        ITEM           *cur_item = INITPTR;
        callback_s     *p = INITPTR;


        init_ncurses ();
        init_main_window ();

        /* create linked list for test cases */
        user_selected_cases = dl_list_create ();

        /* create linked list for test set files */
        test_set_files = dl_list_create ();

        /* create linked list for test set */
        test_set = dl_list_create ();

        /* create linked list for test cases */
        error_list_ = dl_list_create ();

        /* Acquire keystroke */
        while (continue_ && (key = wgetch (main_window))) {
                /* lock cui mutex */
                pthread_mutex_lock (&CUI_MUTEX);
                /* get current highlighted item */
                cur_item = current_item (my_menu);
                /* get user pointer of current highlighted item */
                p = item_userptr (cur_item);

                switch (key) {
                        /* down arrow key pressed */
                case KEY_DOWN:
                        /* move the current selection to an item down */
                        menu_driver (my_menu, REQ_DOWN_ITEM);
                        break;
                        /* up arrow key pressed */
                case KEY_UP:
                        /* move the current selection to an item up */
                        menu_driver (my_menu, REQ_UP_ITEM);
                        break;
                        /* Enter key */
                case 10:
                        /* right arrow key pressed */
                case KEY_RIGHT:
                        /* save focus position */
                        save_focus_pos (item_index (current_item (my_menu)),
                                        top_row (my_menu));

                        if (p->on_select_ != NULL) {
                                p->on_select_ (p->ptr_data);
                                func2 = p->on_select_;
                                data = p->ptr_data;
                        } else
                                func2 = INITPTR;
                        if (p->on_right_ != NULL) {
                                p->on_right_ ();
                                func = p->on_right_;
                        } else
                                func = INITPTR;
                        break;
                        /* left arrow key pressed */
                case KEY_LEFT:
                        /* save focus position */
                        save_focus_pos (item_index (current_item (my_menu)),
                                        top_row (my_menu));

                        if (p->on_left_ != NULL) {
                                p->on_left_ ();
                                func = p->on_left_;
                                func2 = INITPTR;
                        }
                        break;
                default:
                        break;
                }

                my_choice = item_index (current_item (my_menu));
                my_row = top_row (my_menu);

                /* update screen contents */
		if (!not_in_curses) {
			touchwin (main_window);
			wrefresh (main_window);
		}
                /* release cui mutex */
                pthread_mutex_unlock (&CUI_MUTEX);
        }

        /* quit program */
        quit ();
}

/* ------------------------------------------------------------------------- */
/** Updates UI view
 */
void cui_refresh_view ()
{
        /* lock cui mutex */
        pthread_mutex_lock (&CUI_MUTEX);

        /* check if current view needs refresh */
        if (update_) {
                if (func2 != INITPTR)
                        func2 (data);
                if (func != INITPTR)
                        func ();

                if (main_window != INITPTR && my_menu != INITPTR) {
                        /* select last menu item */
                        while (my_choice >= item_count (my_menu))
                                my_choice--;

                        if (my_row > 0)
                                set_top_row (my_menu, my_row);

                        /* update current item to be the same as 
                         * before refresh */
                        if (my_choice >= 0) {
                                ITEM           *item = my_items[my_choice];
                                set_current_item (my_menu, item);
                        }

                        /* refresh main window */
			if (!not_in_curses) {
				touchwin (main_window);
				wrefresh (main_window);
			}
                }

        }

        /* release cui mutex */
        pthread_mutex_unlock (&CUI_MUTEX);
}


/* ------------------------------------------------------------------------- */
/** Creates and shows an info window 
 *  @param string text to print on the window
 *  @param time time in seconds to show window, or -1 if user has to 
 *   press some key to continue
 */
void display_info (char *string, int time)
{
        WINDOW         *small_win;
        int             x = 0;
        int             y = 0;
        int             width = 0;
        int             height = 0;
        char           *str = "Press any key to continue";

        if ((time == -1) && (strlen (str) > strlen (string)))
                width = strlen (str) + 4;
        else
                width = strlen (string) + 4;
        if (width > (maxx - 2))
                width = maxx - 2;
        if (height > (maxy - 2))
                height = maxy - 2;

        height = 3;
        if (time == -1)
                height += 2;
        x = maxx / 2 - width / 2;
        y = maxy / 3 - height / 2;

        /* create and display the small window */
        small_win = newwin (height, width, y, x);
        cui_clear_win (small_win);
	wattron(small_win, COLOR_PAIR(3));

        box (small_win, 0, 0);
        cui_print_title (small_win, "Info");
        mvwaddstr (small_win, 1, 2, string);

        if (time == -1) {
                x = (width - strlen (str)) / 2;
                mvwaddstr (small_win, 3, x, "Press any key to continue");
        }

        touchwin (small_win);
        wrefresh (small_win);

        if (time == -1)
                wgetch (small_win);
        else
                sleep (time);

        /* delete small window */
        delwin (small_win);
        touchwin (main_window);
        wrefresh (main_window);

}

/* ------------------------------------------------------------------------- */
/** Updates Log view
 */
void cui_refresh_log_view ()
{
	DLListIterator it;
	int i = 0, maxy, maxx;
	Text *tx;

	if (log_window != INITPTR) {
		/* clear the log view */
		getmaxyx (log_window, maxy, maxx);
		cui_clear_win (log_window);

		box (log_window, 0, 0);
		for (it = dl_list_tail (error_list_); it != INITPTR;
		     it = dl_list_prev (it)) {
			i++;
			if (i >= LOG_WIN_ROWS - 1)
				break;
			tx = dl_list_data (it);
			mvwaddnstr (log_window, i, 1,  tx_share_buf (tx),
				    maxx - 2);
			
		}
		if (!not_in_curses) {
			touchwin (log_window);
			wrefresh (log_window);
		}
        }

}

/* ------------------------------------------------------------------------- */
/** Horizontally scrolls 1 line, in case it does not fit to window.
 * @param line the data to be displayed
 * @param indent indentation 
 */
void side_scroll_line (char *line, int indent)
{
	int  maxx, x, y;
	WINDOW  *scroll_win;
	char *p;

	getyx (main_window, y, x);
	maxx = getmaxx (menu_window);
	scroll_win = newwin (1, maxx, y , x);
	cui_clear_win (scroll_win);
	p = line;
	do {
		mvwaddnstr (scroll_win, 0, indent,  p,
			    maxx - indent);
		touchwin (scroll_win);
		wrefresh (scroll_win);
		halfdelay (1);
		if (wgetch (scroll_win) != ERR)
			break;
		p ++;
	} while (strlen (p) >= maxx - indent);
	cbreak ();
	wgetch (scroll_win);
        delwin (scroll_win);
        touchwin (main_window);
        wrefresh (main_window);
}

/* ------------------------------------------------------------------------- */

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
