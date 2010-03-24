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
 *  @file       settings_tests_utils.c
 *  @version    0.1
 *  @brief      Parse and clean functions used in settings testing.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
/* ------------------------------------------------------------------------- */
#include "settings_tests_utils.h"
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
/* None */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */
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

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
int test_settings_parse (void *data, MinSectionParser *msp)
{
	test_settings *s = (test_settings *)data;
	MinItemParser    *line = INITPTR; 
	int fooval, ret;
	char *token;

	line = mmp_get_item_line(msp,
				 "Foo",
				 ESTag);
	if (line != INITPTR) {
		ret = mip_get_int (line, "Foo", &fooval);
		if (ret == 0 && fooval == 1) {
			if (s->foo == 0) 
				s->foo = 1; /* Toggle */
			else 
				s->foo = 0;
		} else
			s->foo = 2;
	}
	
	line = mmp_get_item_line(msp,
				 "Bar",
				 ESTag);
	if (line != INITPTR) {
		ret = mip_get_string (line, "Bar", &token);
		dl_list_add (s->bar_list, token);
	}
	
	
	return 0;
}
/* ------------------------------------------------------------------------- */
void test_settings_clean (void *data)
{
	test_settings *s = (test_settings *)data;

	dl_list_free_data (&s->bar_list);
	dl_list_free (&s->bar_list);

	return;
}
/* ------------------------------------------------------------------------- */
void test_settings_init (void *data)
{
	test_settings *s = (test_settings *)data;

	s->bar_list = dl_list_create();


	return;
}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
