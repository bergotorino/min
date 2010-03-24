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
 *  @file       scripter_cli.c
 *  @version    0.1
 *  @brief      This file contains implementation of MIN Scripter CLI
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <stdio.h>
#include <pthread.h>

#include <min_common.h>
#include <dllist.h>
#include <scripter_keyword.h>
#include <scripter_common.h>
#include <min_logger.h>
#include <min_scripter_if.h>
#include <min_lego_interface.h>
#include <min_item_parser.h>
#include <min_test_event_if.h>

#include <scripter_plugin.h>
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
DLList *defines;

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
LOCAL int exit_;
LOCAL int new_command_;
LOCAL char buff [4096];
LOCAL pthread_mutex_t mutex_ = PTHREAD_MUTEX_INITIALIZER;

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
LOCAL void display_available_commands();
/* ------------------------------------------------------------------------- */
LOCAL void list_execute_cmds (DLList *cmds);
/* ------------------------------------------------------------------------- */
LOCAL void serve_cli (void *notused);
/* ------------------------------------------------------------------------- */
LOCAL int  run_cli();
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */
/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/** Displays the commands available for scripter cli
 */
LOCAL void display_available_commands()
{
	printf ("\t ?    - list the available commands\n");
	printf ("\t quit - exit scripter CLI\n"); 
	printf ("\t list - show succesfully executed commands\n"); 

}
/* ------------------------------------------------------------------------- */
/** Displays the sucessfully executed commands in the current session
 *  @param cmds commands saved in list
 */
LOCAL void list_execute_cmds (DLList *cmds)
{
	DLListIterator it;
	Text *tx;
	printf ("\t[Test]\n");
	for (it = dl_list_head (cmds); it != INITPTR; it = dl_list_next (it)){
		tx = dl_list_data (it);
		printf ("\t%s", tx_share_buf (tx));
	}
	printf ("\t[Endtest]\n");
}
/* ------------------------------------------------------------------------- */
/** Thread serving the user io
 */
LOCAL void serve_cli (void *notused)
{
	printf ("Welcome to MIN scripter command line interface\n");
	printf ("give ? for available commands\n");
	while (!exit_) {
		printf ("scripter-cli> ");
		pthread_mutex_lock (&mutex_);
		if (!fgets (buff, 4096, stdin))
			printf ("no input?\n");
		else
			new_command_ = 1;
		pthread_mutex_unlock (&mutex_);
		usleep (100000);
	}
}
/* ------------------------------------------------------------------------- */
/** Scripter CLI main loop
 */
LOCAL int run_cli()
{
	MinItemParser *line;
	char *keyword = INITPTR;
	char *tc_title = "scripter cli";
	Text *tx_desc = tx_create (""), *tx;
        DLList *assoc_cnames = dl_list_create ();
        DLList *assoc_lnames = dl_list_create ();
        DLList *symblist = dl_list_create ();
        DLList *var_list = dl_list_create ();   
        DLList *requested_events = dl_list_create ();
	DLList *slaves = dl_list_create ();

        DLList *class_methods = dl_list_create();
	DLList *interf_objs = dl_list_create();
        DLList *testclasses = dl_list_create ();
	DLList *script_cmds = dl_list_create();
	DLListIterator it;
	void *tmp;
        int     nest_level = 0, ret, lineno = 1;
        char    nesting [255];
	pthread_t  serve_cli_thread;
	TScripterKeyword kw;

	exit_ = 0, new_command_ = 0;
	if (init_scripter_if() != ENOERR) {
		printf ("Failed to initialize scirpter interface\n");
		return 1;
	}
        pthread_create (&serve_cli_thread, NULL, (void *)&serve_cli,
                        &tmp);
	while (1) {
		usleep (100000);
		while (scripter_if_handle_ipc())
			;
		if (!new_command_)
			continue;
		pthread_mutex_lock (&mutex_);
		new_command_ = 0;
		if (strncmp (buff, "quit", 4) == 0) {
			exit_ = 1;
			pthread_mutex_unlock (&mutex_);
			break;
		}
		if (strncmp (buff, "list", 4) == 0) {
			list_execute_cmds (script_cmds);
			pthread_mutex_unlock (&mutex_);
			continue;
		}

		if (strncmp (buff, "?", 1) == 0) {
			display_available_commands();
			pthread_mutex_unlock (&mutex_);
			continue;
		}

		line = mip_create(buff, 0 ,strlen (buff));
		if (keyword != INITPTR) {
			DELETE (keyword);
		}
                mip_get_string (line, "", &keyword);
		if (keyword == INITPTR) {
			pthread_mutex_unlock (&mutex_);
			continue;
		}
		ret = validate_line (keyword, 
				     line, 
				     lineno, 
				     &tc_title, 
				     tx_desc,
				     assoc_lnames, 
				     assoc_cnames,
				     symblist, 
				     var_list,
				     requested_events,
				     slaves, 
				     interf_objs, 
				     nesting,
				     &nest_level);
		if (ret) {
			printf ("syntax error!\n");
			mip_destroy (&line);
		} else {
			lineno++;
			tx = tx_create (buff);
			dl_list_add (script_cmds, tx);
			mip_destroy (&line);
			line = mip_create(buff, 0, strlen (buff));
			DELETE (keyword);
			mip_get_string (line, "", &keyword);
			kw = get_keyword (keyword);
			switch (kw) {
			case EKeywordLoop:
			case EKeywordIf:
				printf ("Nested constructs not supported "
					"in cli mode, sorry.\n");
			case EKeywordEndloop:
			case EKeywordElse:
			case EKeywordEndif:
			case EKeywordBreakloop:
				break;

			default:
				interpreter_handle_keyword (kw,
							    line);
				break;
			}
			mip_destroy (&line);
		}
		pthread_mutex_unlock (&mutex_);
		
	}
	pthread_join (serve_cli_thread, &tmp);
	if (keyword != INITPTR)
		DELETE (keyword);
	for (it = dl_list_head (script_cmds); it != INITPTR; 
	     it = dl_list_next (it)){
		tx = dl_list_data (it);
		tx_destroy (&tx);
	}
	dl_list_free (&script_cmds);
	tx_destroy (&tx_desc);
	do_cleanup (slaves, testclasses, class_methods, assoc_cnames,
		    assoc_lnames, requested_events, symblist,
		    var_list, interf_objs);

	
	return 0;
}
/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
int tm_run_test_case (unsigned int id, const char *cfg_file,
                      TestCaseResult * result)
{
	int ret = run_cli();
	if (ret) {
		ret = TP_CRASHED;
		RESULT (result, ret, "Scripter CLI Internal Errot");
	}
	else {
		scripter_final_verdict (result);
		ret = result->result_;
	}
		
	return ret;
}
/* ------------------------------------------------------------------------- */
int tm_get_test_cases (const char *cfg_file, DLList ** cases)
{
	ENTRYD (*cases, "Scripter CLI", NULL, "Scripter CLI feature");

	return 0;
}
/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */


