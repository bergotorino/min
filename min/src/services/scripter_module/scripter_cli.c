#include <stdio.h>

#include <min_common.h>
#include <dllist.h>
#include <scripter_keyword.h>
#include <scripter_common.h>
#include <min_logger.h>
#include <min_scripter_if.h>
#include <min_lego_interface.h>
#include <min_item_parser.h>
#include <scripter_plugin.h>

DLList *defines;

LOCAL void display_available_commands()
{
	printf ("\t ?    - list the available commands\n");
	printf ("\t quit - exit scripter CLI and return to MIN\n"); 
	printf ("\t list - show succefully executed commands\n"); 

}

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

LOCAL int run_cli()
{
	char buff [4096];
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

        int     nest_level = 0, ret, lineno = 1;
        char    nesting [255];
	printf ("Wellcome to MIN scripter command line interface\n");
	printf ("give ? for available commands\n");
	while (1) {
		printf ("scrpiter-cli>");
		fgets (buff, 4096, stdin);
		if (strncmp (buff, "quit", 4) == 0)
			break;

		if (strncmp (buff, "list", 4) == 0) {
			list_execute_cmds (script_cmds);
			continue;
		}

		if (strncmp (buff, "?", 1) == 0) {
			display_available_commands();
			continue;
		}

		line = mip_create(buff, 0 ,strlen (buff));
		if (keyword != INITPTR)
			DELETE (keyword);
                mip_get_string (line, "", &keyword);
		if (keyword == INITPTR)
			continue;
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
		}
		/*
		 * FIXME do cleanup
		 */
	}
	return 0;
}

int tm_run_test_case (unsigned int id, const char *cfg_file,
                      TestCaseResult * result)
{
	int ret = run_cli();
	RESULT (result, ret, "Scripter CLI Finnished");
	return ret;
}

int tm_get_test_cases (const char *cfg_file, DLList ** cases)
{
	ENTRYD (*cases, "Scripter CLI", NULL, "Scripter CLI feature");

}


