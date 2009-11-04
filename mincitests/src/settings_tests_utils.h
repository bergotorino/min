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
 *  @file       settings_tests_utils.h
 *  @version    0.1
 *  @brief      Header for equal.c
 */

#ifndef SETTINGS_TESTS_UTILS_H
#define SETTINGS_TESTS_UTILS_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <dllist.h>
#include <min_parser.h>

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* ------------------------------------------------------------------------- */
/* None */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* ------------------------------------------------------------------------- */
typedef struct {
	int foo;
	DLList *bar_list;
} test_settings;
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* Structures */


/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
int test_settings_parse (void *data, MinSectionParser *);
/* ------------------------------------------------------------------------- */
void test_settings_clean (void *data);
/* ------------------------------------------------------------------------- */
void test_settings_init (void *data);

#endif                          /* SETTINGS_TESTS_UTILS_H */
