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
DLList *settings_list = INITPTR;

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
/* ------------------------------------------------------------------------- */
/** Find the settings section matching the tag.
 *  @param tag [in] identifier for settings section
 *  @return pointer to section or INITPTR on error.
 */
SettingsSection *section_find (const char *tag)
{
	DLListIterator it;
	SettingsSection *s;
	
	if (settings_list == INITPTR)
		settings_list = dl_list_create(); /* FIXME: move to init() */
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
/** Find the list iterator for settings section matching the tag.
 *  @param tag [in] identifier for settings section
 *  @return pointer to list iterator or INITPTR on error.
 */
DLListIterator section_find_it (const char *tag)
{
	DLListIterator it;
	SettingsSection *s;
	
	if (settings_list == INITPTR)
		settings_list = dl_list_create(); /* FIXME: move to init() */
	for (it = dl_list_head (settings_list); 
	     it != INITPTR; 
	     it = dl_list_next(it)) {
		s = dl_list_data (it);

		if (strcmp (tag, tx_share_buf (s->tag_)) == 0)
			return it;
	}
	
	return INITPTR;
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Creates a new settings section entry
 *  @param tag [in] identifier for this settings section
 *  @param endtag [in] end tag (can be INITPTR)
 *  @param parse_func [in] function used for parsing settings for 
 *                         this section
 *  @param clean_func [in] function used for clean up
 *  @return pointer to new section or INITPTR on error.
 */
SettingsSection *new_section (const char *tag, const char *endtag,
			      int (*parse_func)(void *data, MinSectionParser *),
			      void (*clean_func)(void *data),
			      int initial_data_size)
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
	if (endtag != INITPTR)
		s->end_tag_ = tx_create (endtag);
	else {
		s->end_tag_ = tx_create (tag);
		tx_c_prepend (s->end_tag_, "End_");
	}
	s->data_ = NEW2 (char, initial_data_size);
	memset (s->data_, 0x0, initial_data_size);

	s->parse_func = parse_func;
	s->clean_func = clean_func;
	dl_list_add (settings_list, s);

	return s;
}
/* ------------------------------------------------------------------------- */
/** Reads the settings section from given parser entity
 *  @param tag [in] identifier for this settings section
 *  @return 0 on success
 */
int  settings_read (SettingsSection *ss,
		    MinParser *mp)
{
        MinSectionParser *msp = INITPTR;
	int section_number = 1;

	if (mp == NULL || mp == INITPTR)
		return 1;

	do {
		msp = mp_section (mp,
				  tx_share_buf(ss->tag_), 
				  tx_share_buf(ss->end_tag_), 
				  section_number);
		if (msp != INITPTR)
			ss->parse_func (ss->data_, msp);
		section_number ++;
	} while (msp != INITPTR);
	
	return 0;
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
/** Destroys the settings section matching the tag
 *  @param tag [in] identifier for the settings section
 */
void settings_destroy (const char *tag)
{
	DLListIterator it;
	SettingsSection *s;
	
	it = section_find_it (tag);
	if (it != INITPTR) {
		s = dl_list_data (it);
		s->clean_func (s->data_);
		dl_list_remove_it (it);
	}
	return;
}

/* End of file */
