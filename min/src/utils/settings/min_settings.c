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
#include <tec.h>
#include <min_common.h>
#include <min_logger_common.h>
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
#ifndef MIN_CONF_DIR
# define MIN_CONF_DIR "/etc" /** Default path for min.conf */
#endif

#ifndef MIN_BIN_DIR
# define MIN_BIN_DIR "/usr/bin" /** Default path for tmc executable */
#endif

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
LOCAL SettingsSection *section_find (const char *tag);
/* ------------------------------------------------------------------------- */
LOCAL DLListIterator section_find_it (const char *tag);
/* ------------------------------------------------------------------------- */
LOCAL int parse_engine_defaults (void *data, MinSectionParser *msp);
/* ------------------------------------------------------------------------- */
LOCAL void clean_engine_defaults (void *data);
/* ------------------------------------------------------------------------- */
LOCAL int  parse_logger_defaults (void *data, MinSectionParser *msp);
/* ------------------------------------------------------------------------- */
LOCAL void init_logger_defaults (void *data);
/* ------------------------------------------------------------------------- */
LOCAL void clean_logger_defaults (void *data);
/* ------------------------------------------------------------------------- */

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
LOCAL SettingsSection *section_find (const char *tag)
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
/** Find the list iterator for settings section matching the tag.
 *  @param tag [in] identifier for settings section
 *  @return pointer to list iterator or INITPTR on error.
 */
LOCAL DLListIterator section_find_it (const char *tag)
{
	DLListIterator it;
	SettingsSection *s;
	
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
/** Parser for "Engine_Defaults" section.
 *  @param data pointer to EngineDefaults
 *  @param msp section section to parse
 */
LOCAL int parse_engine_defaults (void *data, MinSectionParser *msp)
{
        MinItemParser   *mip  = INITPTR;
        char            *search_path = INITPTR;
        char            *path_string = NULL;
        char            *bin_path = INITPTR;
	char            *debugger_cmd = INITPTR;
        char            *locdir = NULL;
        char            *home_d = getenv ("HOME");
	EngineDefaults  *engdef;

	engdef = (EngineDefaults *)(data);

        mip = mmp_get_item_line (msp, "Debugger", ESTag);
        mip_get_string(mip,
                       "Debugger",
                       &debugger_cmd);

        if (debugger_cmd != INITPTR) {
		if (engdef->debugger_ != NULL)
			DELETE (engdef->debugger_);
		engdef->debugger_ = debugger_cmd;
        }
	mip_destroy (&mip);

        mip = mmp_get_item_line (msp,
				 "TmcBinPath",
				 ESTag);
        mip_get_string (mip,
			"TmcBinPath",
			&bin_path);
        if (bin_path == INITPTR) {
                bin_path = NEW2(char, 
                                strlen(MIN_BIN_DIR) + strlen("/tmc") + 1 );
                sprintf (bin_path, "%s/tmc", MIN_BIN_DIR);
        }
        STRCPY (engdef->tmc_app_path_, bin_path, MaxFileName);


        if (mip != INITPTR) mip_destroy(&mip);

        mip = mmp_get_item_line(msp,
                                      "ModSearchPath",
                                      ESTag);

        mip_get_string (mip,
			"ModSearchPath",
			&search_path);

        while (search_path != INITPTR) {
                if (strstr (search_path, "$HOME") == NULL) {
                        path_string = NEW2 (char, strlen (search_path) + 1);
                        sprintf (path_string, "%s", search_path);
                        dl_list_add (engdef->search_dirs,
                                     (void *)path_string);
                } else {
                        if (home_d == NULL) {
                                goto next_item;
                        }
                        locdir = strchr (search_path, '/');
                        if (locdir != NULL) {
                                path_string =
                                        NEW2 (char,
                                              strlen (locdir) 
                                              + strlen (home_d) +
                                              1);
                                sprintf (path_string, "%s%s", home_d, locdir);
                                dl_list_add (engdef->search_dirs,
                                             (void *)path_string);
                        } 
                }
        next_item:
                mip_destroy (&mip);
                mip = mmp_get_next_item_line(msp);
                DELETE(search_path);
                mip_get_string(mip,
                               "ModSearchPath",
                               &search_path);
                
                
        }
	mip_destroy (&mip);
        if (bin_path != INITPTR) {
                DELETE (bin_path);
        }
        mmp_destroy (&msp);
	
	return 0;

}
/* ------------------------------------------------------------------------- */
/** Init function for "Engine_Defaults"
 */
LOCAL void init_engine_defaults (void *data)
{
	EngineDefaults  *engdef;

	engdef = (EngineDefaults *)(data);
	
        engdef->search_dirs = dl_list_create();

	return;
}

/* ------------------------------------------------------------------------- */
/** Clean up function for "Engine_Defaults"
 */
LOCAL void clean_engine_defaults (void *data)
{
	EngineDefaults  *engdef;

	engdef = (EngineDefaults *)(data);
	
        dl_list_free_data (&engdef->search_dirs);
        dl_list_free (&engdef->search_dirs);

	return;
}

/*---------------------------------------------------------------------------*/
/** Parser for "Logger_Defaults" section.
 *  @param data pointer to EngineDefaults
 *  @param msp section section to parse
 */
LOCAL int parse_logger_defaults (void *data, MinSectionParser * msp)
{
        MinItemParser *linebreaker = INITPTR;  /* used if it is necessary 
						  to breakdown line into
						  parts */
        char           *opt_val = NULL; /* used to hold option value if 
                                         * it is single */
        int             fresult = 0;
	struct logger_settings_t *logger_settings;

	logger_settings = (struct logger_settings_t *)data;

        fresult = mmp_get_line (msp,
                                "CreateLogDirectories=", &opt_val, ESNoTag);
        if (fresult == 0) {
		logger_settings->is_defined_.create_log_dir_ = ESTrue;
                if (strcasecmp (opt_val, "YES") == 0) {
                        logger_settings->create_log_dir_ = ESTrue;
                } else if (strcasecmp (opt_val, "NO") == 0) {
                        logger_settings->create_log_dir_ = ESFalse;
                } else {
                        MIN_WARN ("Error in config file "
				  "(CreateLogDirectories) !");
			logger_settings->is_defined_.create_log_dir_ = ESFalse;
                }
                DELETE (opt_val);
        }

        fresult = mmp_get_line (msp,
                                "EmulatorBasePath=", &opt_val, ESNoTag);
        if (fresult == 0) {
                sprintf (logger_settings->emulator_path_, "%s", opt_val);
                logger_settings->is_defined_.path_ = ESTrue;
                DELETE (opt_val);
        }

        linebreaker = mmp_get_item_line (msp,
                                         "EmulatorFormat=", ESNoTag);

        if (linebreaker != INITPTR) {
                fresult = mip_get_string (linebreaker, "", &opt_val);
                while (fresult == 0) {
                        if (strcasecmp (opt_val, "HTML") == 0) {
                                logger_settings->emulator_format_ =
                                    logger_settings->emulator_format_ | ESHtml;
                                logger_settings->is_defined_.format_ = ESTrue;
                        }
                        if (strcasecmp (opt_val, "TXT") == 0) {
                                logger_settings->emulator_format_ =
                                    logger_settings->emulator_format_ | ESTxt;
                                logger_settings->is_defined_.format_ = ESTrue;
                        }
                        if (strcasecmp (opt_val, "DATA") == 0) {
                                logger_settings->emulator_format_ =
                                    logger_settings->emulator_format_ | ESData;
                                logger_settings->is_defined_.format_ = ESTrue;
                        }
                        DELETE (opt_val);
                        fresult = mip_get_next_string (linebreaker, &opt_val);
                }
        }

        mip_destroy (&linebreaker);
        linebreaker = mmp_get_item_line (msp,
                                         "EmulatorOutput=", ESNoTag);

        if (linebreaker != INITPTR) {
                fresult = mip_get_string (linebreaker, "", &opt_val);
                while (fresult == 0) {
                        if (strcasecmp (opt_val, "NULL") == 0) {
                                logger_settings->emulator_output_ =
                                    logger_settings->emulator_output_ | ESNull;
                                logger_settings->is_defined_.output_ = ESTrue;
                        }
                        if (strcasecmp (opt_val, "FILE") == 0) {
                                logger_settings->emulator_output_ =
                                    logger_settings->emulator_output_ | ESFile;
                                logger_settings->is_defined_.output_ = ESTrue;
                        }
			if (strcasecmp (opt_val, "SYSLOG") == 0) {
                                logger_settings->emulator_output_ =
                                    logger_settings->
                                    emulator_output_ | ESSyslog;
                                logger_settings->is_defined_.output_ = ESTrue;
                        }
			if (strcasecmp (opt_val, "STDOUT") == 0) {
                                logger_settings->emulator_output_ =
                                    logger_settings->
                                    emulator_output_ | ESStdout;
                                logger_settings->is_defined_.output_ = ESTrue;
                        }
			if (strcasecmp (opt_val, "STDERR") == 0) {
                                logger_settings->emulator_output_ =
                                    logger_settings->
                                    emulator_output_ | ESStderr;
                                logger_settings->is_defined_.output_ = ESTrue;
                        }

                        DELETE (opt_val);
                        fresult = mip_get_next_string (linebreaker, &opt_val);
                }

        }

        mip_destroy (&linebreaker);

        fresult = mmp_get_line (msp,
                                "ThreadIdToLogFile=", &opt_val, ESNoTag);
        if (fresult == 0) {
		logger_settings->is_defined_.pidid_ = ESTrue;
                if (strcasecmp (opt_val, "YES") == 0) {
                        logger_settings->pidid_ = ESTrue;
                } else if (strcasecmp (opt_val, "NO") == 0) {
                        logger_settings->pidid_ = ESFalse;
                } else {
                        MIN_WARN ("Error in config file (ThreadIdToLogFile) !");
			logger_settings->is_defined_.pidid_ = ESFalse;
                }
                DELETE (opt_val);
        }

        fresult = mmp_get_line (msp,
                                "WithTimeStamp=", &opt_val, ESNoTag);
        if (fresult == 0) {
		logger_settings->is_defined_.time_stamp_ = ESTrue;
                if (strcasecmp (opt_val, "YES") == 0) {
                        logger_settings->time_stamp_ = ESTrue;
                } else if (strcasecmp (opt_val, "NO") == 0) {
                        logger_settings->time_stamp_ = ESFalse;
                } else {
                        MIN_WARN ("Error in config file (WithTimeStamp) !");
			logger_settings->is_defined_.time_stamp_ = ESFalse;
                }
                DELETE (opt_val);
        }

        fresult = mmp_get_line (msp,
                                "WithLineBreak=", &opt_val, ESNoTag);
        if (fresult == 0) {
		logger_settings->is_defined_.line_break_ = ESTrue;
                if (strcasecmp (opt_val, "YES") == 0) {
                        logger_settings->line_break_ = ESTrue;
                } else if (strcasecmp (opt_val, "NO") == 0) {
                        logger_settings->line_break_ = ESFalse;
                } else {
                        MIN_WARN ("Error in config file (WithLineBreak) !");
			logger_settings->is_defined_.line_break_ = ESFalse;
                }

                DELETE (opt_val);
        }

        fresult = mmp_get_line (msp,
                                "WithEventRanking=", &opt_val, ESNoTag);
        if (fresult == 0) {
		logger_settings->is_defined_.event_ranking_ = ESTrue;
                if (strcasecmp (opt_val, "YES") == 0) {
                        logger_settings->event_ranking_ = ESTrue;
                } else if (strcasecmp (opt_val, "NO") == 0) {
                        logger_settings->event_ranking_ = ESFalse;
                } else {
                        MIN_WARN ("Error in config file (WithEventRanking) !");
			logger_settings->is_defined_.event_ranking_ = ESFalse;
                }

                DELETE (opt_val);
        }

        fresult = mmp_get_line (msp,
                                "FileCreationMode=", &opt_val, ESNoTag);
        if (fresult == 0) {
		logger_settings->is_defined_.overwrite_ = ESTrue;
                if (strcasecmp (opt_val, "APPEND") == 0) {
                        logger_settings->overwrite_ = ESFalse;
                } else if (strcasecmp (opt_val, "OVERWRITE") == 0) {
                        logger_settings->overwrite_ = ESTrue;
                } else {
			logger_settings->is_defined_.overwrite_ = ESFalse;
                        MIN_WARN ("Error in config file (FileCreationMode) !");
                }

                DELETE (opt_val);
        }
        
        fresult = mmp_get_line (msp,
                                "LogLevel=", &opt_val, ESNoTag);
        if (fresult == 0) {
                if (strcasecmp (opt_val, "fatal") == 0) {
                        logger_settings->loglevel_ = ESFatal;
                        logger_settings->is_defined_.loglevel_ = ESTrue;
                } else if (strcasecmp (opt_val, "error")==0) {
                        logger_settings->loglevel_ = ESError;
                        logger_settings->is_defined_.loglevel_ = ESTrue;
                } else if (strcasecmp (opt_val, "warn")==0) {
                        logger_settings->loglevel_ = ESWarning;
                        logger_settings->is_defined_.loglevel_ = ESTrue;
                } else if (strcasecmp (opt_val, "info")==0) {
                        logger_settings->loglevel_ = ESInfo;
                        logger_settings->is_defined_.loglevel_ = ESTrue;
                } else if (strcasecmp (opt_val, "notice")==0) {
                        logger_settings->loglevel_ = ESNotice;
                        logger_settings->is_defined_.loglevel_ = ESTrue;
                } else if (strcasecmp (opt_val, "debug")==0) {
                        logger_settings->loglevel_ = ESDebug;
                        logger_settings->is_defined_.loglevel_ = ESTrue;
                } else if (strcasecmp (opt_val, "trace")==0) {
                        logger_settings->loglevel_ = ESTrace;
                        logger_settings->is_defined_.loglevel_ = ESTrue;
                } else {
                        MIN_WARN ("Error in config file (LogLevel) !");
                }
                DELETE (opt_val);
        }
	
	return 0;
}

/*--------------------------------------------------------------------*/
/** Initialize logger_settings structure
 */
LOCAL void init_logger_defaults (void *data)
{
	struct logger_settings_t *logger_settings;

	logger_settings  = (struct logger_settings_t *)data;

        logger_settings->create_log_dir_ = ESFalse;
        logger_settings->unicode_ = ESFalse;
        logger_settings->time_stamp_ = ESFalse;
        logger_settings->pidid_ = ESFalse;
        logger_settings->overwrite_ = ESFalse;
        logger_settings->line_break_ = ESFalse;
        logger_settings->event_ranking_ = ESFalse;
        *logger_settings->emulator_path_ = '\0';
        logger_settings->emulator_output_ = ESNull;
        logger_settings->emulator_format_ = 0;
        logger_settings->is_defined_.create_log_dir_ = ESFalse;
        logger_settings->is_defined_.unicode_ = ESFalse;
        logger_settings->is_defined_.time_stamp_ = ESFalse;
        logger_settings->is_defined_.pidid_ = ESFalse;
        logger_settings->is_defined_.path_ = ESFalse;
        logger_settings->is_defined_.overwrite_ = ESFalse;
        logger_settings->is_defined_.output_ = ESFalse;
        logger_settings->is_defined_.line_break_ = ESFalse;
        logger_settings->is_defined_.hw_path_ = ESFalse;
        logger_settings->is_defined_.hw_output_ = ESFalse;
        logger_settings->is_defined_.hw_format_ = ESFalse;
        logger_settings->is_defined_.format_ = ESFalse;
        logger_settings->is_defined_.event_ranking_ = ESFalse;
}

/* ------------------------------------------------------------------------- */
/** Clean up function for "Logger_Defaults"
 */
LOCAL void clean_logger_defaults (void *data)
{
	struct logger_settings_t *logdef;

	logdef = (struct logger_settings_t *)(data);

	
	return;
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Initialize the settings system.
 */
int settings_init()
{
	int ret = 0;

	if (settings_list == INITPTR)
		settings_list = dl_list_create(); 
	
	ret += new_section ("Engine_Defaults",
			    "End_Defaults",
			    parse_engine_defaults,
			    init_engine_defaults,
			    clean_engine_defaults,
			    sizeof (EngineDefaults));

	ret += new_section ("Logger_Defaults",
			    INITPTR,
			    parse_logger_defaults,
			    init_logger_defaults,
			    clean_logger_defaults,
			    sizeof (struct logger_settings_t));

	return ret;
}
/* ------------------------------------------------------------------------- */
/** Creates a new settings section entry
 *  @param tag [in] identifier for this settings section
 *  @param endtag [in] end tag (can be INITPTR)
 *  @param parse_func [in] function used for parsing settings for 
 *                         this section
 *  @param clean_func [in] function used for clean up
 *  @param initial_data_size [in] size of data 
 *                                needed for saving the section setings.
 *  @return 0 or 1 on error
 */
int new_section (const char *tag, const char *endtag,
		 int (*parse_func)(void *data, MinSectionParser *),
		 void (*init_func)(void *data),
		 void (*clean_func)(void *data),
		 int initial_data_size)
{
	SettingsSection *s;

	if (tag == NULL || tag == INITPTR || parse_func == NULL ||
	    parse_func == INITPTR || clean_func == NULL || 
	    clean_func == INITPTR)
		return 1; /* invalid argument */

	if (section_find (tag) != INITPTR)
		return 1; /* section already exists */

	s = NEW (SettingsSection);
	if (s == NULL)
		return 1;
	
	s->tag_ = tx_create (tag);
	if (endtag != INITPTR)
		s->end_tag_ = tx_create (endtag);
	else {
		s->end_tag_ = tx_create (tag);
		tx_c_prepend (s->end_tag_, "End_");
	}
	s->data_ = NEW2 (char, initial_data_size);
	memset (s->data_, 0x0, initial_data_size);
	if (init_func)
		init_func (s->data_);
	s->parse_func = parse_func;
	s->clean_func = clean_func;

	dl_list_add (settings_list, s);

	return 0;
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
		mmp_destroy (&msp);
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
/** Gets pointer to SettingsSection with tag
 *  @param tag [in] identifier for this settings section
 *  @return pointer to data or INITPTR on error.
 */
SettingsSection *settings_get_section (const char *tag)
{
	
	return section_find (tag);
}

/* ------------------------------------------------------------------------- */
/** Destroys the settings section matching the tag
 *  @param tag [in] identifier for the settings section
 */
void settings_destroy_section (const char *tag)
{
	DLListIterator it;
	SettingsSection *s;
	
	it = section_find_it (tag);
	if (it != INITPTR) {
		s = dl_list_data (it);
		if (s->clean_func != INITPTR)
			s->clean_func (s->data_);
		dl_list_remove_it (it);
	}
	return;
}

/* ------------------------------------------------------------------------- */
/** Destroys the whole settings system
 *  @param tag [in] identifier for the settings section
 */
void settings_destroy (void)
{
	DLListIterator it;
	SettingsSection *s;
	
	
	for (it = dl_list_head(settings_list); it != INITPTR;
	     it = dl_list_next (it)) {
		s = dl_list_data (it);
		if (s->clean_func != INITPTR)
			s->clean_func (s->data_);
		else
			DELETE (s->data_);
	}
	dl_list_free (&settings_list);

	return;
}


/* End of file */
