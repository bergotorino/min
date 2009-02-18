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
#include "consoleui.h"
#include <string.h>             /* for strlen() */
#include <stdlib.h>             /* for calloc() */
#include <menu.h>               /* for ncurses  */
#include <unistd.h>             /* for usleep() */
#include <pthread.h>            /* for mutex */

#include <min_system_logger.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
extern callback_s cb_main_menu[];       /* main menu structure */
extern DLList  *test_set_files;
extern DLList  *test_modules;
extern DLList  *test_case_files;
extern focus_pos_s main_menu_focus;

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
/** main window containing the border and title */
WINDOW         *main_window = INITPTR;
/** window on which the user interacts with the menu */
WINDOW         *menu_window = INITPTR;
/** the menu itsef */
MENU           *my_menu = INITPTR;
/** flag indicating whether to continue program */
bool            continue_ = true;

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
/* consoleui specific mutex to protect data */
LOCAL pthread_mutex_t CUI_MUTEX = PTHREAD_MUTEX_INITIALIZER;
/* height of the main window */
LOCAL int       maxy = 0;
/* width of the main window */
LOCAL int       maxx = 0;
/* current menu's focus position */
LOCAL focus_pos_s       *focus_pos = &main_menu_focus;

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
LOCAL void      init_ncurses (void);
/* ------------------------------------------------------------------------- */
LOCAL void      quit (void);
/* ------------------------------------------------------------------------- */
LOCAL void      wclrscr (WINDOW * pwin);
/* ------------------------------------------------------------------------- */
LOCAL void      wCenterTitle (WINDOW * pwin, const char *title);
/* ------------------------------------------------------------------------- */
LOCAL void      init_main_window (void);
/* ------------------------------------------------------------------------- */
LOCAL void      create_main_window (void);
/* ------------------------------------------------------------------------- */
LOCAL void      delete_menu (void);
/* ------------------------------------------------------------------------- */
LOCAL void      create_menu (callback_s * cb, const char *string);
/* ------------------------------------------------------------------------- */
LOCAL void      wclreoln (WINDOW * pwin, int y, int x);
/* ------------------------------------------------------------------------- */
LOCAL void      restore_focus_pos (void);
/* ------------------------------------------------------------------------- */
LOCAL void      save_focus_pos (int index, int top_row);

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

        /* unpost and free all allocated menu and item resources */
        unpost_menu (my_menu);
        free_menu (my_menu);
        for (i = 0; i < n_choices; ++i)
                free_item (my_items[i]);

        /* destroy menu windows and main window */
        delwin (menu_window);
        delwin (main_window);

        /* make cursor visible again */
        curs_set (1);

        /* update screen contents */
        touchwin (stdscr);
        wrefresh (stdscr);

        /* end ncurses */
        endwin ();
}

/* ------------------------------------------------------------------------- */
/** Clears the specified window
 *  @param pwin pointer to window
 */
LOCAL void wclrscr (WINDOW * pwin)
{
        int             y = 0;
        int             x = 0;
        int             maxy = 0;
        int             maxx = 0;

        /* get dimensions of the window */
        getmaxyx (pwin, maxy, maxx);

        for (y = 0; y < maxy; y++)
                for (x = 0; x < maxx; x++)
                        /* write single character to window */
                        mvwaddch (pwin, y, x, ' ');
}

/* ------------------------------------------------------------------------- */
/** Erases the current line from the cursor to the end of the line in the 
 *  specified window
 *  @param pwin pointer to window
 *  @param y y-coordinate of the cursor
 *  @param x x-coordinate of the cursor
 */
LOCAL void wclreoln (WINDOW * pwin, int y, int x)
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
LOCAL void wCenterTitle (WINDOW * pwin, const char *title)
{
        int             x = 0;
        int             maxy = 0;
        int             maxx = 0;
        int             stringsize = 0;

        /* get dimensions of the window */
        getmaxyx (pwin, maxy, maxx);

        stringsize = 2 + strlen (title);
        x = (maxx - stringsize) / 2;

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
LOCAL void create_main_window ()
{
        /* get dimensions of the entire screen */
        getmaxyx (stdscr, maxy, maxx);

        /* set up main window */
        main_window = newwin (maxy, maxx, WIN_X, WIN_Y);

        wclrscr (main_window);
        keypad (main_window, TRUE);

        /* print a border around the main window and print a title */
        box (main_window, 0, 0);
        wCenterTitle (main_window, "MIN TEST FRAMEWORK");

        /* make cursor invisible */
        curs_set (0);

        /* update screen contents */
        touchwin (main_window);
        wrefresh (main_window);
}

/* ------------------------------------------------------------------------- */
/** Creates and displays menu
 *  @param cb pointer to menu callback structure
 *  @param string menu name
 */
LOCAL void create_menu (callback_s * cb, const char *string)
{
        int             i = 0;
        int             menu_win_height = maxy - MENU_WIN_Y - 1;
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

        /* set up window for the menu's user interface */
        menu_window = derwin (main_window,
                              menu_win_height,
                              menu_win_width, MENU_WIN_Y, MENU_WIN_X);

        /* clears menu window */
        wclrscr (menu_window);

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
        wclreoln (main_window, 1, 1);
        mvwaddstr (main_window, 1, 1, string);

        /* display the menu */
        post_menu (my_menu);
}

/* ------------------------------------------------------------------------- */
/** Initializes main window and menu
 */
LOCAL void init_main_window ()
{
        create_main_window ();
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

/* ======================== FUNCTIONS ====================================== */
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

        /* create linked list for test modules */
        test_modules = dl_list_create ();

        /* create linked list for testcase files */
        test_case_files = dl_list_create ();

        /* create linked list for test set */
        test_set = dl_list_create ();

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
                touchwin (main_window);
                wrefresh (main_window);

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
                        touchwin (main_window);
                        wrefresh (main_window);
                }
        }

        /* release cui mutex */
        pthread_mutex_unlock (&CUI_MUTEX);
}

/* ------------------------------------------------------------------------- */
/** Creates and shows popup window for showing information to user
 *  @param string text to print on the popup window
 *  @param time time in seconds to show popup window, or -1 if user has to 
 * press some key to continue
 */
void popup_window (char *string, int time)
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
        wclrscr (small_win);
        box (small_win, 0, 0);
        wCenterTitle (small_win, "Info");
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