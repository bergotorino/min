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
#include <min_engine_api.h>
#include <dllist.h>
#include <tllib.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
extern char* optarg;
extern eapiIn_t in_str;

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
eapiIn_t *in;
eapiOut_t *out;

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
eapiOut_t out_str;


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
        printf (" -p,  --plugin plugin"
                "[:plugin2:... ]\n\t\t\t"
                "\tLoad input plugin for MIN, by default cui plugin "
                "is loaded\n");
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

LOCAL pthread_t load_plugin (const char *plugin_name)
{
        void *pluginhandle = INITPTR;
        void (*pl_attach) (eapiIn_t **out_callback, 
		           eapiOut_t *in_callback);
        void (*pl_open) (void *arg);
        pthread_t plugin_thread;
        void *tmp = INITPTR;
        int retval = 0;

        /* Construct plugin name */
        Text *plugin = tx_create("/usr/lib/min/min_");
        tx_c_append(plugin,plugin_name);
        tx_c_append(plugin,".so");

        /* Load plugin and resolve functions from Plugin API */
        pluginhandle = dlopen(tx_share_buf(plugin),RTLD_NOW);
        if (!pluginhandle) {
                printf ("Error opening plugin %s\n", dlerror());
                exit (-1);
        }
        pl_attach = dlsym (pluginhandle, "pl_attach_plugin");
        pl_open = dlsym (pluginhandle, "pl_open_plugin");
        if (!pl_attach || !pl_open) {
                printf ("Error opening plugin %s\n", dlerror());
                exit (-1);
        }

        /* Initialize Eapi/Attach the plugin */
        in  = &in_str;
        out = &out_str;
        pl_attach (&in, out);
        eapi_init (in, out);
        pl_attach (&in, out);

        /* Run the plugin */
        retval = pthread_create (&plugin_thread, NULL, (void *)pl_open,
                                 &tmp);

        /* In this fancy way we do display legal and contact information on
         * consoleUI. */
        if (in->error_report) {
                usleep (100000);
                in->error_report ("Contact: Antti Heimola, "
                                  "DG.MIN-Support@nokia.com");
                in->error_report ("licensed under the Gnu General "
                                  "Public License version 2,");
                in->error_report ("MIN Test Framework, (c) Nokia 2008,"
                                  " All rights reserved,");
        }

        return plugin_thread;
}

/* ------------------------------------------------------------------------- */
int main (int argc, char *argv[], char *envp[])
{
        int             cont_flag, status, c, oper_mode, exit_flag;
        int             no_cui_flag,
                        help_flag,
                        version_flag,
                        retval;
        DLList         *modulelist;
        DLListIterator  work_module_item;
        pthread_t       plugin_thread[10];
        void *tmp;
        char *c2 = INITPTR;
        char *c3 = INITPTR;
        unsigned int num_of_plugins = 0;
        Text *plugin = tx_create("cui");
/*        FILE *fp = INITPTR;
        int ii=0;
*/

        /* Detect if DBus plugin has been installed */
        /* To be included in the future when engine can
         * hande multiple plugins.
         * INFO: removed due to problems with exiting min
        fp = fopen("/usr/lib/min/min_dbus.so","r");
        if (fp) {
                tx_c_prepend (plugin,"dbus:");
                fclose (fp);
        }*/

	struct option min_options[] =
		{
			{"no-cui", no_argument, &no_cui_flag, 1},
			{"console", no_argument, &no_cui_flag, 1},
			{"help", no_argument, &help_flag, 1},
			{"version", no_argument, &version_flag, 1},
			{"info", required_argument, NULL, 'i'},
                        {"execute", required_argument, NULL, 'x'},
                        {"plugin",required_argument, NULL,'p'},
		};

        modulelist = dl_list_create();
	work_module_item = DLListNULLIterator;
	oper_mode = no_cui_flag = help_flag = version_flag = cont_flag = 0;
        retval = exit_flag = 0;
        
        /* Detect commandline arguments */
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
                case 'p':
                        tx_c_copy (plugin,optarg);
                        break;
		default:
			abort ();
             }
        }

        /* Handle options. */
	if (!no_cui_flag) {
		display_license();
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

        /* Perform application start-up */
        ec_min_init (envp, oper_mode);

	if (no_cui_flag) {
		in = &in_str;

		ec_start_modules();
		if ( add_command_line_modules (modulelist))
			exit (1);
		while (cont_flag == 0) {
			cont_flag = 1;
			usleep (500000);
			work_module_item = dl_list_head (instantiated_modules);
			while (work_module_item != DLListNULLIterator) {
				status = tm_get_status (work_module_item);
				if (status != TEST_MODULE_READY)
					cont_flag = 0;
				work_module_item = 
					dl_list_next (work_module_item);
			}
		}
		retval = ext_if_exec ();
	} else {

                c2 = tx_get_buf(plugin);
                do {
                        c3 = strchr (c2,':');
                        if (c3!=NULL) (*c3) = '\0';
                        plugin_thread[num_of_plugins] = load_plugin(c2);
                        num_of_plugins++;
                        /* Multiple plugins not supported yet by engine.
                         * FIXME: remove in future following line. */
                        if (num_of_plugins>0) break;
                        if (c3==NULL) break;
                        c2 = c3+1;
                } while (c3!=NULL);
                tx_destroy (&plugin);

		if (add_command_line_modules (modulelist))
			exit (1);

	        pthread_join (plugin_thread[0], &tmp);
        }

        dl_list_free (&modulelist);

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
