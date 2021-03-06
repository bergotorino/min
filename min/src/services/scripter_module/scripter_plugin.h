/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Konrad Marek Zapalowicz
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
 *  @file       scripter_plugin.h
 *  @version    0.1
 *  @brief      This file contains header file of the Scripter Plugin.
 */

#ifndef SCRIPTER_PLUGIN_H
#define SCRIPTER_PLUGIN_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
/* ------------------------------------------------------------------------- */
#include <min_common.h>
#include <scripter_keyword.h>
#include <scripter_common.h>
#include <min_scripter_if.h>
#include <min_lego_interface.h>
#include <min_item_parser.h>
#include <dllist.h>
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
/* ------------------------------------------------------------------------- */
/* None */

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
char           *validate_test_case (MinSectionParser * testcase, 
				    char **description);
/* ------------------------------------------------------------------------- */
int             validate_line (char *keyword,  MinItemParser * line, 
			       int line_number, char **p_tc_title, 
			       Text *tx_desc, DLList *assoc_lnames,
			       DLList *assoc_cnames, DLList *symblist,
			       DLList *var_list, DLList *requested_events,
			       DLList *slaves, DLList *interf_objs, 
			       char *nesting, int *nest_level);
/* ------------------------------------------------------------------------- */
void            interpreter_handle_keyword (TScripterKeyword keyword,
					    MinItemParser * mip);
/* ------------------------------------------------------------------------- */
int             interpreter_next ();
/* ------------------------------------------------------------------------- */
int             scripter_init (minScripterIf * scripter_if);
/* ------------------------------------------------------------------------- */
void            do_cleanup (DLList *slaves, 
			    DLList *testclasses, 
			    DLList *classmethods,
			    DLList *assoc_cnames,
			    DLList *assoc_lnames,
			    DLList *requested_events,
			    DLList *symblist,
			    DLList *var_list,
			    DLList *interf_objs);
/* ------------------------------------------------------------------------- */
 
#endif                          /* SCRIPTER_PLUGIN_H */
/* End of file */
