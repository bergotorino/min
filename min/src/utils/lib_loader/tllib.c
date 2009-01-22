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
/** Loads .so library.
 *  @param libname[in] the name of the library
 *  @return pointer to dll handler, or INITPTR in case of failure.
 *
 *  It loads the library. .so extension is added automatically, settings
 *  system is used to fetch lobrary paths.
 */
LOCAL void* tl_load_lib( const char *libname );
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
LOCAL void* tl_load_lib( const char *libname )
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
                retval = dlopen(libname,RTLD_LOCAL|RTLD_LAZY);
                if(retval==NULL) {
                        MIN_NOTICE ("Library [%s] not opened because: %s"
                                   ,libname,dlerror());
                        retval = INITPTR;
                }
                return retval;
        }

        /* Get paths from settings system. */
        shmid = sm_create('a',sizeof(struct logger_settings_t));
        shmaddr = sm_attach(shmid);
        tmp = shmaddr+sizeof(struct logger_settings_t);
        plen = strlen(tmp);
        path = NEW2(char,plen+1);
        sm_read( tmp, path, plen+1);
        sm_detach(shmaddr);
        /* Try each path to load library. */ 
        c = &path[0];
        c2 = c;
        fullpath = tx_create("");
        do {
                c = strchr(c2,':');
                if(c==NULL) {
                        if(strlen(c2)!=0) { c = &path[plen]; }
                        else { c = INITPTR; break; }
                } else { *c='\0'; }
                if( *(c-1) == '/' ) {
                        tx_c_append(fullpath,c2);
                        tx_c_append(fullpath,libname);
                        if(!strchr(libname,'.')) {tx_c_append(fullpath,".so");}
                } else {
                        tx_c_append(fullpath,c2);
                        tx_c_append(fullpath,"/");
                        tx_c_append(fullpath,libname);
                        if(!strchr(libname,'.')) {tx_c_append(fullpath,".so");}
                }
                c=c+1;
                c2=c;
                retval = dlopen(tx_share_buf(fullpath),RTLD_LOCAL|RTLD_LAZY);
                if(retval==NULL) {
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
        return retval;
}
/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
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

        test_lib = tl_load_lib(lib_name);
	if (test_lib == INITPTR) { return 1; }

	tlibl->test_library_ = test_lib;

        /* Get version and type */
        ty = (ptr2internals)dlsym(tlibl->test_library_,"get_module_type");
        if(ty!=NULL) {
                switch(ty())
                {
                case 0x1:
                        STRCPY(tlibl->type_,"Hardcoded",9);
                        break;
                case 0x2:
                        STRCPY(tlibl->type_,"Normal",6);
                        break;
                case 0x3:
                        STRCPY(tlibl->type_,"MINUnit",8);
                        break;
                case 0x4:
                        STRCPY(tlibl->type_,"TestClass",9);
                        break;
                case 0x5:
                        STRCPY(tlibl->type_,"LuaTestClass",12);
                        break;
                default:
                        STRCPY(tlibl->type_,"Unknown",7);
                }
        } else {
                STRCPY(tlibl->type_,"Unknown",7);
        }
        ve = (ptr2internals)dlsym(tlibl->test_library_,"get_module_version");
        if(ve!=NULL) { tlibl->version_=ve(); }
        da = (ptr2internals2)dlsym(tlibl->test_library_,"get_module_date");
        if(da!=NULL) { STRCPY(tlibl->date_,da(),11); }
        else         { STRCPY(tlibl->date_,"Unknown",7); }
        te = (ptr2internals2)dlsym(tlibl->test_library_,"get_module_time");
        if(te!=NULL) { STRCPY(tlibl->time_,te(),8); }
        else         { STRCPY(tlibl->time_,"Unknown",7); }
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

        retval = tl_load_lib(file);
        if(retval==INITPTR) { return retval; }

        /* Get version and type */
        ty = (ptr2internals)dlsym(retval,"get_module_type");
        if(ty!=NULL) {
                switch(ty())
                {
                case 0x1:
                        STRCPY(type,"Hardcoded",9);
                        break;
                case 0x2:
                        STRCPY(type,"Normal",6);
                        break;
                case 0x3:
                        STRCPY(type,"MINUnit",8);
                        break;
                case 0x4:
                        STRCPY(type,"TestClass",9);
                        break;
                case 0x5:
                        STRCPY(type,"LuaTestClass",12);
                        break;
                default:
                        STRCPY(type,"Unknown",7);
                }
        } else {
                STRCPY(type,"Unknown",7);
        }
        ve = (ptr2internals)dlsym(retval,"get_module_version");
        if(ve!=NULL) { version=ve(); }
        da = (ptr2internals2)dlsym(retval,"get_module_date");
        if(da!=NULL) { STRCPY(date,da(),11); }
        else         { STRCPY(date,"Unknown",7); }
        te = (ptr2internals2)dlsym(retval,"get_module_time");
        if(te!=NULL) { STRCPY(time,te(),8); }
        else         { STRCPY(time,"Unknown",7); }
        MIN_INFO ("Module: %s, Version: %d, Build: %s %s"
                   ,type,version,date,time);
EXIT:
        return retval;
} 
/* ------------------------------------------------------------------------- */
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
