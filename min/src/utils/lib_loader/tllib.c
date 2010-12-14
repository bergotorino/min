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
 *  @file       tllib.c
 *  @version    0.1
 *  @brief      This file contains implementation of min test library loader
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <stdio.h>
#include <string.h>

#include <tllib.h>
#include <min_text.h>
#include <min_common.h>
#include <min_logger.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
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
LOCAL void* tl_load_lib (const char *libname, const char *extension);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/** Loads .so library.
 *  @param libname[in] the name of the library
 *  @param extension[in] extension to try
 *  @return pointer to dll handler, or INITPTR in case of failure.
 *
 *  It loads the library. Extension is added to filename, settings
 *  system is used to fetch library paths.
 */
LOCAL void* tl_load_lib( const char *libname, const char *extension )
{
        void *retval = INITPTR;
        void *shmaddr = INITPTR;
        void *tmp = INITPTR;
        char *path = INITPTR;
        int shmid = 0;
        int plen = 0;
        char *c = INITPTR;
        char *c2 = INITPTR;
        Text *fullpath = INITPTR;
        if( libname == INITPTR ) { return retval; }

        /* If path specified try to open. */
        if(strrchr(libname,'/')!=NULL) {
		fullpath = tx_create (libname);
		if(!strchr(libname,'.')) {tx_c_append(fullpath,
						      extension);}
		
                retval = dlopen(tx_share_buf(fullpath),RTLD_LOCAL|RTLD_LAZY);
                if(retval==NULL) {
                        MIN_NOTICE ("Library [%s] not opened because: %s"
                                   ,libname,dlerror());
                        retval = INITPTR;
                }
		tx_destroy (&fullpath);
                return retval;
        }

        /* Get paths from settings system. */
        shmid = sm_create ('a', sizeof (struct logger_settings_t));
	if (shmid < 0)
		return INITPTR;
        shmaddr = sm_attach (shmid);
        tmp = shmaddr + sizeof (struct logger_settings_t);
        plen = strlen (tmp);
        path = NEW2 (char, plen + 2);
	memset (path, 0x0, plen + 2);
        sm_read (tmp, path, plen);
        sm_detach (shmaddr);
        /* Try each path to load library. */ 
	path [plen] = '\0';
        c = &path[0];
        c2 = c;
        fullpath = tx_create ("");
        do {
                c = strchr (c2,':');
                if( c == NULL) {
                        if (strlen (c2) != 0) { c = &path [plen]; }
                        else { c = INITPTR; break; }
                } else { *c='\0'; }

                if( *(c-1) == '/' ) {
                        tx_c_append(fullpath,c2);
                        tx_c_append(fullpath,libname);
                        if(!strchr(libname,'.')) {tx_c_append(fullpath,
							      extension);}
                } else {
                        tx_c_append(fullpath,c2);
                        tx_c_append(fullpath,"/");
                        tx_c_append(fullpath,libname);
                        if(!strchr(libname,'.')) {tx_c_append(fullpath,
							      extension);}
                }
                c=c+1;
                c2=c;
                retval = dlopen(tx_share_buf(fullpath),RTLD_LOCAL|RTLD_LAZY);
                if(retval == NULL) {
                        MIN_NOTICE ("Library [%s] not opened because: %s"
                                   ,tx_share_buf(fullpath),dlerror());
                        tx_c_copy(fullpath,"");
                        retval = INITPTR;
                } else {
                        MIN_INFO ("Library [%s] opened"
                                   ,tx_share_buf(fullpath));
                        break;
                }
        } while(c!=INITPTR);
        tx_destroy(&fullpath);
        if(retval==INITPTR) {
                MIN_WARN ("There is no valid library [%s] under paths: [%s]"
                           ,libname,path);
        }
        DELETE(path);
        return retval;
}
/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Tries to open test library.
 *  @return 0 when everything ok. 1 on error.
 *  @param *tlibl test library loader entity for one test module
 *  @param *lib_name path to the *.so file to be loaded
 */
int tl_open (test_libl_t *tlibl, const char *lib_name)
{
	void *test_lib;
        void(*init_func)();
        ptr2internals ty,ve;
        ptr2internals2 da,te;
	ptr2case get_fun = (ptr2case)INITPTR;
	ptr2run run_fun = (ptr2run)INITPTR;
	tlibl->get_cases_fun_ = (ptr2case)INITPTR;
	tlibl->run_case_fun_  = (ptr2run)INITPTR;
	tlibl->test_library_ = INITPTR;
        memset(tlibl->type_,'\0',16);
        tlibl->version_ = -1;
        memset(tlibl->date_,'\0',12);
        memset(tlibl->time_,'\0',9);

        test_lib = tl_load_lib(lib_name, ".so");
	if (test_lib == INITPTR)
		test_lib = tl_load_lib(lib_name, ".so.0");
	
	if (test_lib == INITPTR) { return 1; }

	tlibl->test_library_ = test_lib;

        /* Get version and type */
        ty = (ptr2internals)dlsym(tlibl->test_library_,"get_module_type");
        if(ty!=NULL) {
                switch(ty())
                {
                case 0x1:
                        strcpy(tlibl->type_,"Hardcoded");
                        break;
                case 0x2:
                        strcpy(tlibl->type_,"Normal");
                        break;
                case 0x3:
                        strcpy(tlibl->type_,"MINUnit");
                        break;
                case 0x4:
                        strcpy(tlibl->type_,"TestClass");
                        break;
                case 0x5:
                        strcpy(tlibl->type_,"LuaTestClass");
                        break;
                default:
                        strcpy(tlibl->type_,"Unknown");
                }
        } else {
                strcpy(tlibl->type_,"Unknown");
        }
        ve = (ptr2internals)dlsym(tlibl->test_library_,"get_module_version");
        if(ve!=NULL) { tlibl->version_=ve(); }
        da = (ptr2internals2)dlsym(tlibl->test_library_,"get_module_date");
        if(da!=NULL) { STRCPY(tlibl->date_,da(),12); }
        else         { strcpy(tlibl->date_,"Unknown"); }
        te = (ptr2internals2)dlsym(tlibl->test_library_,"get_module_time");
        if(te!=NULL) { STRCPY(tlibl->time_,te(),9); }
        else         { strcpy(tlibl->time_,"Unknown"); }
        MIN_INFO ("Module: %s, Version: %d, Build: %s %s"
                   ,tlibl->type_,tlibl->version_,tlibl->date_,tlibl->time_);


	get_fun = (ptr2case)dlsym (test_lib, "tm_get_test_cases");
	if (get_fun == NULL) {
		MIN_ERROR ("get_fun unresolved in Test Library %s: %s",
			  lib_name, dlerror());
		return 1;
	}

	run_fun = (ptr2run)dlsym (test_lib, "tm_run_test_case");
	if (run_fun == NULL) {
		MIN_ERROR ("run_fun unresolved in Test Library %s: %s",
			  lib_name, dlerror());
		return 1;
	}
	
        init_func = (void(*)())dlsym(test_lib, "tm_initialize");
        if (init_func != NULL) init_func();
	tlibl->get_cases_fun_ = get_fun;
	tlibl->run_case_fun_  = run_fun;

	STRCPY(tlibl->fname_, lib_name, 255);
	
	return 0;
}
/* ------------------------------------------------------------------------- */
/** Tries to open test class.
 *  @param* file [in] name of the test liblary.
 */
void* tl_open_tc( const char* file )
{
        void*           retval = INITPTR;
        ptr2internals ty,ve;
        ptr2internals2 da,te;
        int flen = 0;

        char type[16];
        int version=-1;
        char date[12];
        char time[9];
        memset(type,'\0',16);
        memset(date,'\0',12);
        memset(time,'\0',9);

        /* error checking */
        if( file == INITPTR )   { goto EXIT; }
        flen = strlen(file);
        if( flen == 0 )         { goto EXIT; }

        retval = tl_load_lib(file, ".so");
        if(retval==INITPTR) { retval=tl_load_lib(file, ".so.0"); }
        if(retval==INITPTR) { return retval; }

        /* Get version and type */
        ty = (ptr2internals)dlsym(retval,"get_module_type");
        if(ty!=NULL) {
                switch(ty())
                {
                case 0x1:
                        strcpy(type,"Hardcoded");
                        break;
                case 0x2:
                        strcpy(type,"Normal");
                        break;
                case 0x3:
                        strcpy(type,"MINUnit");
                        break;
                case 0x4:
                        strcpy(type,"TestClass");
                        break;
                case 0x5:
                        strcpy(type,"LuaTestClass");
                        break;
                default:
                        strcpy(type,"Unknown");
                }
        } else {
                strcpy(type,"Unknown");
        }
        ve = (ptr2internals)dlsym(retval,"get_module_version");
        if(ve!=NULL) { version=ve(); }
        da = (ptr2internals2)dlsym(retval,"get_module_date");
        if(da!=NULL) { STRCPY(date,da(),11); }
        else         { strcpy(date,"Unknown"); }
        te = (ptr2internals2)dlsym(retval,"get_module_time");
        if(te!=NULL) { STRCPY(time,te(),8); }
        else         { strcpy(time,"Unknown"); }
        MIN_INFO ("Module: %s, Version: %d, Build: %s %s"
                   ,type,version,date,time);
EXIT:
        return retval;
} 
/* ------------------------------------------------------------------------- */
/** Closes the test library
 *  @return 0 when everything ok. 1 on error.
 *  @param *tlibl test library loader entity for one test module
 */
int tl_close (test_libl_t *tlibl)
{
	if (tlibl == INITPTR || tlibl->test_library_ == INITPTR) {
		MIN_ERROR ("%s: invalid pointer\n", tl_close);
		return 1;
	}
	
	void(*close_func)() = (void(*)())dlsym(tlibl->test_library_
                                              ,"tm_finalize");
        if (close_func != NULL) close_func();
        
        
	if (dlclose (tlibl->test_library_)) {
		MIN_ERROR ("%s: error while closing the test library %s\n",
			  dlerror());
                return 1;
	}
	
        
	tlibl->test_library_  = INITPTR;
	tlibl->get_cases_fun_ = INITPTR;
	tlibl->run_case_fun_  = INITPTR;
	
	return 0;
}
/* ------------------------------------------------------------------------- */
/** Determines whether the test library looks correct.
 *  @return 1 when everything ok. 0 on error.
 *  @param *tlibl test library loader entity for one test module
 */
int tl_is_ok (test_libl_t *tlibl)
{
	if (tlibl == INITPTR) {
		return 0;
	}
	
	if (!tlibl->test_library_ ||
	    tlibl->test_library_ == INITPTR)
		return 0;
	
	if (!tlibl->get_cases_fun_ ||
	    tlibl->get_cases_fun_ == INITPTR)
		return 0;
	
	if (!tlibl->run_case_fun_ ||
	    tlibl->run_case_fun_ == INITPTR)
		return 0;
	
	return 1;
}
/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "tllib.tests"
#endif /* MIN_UNIT_TEST */
/* End of file */
