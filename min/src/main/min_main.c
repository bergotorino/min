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
#include <cli.h>
#include <consoleui.h>
#include <cli.h>
#include <tcpip_plugin.h>
#include <tec.h>
#include <tec_tcp_handling.h>
#include <data_api.h>
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
extern int min_return_value;

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
eapiIn_t *in;
eapiOut_t *out;
int slave_exit;
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
/* ------------------------------------------------------------------------- */
LOCAL void      display_version ();
/* ------------------------------------------------------------------------- */
LOCAL void      display_license ();
/* ------------------------------------------------------------------------- */
LOCAL void      display_help ();
/* ------------------------------------------------------------------------- */
LOCAL char     *patch_path (char *p);
/* ------------------------------------------------------------------------- */
LOCAL int       add_command_line_modules (DLList * modulelist);
/* ------------------------------------------------------------------------- */
LOCAL int       add_ip_slaves (DLList * slavelist);
/* ------------------------------------------------------------------------- */
LOCAL pthread_t load_plugin (const char *plugin_name, void *plugin_conf);
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/** Displays license and contact information
 */
LOCAL void display_license ()
{
        printf ("MIN Test Framework, © Nokia 2008, All rights reserved,\n"
                "licensed under the Gnu General Public License version 2,\n"
                "Contact: Pekka Nuotio, DG.MIN-Support@nokia.com\n\n");

}
/* ------------------------------------------------------------------------- */
/** Displays the MIN release tag 
 */
LOCAL void display_version ()
{

        printf ("MIN Test Framework release %s\n", MIN_VERSION_STR);
}
/* ------------------------------------------------------------------------- */
/** Displays short help 
 */
LOCAL void display_help ()
{
        printf ("\nUsage: min [options]\n");
        printf ("Options:\n");
	printf (" -h,  --help\t\t\tDisplay this information and exits\n");
        printf (" -v,  --version\t\t\t"
                "Display version information and exits\n");
        printf (" -c,  --console\t\t\tStart MIN without consoleui\n");
	printf (" -d,  --debug\t\t\tAttach a debugger to test case(s)\n");
	printf ("\t\t\t\tNote: implies -c\n");
        printf (" -i,  --info test_module\t"
                "Print details about test_module and exits\n");
        printf (" -x,  --execute test_module"
                "[:configuration_file:... ]\n\t\t\t"
                "\tExecute test cases from test_module"
                "\n\t\t\t\t(with configuration file(s))\n");
        printf (" -t,  --title title"
                "\t\tShow/execute test cases with title\n");
        printf (" -r,  --title-regex regular_experession"
                "\n\t\t\t\tShow/execute test cases with title matching"
		"\n\t\t\t\tregular_expression\n");
	printf (" -s,  --slave host[:slave_type]"
                "\n\t\t\t\tRegister host (with type slave_type) for"
		"\n\t\t\t\tmaster/slave testing\n");
        printf (" -p,  --plugin plugin"
                "[:plugin2:... ]\n\t\t\t"
                "\tLoad input plugin for MIN,\n\t\t\t\tby default cli or cui "
		"plugin is loaded\n");
        printf ("\nReport bugs to:\n");
        printf ("DG.MIN-Support@nokia.com\n");
}
/* ------------------------------------------------------------------------- */
/** Appends the pwd to a relative path 
 * @param p the path coming from commandline
 * @return the absolute path
 */
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
/* ------------------------------------------------------------------------- */
/** Adds the modules and configuration files given from commandline
 *  @param modulelist list of module and filenames 
 *  @return 0 - on success, 1 - on error (e.g. file not found)
 */
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
                ec_add_module (path, configs_list, 0, 1);
                DELETE (path);
        }

        return 0;
}
/* ------------------------------------------------------------------------- */
/** Adds the IP slaves given from commandline through eapi to slave pool
 *  @param slavelist list of hostnames or ip addresses
 *  @return 0 - on success, 1 - on error 
 */
LOCAL int add_ip_slaves (DLList * slavelist)
{
 
	char *command, *p = NULL;
        DLListIterator it;


        for (it = dl_list_head (slavelist); it != DLListNULLIterator;
             it = dl_list_next (it)) {
                command = (char *)dl_list_data (it);
                if ((p = strrchr (command, ':'))) {
                        *p = '\0';
                        p++;
               }
               if (out_str.register_slave (command, p ? p : "phone")) {
                       fprintf (stderr, "slave \"%s\" registration failed!\n",
                                command);
                       return 1;
               }
        }

        return 0;
}
/* ------------------------------------------------------------------------- */
/** Loads the plugin with given name and starts executing it in a thread
 *  @param plugin_name short name for the plugin e.g. "cui" loads "min_cui.so" 
 *  @param plugin_conf passed as is to the plugin
 *  @return handler for the created thread
 */
LOCAL pthread_t load_plugin (const char *plugin_name, void *plugin_conf)
{
        void *pluginhandle = INITPTR;
        void (*pl_attach) (eapiIn_t **out_callback, 
		           eapiOut_t *in_callback);
        void (*pl_open) (void *arg);
        pthread_t plugin_thread;
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
                                 plugin_conf);

        /* In this fancy way we do display legal and contact information on
         * consoleUI. */
        if (!strcmp (plugin_name, "cui") &&  in->error_report) {
                usleep (100000);
                in->error_report ("Contact: Pekka Nuotio, "
                                  "DG.MIN-Support@nokia.com");
                in->error_report ("licensed under the Gnu General "
                                  "Public License version 2,");
                in->error_report ("MIN Test Framework, (c) Nokia 2008,"
                                  " All rights reserved,");
        }

        return plugin_thread;
}
/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** main() for MIN - handle command line swithces and load plugins
 *  @param argc argument count
 *  @param argv arguments
 *  @param evnp environment
 *  @return number of failed (aborted,crashed) cases or -1 in case  of 
 *   serious error
 */
int main (int argc, char *argv[], char *envp[])
{
        int             cont_flag, opt_char, oper_mode, exit_flag;
        int             cli_flag, ip_flag, help_flag, version_flag, info_flag;
	int 	        slave_mode = 0, master_socket = -1;
        DLList         *modulelist, *slavelist;
        DLListIterator  work_module_item;
        pthread_t       plugin_thread[10];
	void           *plugin_opts = NULL;
	cli_opts        cli_opts;
	tcpip_opts      tcpip_opts;
        void *tmp;
        char *c2 = INITPTR;
        char *c3 = INITPTR;
        unsigned int num_of_plugins = 0;
        Text *plugin = tx_create("cui");
	struct option min_options[] =
		{
			{"help", no_argument, &help_flag, 1},
			{"version", no_argument, &version_flag, 1},
			{"console", no_argument, &cli_flag, 1},
			{"info", required_argument, NULL, 'i'},
                        {"execute", required_argument, NULL, 'x'},
			{"slave", required_argument, NULL, 's'},
                        {"plugin", required_argument, NULL,'p'},
                        {"title", required_argument, NULL, 't'},
			{"title-regex", required_argument, NULL, 'r'}, 
                        {"masterfd", required_argument, NULL,'m'}
		};

	memset (&cli_opts, 0x0, sizeof (cli_opts));

        modulelist = dl_list_create();
	slavelist = dl_list_create();
	work_module_item = DLListNULLIterator;
	oper_mode = cli_flag = help_flag = version_flag = cont_flag = 0;
        info_flag = exit_flag = ip_flag = 0;
        
        /* Detect commandline arguments */
	while (1) {
		/* getopt_long stores the option index here. */
		int option_index = 0;
     
		opt_char = getopt_long (argc, argv, "hvcdi:x:s:p:t:r:m:f:",
					min_options, &option_index);
     
		/* Detect the end of the options. */
		if (opt_char == -1)
			break;
     
		switch (opt_char)
		{
		case 0:
			break;
	       
		case 'c':
			cli_flag = 1;
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
                        oper_mode = 1;
                        dl_list_add (modulelist, optarg);
			cli_flag = 1;
			cli_opts.display_info_ = 1;
                        break;
                case 'd':
			cli_flag = 1;
			cli_opts.debug_ = 1;
                        break;

                case 'p':
                        tx_c_copy (plugin,optarg);
                        break;

		case 's':
			dl_list_add (slavelist, optarg);
			break;

		case 'm':
			slave_mode = 1;
			cli_flag = 1;
			oper_mode = 0;
			master_socket = atoi (optarg);
			break;
		case 'f':
			ip_flag = 1;
			tcpip_opts.fd_ = atoi (optarg);
			break;
		case 't':
			ec_add_title_filter (optarg, 0);
			break;

		case 'r':
			ec_add_title_filter (optarg, 1);
			break;
		default:
			abort ();
             }
        }

        /* Handle options. */
	if (cli_flag) {
		display_license();
		plugin_opts = (void *)&cli_opts;
		tx_c_copy (plugin, "cli");
	} 

	if (ip_flag) {
		plugin_opts = (void *)&tcpip_opts;
		tx_c_copy (plugin, "tcpip");
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

	if (slave_mode) {
                in = &in_str;
                out = &out_str;
                eapi_init (in, out);
		ec_configure();
		new_tcp_master (master_socket);
		while (slave_exit == 0)
			usleep (500000);
		ec_cleanup();
		return 0;

	}
	/**
	 *  Load plugin and go
	 */
	c2 = tx_get_buf(plugin);
	do {
		c3 = strchr (c2,':');
		if (c3!=NULL) (*c3) = '\0';
		plugin_thread[0] = load_plugin(c2, plugin_opts);
		num_of_plugins++;
                        /* Multiple plugins not supported yet by engine.
                         * FIXME: remove in future following line. */
		break;
		/* if (c3==NULL) break;
		   c2 = c3+1;*/
	} while (c3!=NULL);
	tx_destroy (&plugin);
	if (add_command_line_modules (modulelist) ||
	    add_ip_slaves (slavelist)) {
		exit (-1);
        }
	
	pthread_join (plugin_thread[0], &tmp);

	
	dl_list_free (&modulelist);
	dl_list_free (&slavelist);
	ec_cleanup ();

	return min_return_value;
}

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
