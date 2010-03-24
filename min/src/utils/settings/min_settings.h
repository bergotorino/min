/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Sampo Saaristo
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
 *  @file       min_settings.h
 *  @version    0.1
 *  @brief      This file contains header file of the Settings implementation.
 */

#ifndef MIN_SETTINGS_H
#define MIN_SETTINGS_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <stdlib.h>
#include <errno.h>
#include <min_common.h>
#include <min_parser.h>

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
/** Basic container for settings sections */
typedef struct {
        /** Name of setting section. (e.g. Logger_Defaults */
        Text           *tag_;

        /** Name of setting section. (e.g. Logger_Defaults */
        Text           *end_tag_;

	/** Function for parsing settings for this section */
	int           (*parse_func)(void *data, MinSectionParser *);

	/** Function for clean up, can be NULL */
	void          (*clean_func)(void *data);

	/** Settings data for this section */
	void          *data_;
} SettingsSection;

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
int              settings_init (void);
/* ------------------------------------------------------------------------- */
int              new_section (const char *tag, const char *endtag,
			      int (*parse_func)(void *data, MinSectionParser *),
			      void (*init_func) (void *data),
			      void (*clean_func)(void *data),
			      int initial_data_size);
/* ------------------------------------------------------------------------- */
int              settings_read (SettingsSection *ss,
				MinParser *mp);
/* ------------------------------------------------------------------------- */
SettingsSection *settings_get_section (const char *tag);
/* ------------------------------------------------------------------------- */
void            *settings_get (const char *tag);
/* ------------------------------------------------------------------------- */
void             settings_destroy_section (const char *tag);
/* ------------------------------------------------------------------------- */
void             settings_destroy (void);
/* ------------------------------------------------------------------------- */

#endif                          /* MIN_SETTINGS_H */
/* End of file */
