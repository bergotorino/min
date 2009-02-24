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
 *  @file       callback.h
 *  @version    0.1
 *  @brief      This file contains header file of console UI of MIN
 */

#ifndef CALLBACK_H
#define CALLBACK_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <min_common.h>
#include <min_text.h>

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

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
/** data type for void function */
typedef void    (*ptr_to_fun) (void);
/** data type for void function that takes void* parameter */
typedef void    (*ptr_to_fun2) (void *p);

/** menu callback data type */
typedef struct {
        /** name of the menu item */
        char           *choice;
        /** description of the menu item */
        char           *desc;
        /** function to call when right arrow key is pressed */
        ptr_to_fun      on_right_;
        /** function to call when left arrow key is pressed */
        ptr_to_fun      on_left_;
        /** second function to call when right arrow key is pressed */
        ptr_to_fun2     on_select_;
        /** parameter to second function */
        void           *ptr_data;
} callback_s;

/** focus position data type */
typedef struct {
        /** index of the focus position */
        int index;
        /** top row of the shown menu */
        int top_row;
} focus_pos_s;

typedef struct {
        unsigned moduleid_;
        unsigned caseid_;
        Text *casetitle_;
} CUICaseData;

typedef struct {
#define TCASE_STATUS_INVALID   0
#define TCASE_STATUS_ONGOING   1
#define TCASE_STATUS_PAUSED    2
#define TCASE_STATUS_FINNISHED 3
        unsigned status_;
        unsigned result_;
        Text *resultdesc_;
} ExecutedTestCase;
/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
#endif                          /* CALLBACK_H */
/* End of file */
