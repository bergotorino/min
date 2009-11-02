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
 *  @file       min_settings.c
 *  @version    0.1
 *  @brief      This file contains implementation of Settings.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <stdio.h>
#include <dllist.h>
#include <min_settings.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
DLList *settings_list;

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
SettingsSection *section_find (const char *tag)
{
	DLListIterator it;
	SettingsSection *s;
	
	for (it = dl_list_head (settings_list); 
	     it != INITPTR; 
	     it = dl_list_next(it)) {
		s = dl_list_data (it);

		if (strcmp (tag, tx_share_buf (s->tag_)) == 0)
			return s;
	}
	
	return INITPTR;
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Creates a new settings section entry
 *  @param tag [in] identifier for this settings section
 *  @param parse_func [in] function used for parsing settings for 
 *                         this section
 *  @param clean_func [in] function used for clean up
 *  @return pointer to new section or INITPTR on error.
 */
SettingsSection *new_section (const char *tag, int (*parse_func)(MinParser *),
			      void (*clean_func)())
{
	SettingsSection *s;

	if (tag == NULL || tag == INITPTR || parse_func == NULL ||
	    parse_func == INITPTR || clean_func == NULL || 
	    clean_func == INITPTR)
		return INITPTR; /* invalid argument */

	if (section_find (tag) != INITPTR)
		return INITPTR; /* section already exists */

	s = NEW (SettingsSection);
	if (s == NULL)
		return INITPTR;
	
	s->tag_ = tx_create (tag);
	s->parse_func = parse_func;
	s->clean_func = clean_func;

	return s;
}
/* ------------------------------------------------------------------------- */
/** Gets pointer to settings data with tag
 *  @param tag [in] identifier for this settings section
 *  @return pointer to data or INITPTR on error.
 */

void *settings_get (const char *tag)
{
	SettingsSection *s;
	
	s = section_find (tag);

	if (s != INITPTR)
		return s->data_;

	return INITPTR;
}
/* ------------------------------------------------------------------------- */


/* End of file */
