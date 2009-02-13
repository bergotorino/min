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
 *  @file       min_main.c
 *  @version    0.1
 *  @brief      This file contains implementation of MIN main.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <getopt.h>

#include <min_common.h>
#include <consoleui.h>
#include <tec.h>
#include <data_api.h>
#include <ext_interface.h>
#include <dllist.h>
#include <tllib.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
extern char* optarg;
/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */

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
/** Displays the MIN release tag */
LOCAL void      display_version ();
/* ------------------------------------------------------------------------- */
/** Displays short help */
LOCAL void      display_help ();
/* ------------------------------------------------------------------------- */
/** Appends the pwd to a relative path 
 * @param p the path coming from commandline
 * @return the absolute path
 */
LOCAL char     *patch_path (char *p);
/* ------------------------------------------------------------------------- */
/** Displays information of test module library 
 *  @param libname the test module name coming from commandline
 */
LOCAL void test_module_info(char *libname);
/* ------------------------------------------------------------------------- */
/** Adds the modules and configuration files given from commandline
 *  @param modulelist list of module and filenames 
 *  @return 0 - on success, 1 - on error (e.g. file not found 
 */
LOCAL int add_command_line_modules (DLList * modulelist);

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
LOCAL void display_license ()
{
        printf ("MIN Test Framework, © Nokia 2008, All rights reserved,\n"
                "licensed under the Gnu General Public License version 2,\n"
                "Contact: Antti Heimola, DG.MIN-Support@nokia.com\n\n");

}

LOCAL void display_version ()
{

        printf ("MIN Test Framework release %s\n", MIN_VERSION_STR);
}

LOCAL void display_help ()
{
        printf ("\nUsage: min [options]\n");
        printf ("Options:\n");
	printf (" -h,  --help\t\t\tDisplay this information and exits\n");
        printf (" -v,  --version\t\t\t"
                "Display version information and exits\n");
        printf (" -c,  --console\t\t\tStart MIN without consoleui\n");
        printf (" -i,  --info test_module\t"
                "Print details about test_module and exits\n");
        printf (" -x,  --execute test_module"
                "[:configuration_file:... ]\n\t\t\t"
                "\tExecute test cases from test_module "
                "(with configuration file(s))\n");
        printf ("\nReport bugs to:\n");
        printf ("DG.MIN-Support@nokia.com\n");
}

LOCAL char *patch_path (char *p)
{
        Text *path, *cwd;
        char *retval;

        path = tx_create (p);
        if (*p != '/') { /* relative path have to prepend with pwd */
                cwd = tx_create (getenv ("PWD"));
                tx_c_append (cwd, "/");
                tx_append (cwd, path);
                retval = tx_get_buf (cwd);
                tx_destroy (&cwd);
        } else
                retval = tx_get_buf (path);
        tx_destroy (&path);

        return retval;
}

LOCAL void test_module_info(char *libname)
{
        test_libl_t tlibl;
        int ret;
        static int first = 1;
        char *fullpath;

        fullpath = patch_path (libname);

        ret = tl_open (&tlibl, fullpath);

        if( (ret != ENOERR) && (tlibl.test_library_==INITPTR) ) {
                printf("Error when looking for test module %s\n",libname);
                DELETE (fullpath);
                return;
        }
        if (first) {
                printf ("Module Type:\tModule Version:\t\tBuild date:\n");
                first = 0;
        }
        printf ("%s\t%d\t\t\t%s %s\n", 
                tlibl.type_, 
                tlibl.version_, 
                tlibl.date_, 
                tlibl.time_);

        tl_close (&tlibl);
        DELETE (fullpath);

        return;
}

LOCAL int add_command_line_modules (DLList * modulelist)
{

        char *path, *command, *p = NULL, *conffname;
        DLList *configs_list;
        DLListIterator it;
        FILE *f;


        for (it = dl_list_head (modulelist); it != DLListNULLIterator;
             it = dl_list_next (it)) {
                configs_list = dl_list_create();

                command = (char *)dl_list_data (it);
                while ((p = strrchr (command, ':'))) {
                        *p = '\0';
                        p ++;
                        conffname = patch_path (p);
                        if (!(f = fopen (conffname, "r"))) {
                                fprintf (stderr, "\n*** Could not "
                                         "open %s: %s ***\n",  
                                         conffname, strerror (errno));
                                return 1;
                        }
                        fclose (f);
                        dl_list_add (configs_list, conffname);
                }

                path = patch_path (command);
                if (!(f = fopen (path, "r"))) {
                        fprintf (stderr, "\n*** Could not open %s: %s ***\n",  
                                 path, strerror (errno));
                        DELETE (path);
                        return 1;
                }
                fclose (f);
                ec_add_module (path, configs_list, 0);
                DELETE (path);
        }

        return 0;
}


/* ------------------------------------------------------------------------- */
int main (int argc, char *argv[], char *envp[])
{
        int             cont_flag, status, c, oper_mode, exit_flag;
        int             no_cui_flag, help_flag, version_flag, retval;
        DLList         *modulelist;
        DLListIterator  work_module_item;
	struct option min_options[] =
		{
			{"no-cui", no_argument, &no_cui_flag, 1},
			{"console", no_argument, &no_cui_flag, 1},
			{"help", no_argument, &help_flag, 1},
			{"version", no_argument, &version_flag, 1},
			{"info", required_argument, NULL, 'i'},
                        {"execute", required_argument, NULL, 'x'},
		};

        modulelist = dl_list_create();
	work_module_item = DLListNULLIterator;
	oper_mode = no_cui_flag = help_flag = version_flag = cont_flag = 0;
        retval = exit_flag = 0;
        
        display_license();
        usleep (500000);

	while (1) {
		/* getopt_long stores the option index here. */
		int option_index = 0;
     
		c = getopt_long (argc, argv, "nchvi:x:",
				 min_options, &option_index);
     
		/* Detect the end of the options. */
		if (c == -1)
			break;
     
		switch (c)
		{
		case 0:
			break;
	       
		case 'n':
		case 'c':
			no_cui_flag = 1;
			break;
			
		case 'v':
			version_flag = 1;
			break;
     
		case 'h':
			help_flag = 1;
			break;

                case 'x':
                        oper_mode = 1;
                        dl_list_add (modulelist, optarg);
                        break;

		case '?':
			help_flag = 1;
			break;
                case 'i':
                        test_module_info (optarg);
                        exit_flag = 1;
                        break;
		default:
			abort ();
             }
        }
     
	if (optind < argc) {
		printf ("unknown options: ");
		while (optind < argc)
			printf ("\t%s", argv[optind++]);
		display_help();
		exit (0);
	}
     
	if (version_flag && help_flag) {
		display_version();
		display_help();
		exit (0);
	}
	if (version_flag) {
		display_version();
		exit (0);
	}

	if (help_flag) {
		display_help();
		exit (0);
	}

        if (exit_flag) {
                exit (0);
        }

        ec_min_init (NULL, NULL, NULL, envp, oper_mode);

        if (add_command_line_modules (modulelist))
                exit (1);
        
        while (cont_flag == 0) {
                cont_flag = 1;
                usleep (500000);
                work_module_item = dl_list_head (instantiated_modules);
                while (work_module_item != DLListNULLIterator) {
                        status = tm_get_status (work_module_item);
                        if (status != TEST_MODULE_READY)
                                cont_flag = 0;
                        work_module_item = dl_list_next (work_module_item);
                }
                printf ("Test case gathering...%d \n", cont_flag);
        }

        if (no_cui_flag) {
                ext_if_exec ();
                retval = log_summary_stdout ();
        } else
                cui_exec ();

        dl_list_free (&modulelist);
        ec_cleanup ();
        return retval;
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* None */

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
