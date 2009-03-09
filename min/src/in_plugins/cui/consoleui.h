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
 *  @file       consoleui.h
 *  @version    0.1
 *  @brief      This file contains header file of console UI of MIN
 */

#ifndef CONSOLEUI_H
#define CONSOLEUI_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <callback.h>
#include <min_common.h>
#include <dllist.h>

/* ----------------------------------------------------------------------------
 * GLOBAL VARIABLES
 */
/** list of test cases selected for execution */
DLList         *user_selected_cases;
/** test set including user selected test cases */
DLList         *test_set;

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/** main window corner x-coordinate */
#define WIN_X           0
/** main window corner y-coordinate */
#define WIN_Y           0
/** menu window corner x-coordinate */
#define MENU_WIN_X      2
/** menu window corner y-coordinate */
#define MENU_WIN_Y      2
/** number of menu columns */
#define MENU_COLUMNS    1
/** Log window height */
#define LOG_WIN_ROWS    6 

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* None */
/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
void            update_menu (callback_s * cb, char *string, int update,
                             focus_pos_s *position);
/* ------------------------------------------------------------------------- */
void            cui_exec (void);
/* ------------------------------------------------------------------------- */
void            cui_refresh_view (void);
/* ------------------------------------------------------------------------- */
void            popup_window (char *string, int time);

/* ------------------------------------------------------------------------- */
#endif                          /* CONSOLEUI_H */
/* End of file */
