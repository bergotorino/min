/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Konrad Marek Zapalowicz
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
 *  @file       lua_scripter_if.c
 *  @version    0.1
 *  @brief      This file contains implementation of LUA MIN Scripter.
 */

/* -------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <lua_scripter_if.h>

/* -------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* -------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* None */

/* -------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* -------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
/* None */

/* -------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* -------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* -------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* -------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* -------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* -------------------------------------------------------------------------- */
LOCAL int _look4event (const void *a, const void *b);
/* -------------------------------------------------------------------------- */
LOCAL int ls_load_testmodule (lua_State *l);
/* -------------------------------------------------------------------------- */
LOCAL int ls_release_testmodule (lua_State *l);
/* -------------------------------------------------------------------------- */
LOCAL int ls_register_testcase (lua_State *l);
/* -------------------------------------------------------------------------- */
LOCAL int ls_print (lua_State *l);
/* -------------------------------------------------------------------------- */
LOCAL int ls_request (lua_State *l);
/* -------------------------------------------------------------------------- */
LOCAL int ls_release (lua_State *l);
/* -------------------------------------------------------------------------- */
LOCAL int ls_set (lua_State *l);
/* -------------------------------------------------------------------------- */
LOCAL int ls_unset (lua_State *l);
/* -------------------------------------------------------------------------- */
LOCAL int ls_wait (lua_State *l);
/* -------------------------------------------------------------------------- */
LOCAL int ls_sleep (lua_State *l);
/* -------------------------------------------------------------------------- */
LOCAL int ls_run (lua_State *l);
/* -------------------------------------------------------------------------- */
LOCAL int ls_allocate_slave (lua_State *l);
/* -------------------------------------------------------------------------- */
LOCAL int ls_free_slave (lua_State *l);
/* -------------------------------------------------------------------------- */
LOCAL int ls_slave_request (lua_State *l);
/* -------------------------------------------------------------------------- */
LOCAL int ls_slave_release (lua_State *l);
/* -------------------------------------------------------------------------- */
LOCAL int ls_slave_run (lua_State *l);
/* -------------------------------------------------------------------------- */
LOCAL int ls_slave_expect (lua_State *l);
/* -------------------------------------------------------------------------- */
LOCAL int ls_slave_sendrecv (lua_State *l);
/* -------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
/* -------------------------------------------------------------------------- */
DLList *events = INITPTR;       /**< List of requested events. */
DLList *ls_tc_list = INITPTR;   /**< List of test cases from scriptfile. */
/* -------------------------------------------------------------------------- */
/** Table of methods that will be exported to the Lua in min namespace. */
LOCAL const luaL_reg min_methods[] = {
        {"load",                ls_load_testmodule},
        {"unload",              ls_release_testmodule},
        {"register_testcase",   ls_register_testcase},
        {"print",               ls_print},
        {"request",             ls_request},
        {"release",             ls_release},
        {"set",                 ls_set},
        {"unset",               ls_unset},
        {"wait",                ls_wait},
        {"sleep",               ls_sleep},
        {"run",                 ls_run},
        {"slave_allocate",      ls_allocate_slave},
        {"slave_free",          ls_free_slave},
        {NULL,                  NULL}
};
/* -------------------------------------------------------------------------- */
/** Table of methods that will be exported to the Lua in slave namespace. */
LOCAL const luaL_reg slave_methods[] = {
        {"request",             ls_slave_request},
        {"release",             ls_slave_release},
        {"run",                 ls_slave_run},
        {"expect",              ls_slave_expect},
        {"send",                ls_slave_sendrecv},
        {NULL,                  NULL}
};
/* -------------------------------------------------------------------------- */
/** Table of methods that will come from loaded .so test module. They will be
 *  exported to Lua. */
LOCAL luaL_reg *test_module_method = INITPTR;
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* -------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ===================================== */
/* -------------------------------------------------------------------------- */
/** Comparator used for findings events on the list 
 */
LOCAL int _look4event( const void *a, const void *b )
{
        minEventIf *e = (minEventIf*)a;
        return strcmp(e->Name(e),(char*)b);
}
/* -------------------------------------------------------------------------- */
/** Loads .so test module and exports functions from within it to the Lua.
 *  This function is intended to be called from LUA.
 *  @param l [in] the state of the Lua interpreter.
 *  @return number of variables returned to the Lua: 1 or 0 in case of failure.
 */
LOCAL int ls_load_testmodule (lua_State *l)
{
        int              stacktop       = 0;
        int              tmp            = 0;
        int              n              = 0;
        char            *testmodule     = INITPTR;
        void            *dllhandle      = INITPTR;
        ptr2get          tsgettestcases = INITPTR;
        DLList          *tclist         = INITPTR;
        DLListIterator   it             = DLListNULLIterator;
        TCInfo          *tcdata         = INITPTR;
        TMInfo *ti = INITPTR;
        stacktop = lua_gettop(l);
        if( stacktop != 1 ) {
                MIN_WARN("Incorrect number of arguments for"
                           " min.load: expected one argument,"
                           " got %d",stacktop);
                return 0;
        }
        if( lua_isstring(l,1) ) {
                testmodule = (char*)lua_tostring(l,1);
                dllhandle  = tl_open_tc(testmodule);
                if(dllhandle == NULL ) {
                        MIN_WARN("Failed to load test module: %s"
                                  ",error %s\n",testmodule,dlerror());
                        return 0;
                }
                tsgettestcases = dlsym(dllhandle,"ts_get_test_cases");
                if( tsgettestcases  == NULL ) {
                        MIN_WARN("Unable to resolve ts_get_test_cases "
                                    "function from test module %s, error: %s"
                                   ,testmodule,dlerror());
                        dlclose(dllhandle);
                }
                tclist=dl_list_create();
                tsgettestcases(&tclist);
                test_module_method = NEW2(luaL_reg,dl_list_size(tclist)+1);
                it=dl_list_head(tclist);
                while(it!=DLListNULLIterator) {
                        tcdata = (TCInfo*)dl_list_data(it);
                        tmp = strlen(tcdata->name_);
                        test_module_method[n].name = NEW2(char,tmp+1);
                        STRCPY( (char*)test_module_method[n].name
                              , tcdata->name_
                              , tmp+1 );
                        test_module_method[n].func = tcdata->test_;
                        n++;
                        it = dl_list_next(it);
                }
                dl_list_free(&tclist);
                test_module_method[n].name = NULL;
                test_module_method[n].func = NULL;


                luaL_openlib(l,testmodule,test_module_method,0);
                luaL_newmetatable(l,testmodule);
                lua_pushliteral(l,"__index");
                lua_pushvalue(l,-3);
                lua_rawset(l,-3);
                lua_pushliteral(l,"__metatable");
                lua_pushvalue(l,-3);
                lua_rawset(l,-3);
                lua_pop(l,1);

                ti = (TMInfo*)lua_newuserdata(l,sizeof(TMInfo));
                luaL_getmetatable(l,testmodule);
                lua_setmetatable(l,-2);
                ti->dllhnd_=dllhandle;
                STRCPY(ti->name_,testmodule,128);
                return 1;

        } else {
                MIN_WARN("Wrong argument type in"
                           " min.register_testcase: expeced string");
                return 0;
        }
        return 0;
}
/* -------------------------------------------------------------------------- */
/** Unloads loaded test module.
 *  This function is intended to be called from LUA.
 *  @param l [in] the state of the Lua interpreter.
 *  @return number of variables returned to the Lua: 0
 */
LOCAL int ls_release_testmodule (lua_State *l)
{
        int stacktop = lua_gettop (l);
        TMInfo *ti = INITPTR;
        if(stacktop!=1) {
                MIN_WARN("Invalid number of arguments for min.unload"
                            ", expected one, got %d",stacktop);
                return 0;
        }
        if(lua_isuserdata(l,1)) { ti = (TMInfo*)lua_touserdata(l,1); }
        else {
                MIN_WARN("Wrong argument type for min.unload, expected"
                            " test module handle.");
                return 0;
        }
        lua_pop(l,1);
        return 0;
}
/* -------------------------------------------------------------------------- */
/** Registers test case on the global list.
 *  This function is intended to be called from LUA.
 *  @param l [in] the state of the Lua interpreter.
 *  @return number of variables returned to the Lua: 0.
 */
LOCAL int ls_register_testcase (lua_State *l)
{
        char    *tctitle        = INITPTR;
        char    *funname        = INITPTR;
        TCInfo  *tcinfo         = INITPTR;
        int      stacktop       = lua_gettop(l);
        if(stacktop==1) {
                if( ls_tc_list == INITPTR ) { ls_tc_list = dl_list_create(); }
                if(lua_isstring(l,1)) { funname = (char*)lua_tostring(l,1); }
                else {
                        MIN_WARN("Wrong argument type in"
                                    " min.register_testcase: expeced string");
                        return 0;
                }
        } else if(stacktop==2) {
                if( ls_tc_list == INITPTR ) { ls_tc_list = dl_list_create(); }
                if(lua_isstring(l,1)) { funname = (char*)lua_tostring(l,1); }
                else {
                        MIN_WARN("Wrong argument type in"
                                    " min.register_testcase: expeced string");
                        return 0;
                }
                if(lua_isstring(l,2)) { tctitle = (char*)lua_tostring(l,2); }
                else {
                        MIN_WARN("Wrong argument type in"
                                    " min.register_testcase: expeced string");
                        return 0;
                }
        } else {
                MIN_WARN("Incorrect number of arguments for"
                            " min.register_testcase: expected two arguments.");
                return 0;
        }
        tcinfo  = NEW(TCInfo);
        memset(tcinfo->name_,'\0',MaxTestCaseName);
        STRCPY(tcinfo->name_
              ,(tctitle==INITPTR)?funname:tctitle
              ,(tctitle==INITPTR)?strlen(funname):strlen(tctitle));
        tcinfo->test_ = INITPTR;
        tcinfo->id_   = dl_list_size(ls_tc_list)+1;
        dl_list_add(ls_tc_list,(void*)tcinfo);
        return 0;
}
/* -------------------------------------------------------------------------- */
/** Prints message on the UI.
 *  @param l [in] the state of the Lua interpreter.
 *  @return number of variables returned to the Lua: 0.
 */
LOCAL int ls_print (lua_State *l)
{
        char *format = INITPTR;
        int stacktop = lua_gettop(l);
        if( stacktop > 0 ) {
                if( lua_isstring(l,1) ) {
                        format = (char*)lua_tostring(l,1);
                        tm_printf(1,"",format);
                } else {
                        MIN_WARN("Wrong argument type in min.print:"
                                    " expeced string");
                        return 0;
                }
        } else {
                MIN_WARN("Incorrect number of arguments for min.print:"
                            " expected one argument.");
                return 0;
        }
        return 0;
}
/* -------------------------------------------------------------------------- */
/** Requests an event.
 *  @param l [in] the state of the Lua interpreter.
 *  @return number of variables returned to the Lua: 0.
 */
LOCAL int ls_request (lua_State *l)
{
        minEventIf *event = INITPTR;
        char *eventname = INITPTR;
        int state = 0;
        int stacktop = lua_gettop(l);
        if(stacktop==1) {
                if(lua_isstring(l,-1)) {eventname=(char*)lua_tostring(l,-1);} 
                else {
                        MIN_WARN("Wrong argument type in min.request:"
                                    " expeced event name");
                        return 0;
                }
        } else if(stacktop==2) {       
                if(lua_isnumber(l,-1)) {state=lua_tonumber(l,-1);} 
                else {
                        MIN_WARN("Wrong argument type in min.request:"
                                    " expeced event name");
                        return 0;
                }
                if(lua_isstring(l,-2)) {eventname =(char*)lua_tostring(l,-2);} 
                else {
                        MIN_WARN("Wrong argument type in min.request:"
                                    " expeced event type");
                        return 0;
                }
        } else {
                MIN_WARN("Incorrect number of arguments for min.request:"
                            " expected at least event name");
                return 0;
        }
        event = min_event_create(eventname,state?EState:EIndication);
        event->SetEventType(event,state?EState:EIndication);
        event->SetType(event,EReqEvent);
        Event(event);
        if(event->event_status_ != EventStatOK) {
                MIN_ERROR("Event: %s - requesting error!",eventname);
        } else {
                MIN_DEBUG("Event %s requested",eventname);
                if( events == INITPTR ) { events = dl_list_create(); }
                dl_list_add(events,(void*)event);
        }
        return 0;
}
/* -------------------------------------------------------------------------- */
/** Releases an event.
 *  @param l [in] the state of the Lua interpreter.
 *  @return number of variables returned to the Lua: 0.
 */
LOCAL int ls_release (lua_State *l)
{
        minEventIf *event = INITPTR;
        char *eventname = INITPTR;
        DLListIterator it = DLListNULLIterator;
        int stacktop = lua_gettop(l);
        if(stacktop==1) {
                if(lua_isstring(l,-1)) {eventname=(char*)lua_tostring(l,-1);} 
                else {
                        MIN_WARN("Wrong argument type in min.release:"
                                    " expeced event name");
                        return 0;
                }
        } else {
                MIN_WARN("Incorrect number of arguments for min.set:"
                            " expected event name");
                return 0;
        }
        it = dl_list_find( dl_list_head(events)
                         , dl_list_tail(events)
                         , _look4event
                         , eventname );
        if( it == DLListNULLIterator ) {
                MIN_ERROR("Event: %s could not be found on registered"
                            " events list",eventname);
                return 0;
        }
        event = (minEventIf*)dl_list_data(it);
        event->SetType(event,ERelEvent);
        Event(event);
        if(event->event_status_ != EventStatOK) {
                MIN_ERROR("Event: %s - releasing error!",eventname);
        }
        min_event_destroy( event );
        dl_list_remove_it(it);
        if(dl_list_size(events)==0) {dl_list_free(&events);events=INITPTR;}
        return 0;
}
/* -------------------------------------------------------------------------- */
/** Sets an event.
 *  @param l [in] the state of the Lua interpreter.
 *  @return number of variables returned to the Lua: 0.
 */
LOCAL int ls_set (lua_State *l)
{
        minEventIf *event = INITPTR;
        char *eventname = INITPTR;
        int state = 0;
        int stacktop = lua_gettop(l);
        if(stacktop==1) {
                if(lua_isstring(l,-1)) {eventname=(char*)lua_tostring(l,-1);} 
                else {
                        MIN_WARN("Wrong argument type in min.set:"
                                    " expeced event name");
                        return 0;
                }
        } else if(stacktop==2) {       
                if(lua_isnumber(l,-1)) {state=lua_tonumber(l,-1);} 
                else {
                        MIN_WARN("Wrong argument type in min.set:"
                                    " expeced event name");
                        return 0;
                }
                if(lua_isstring(l,-2)) {eventname=(char*)lua_tostring(l,-2);} 
                else {
                        MIN_WARN("Wrong argument type in min.set:"
                                    " expeced event type");
                        return 0;
                }
        } else {
                MIN_WARN("Incorrect number of arguments for min.set:"
                            " expected at least event name");
                return 0;
        }
        event = min_event_create(eventname,state?EState:EIndication);
        event->SetEventType(event,state?EState:EIndication);
        event->dont_block_ = ESFalse;
        event->SetType(event,ESetEvent);
        Event(event);
        if(event->event_status_ != EventStatOK) {
                MIN_ERROR("Event setting error!");
        }
        min_event_destroy( event );
        return 0;
}
/* -------------------------------------------------------------------------- */
/** Unsets an event.
 *  @param l [in] the state of the Lua interpreter.
 *  @return number of variables returned to the Lua: 0.
 */
LOCAL int ls_unset (lua_State *l)
{
        minEventIf *event = INITPTR;
        char *eventname = INITPTR;
        int stacktop = lua_gettop(l);
        if(stacktop==1) {
                if(lua_isstring(l,-1)) {eventname=(char*)lua_tostring(l,-1);} 
                else {
                        MIN_WARN("Wrong argument type in min.unset:"
                                    " expeced event name");
                        return 0;
                }
        } else {
                MIN_WARN("Incorrect number of arguments for min.unset:"
                            " expected event name");
                return 0;
        }
        event = min_event_create(eventname,EState);
        event->SetEventType(event,EState);
        event->dont_block_ = ESTrue;
        event->SetType(event,EUnsetEvent);
        Event(event);
        if(event->event_status_ != EventStatOK) {
                MIN_ERROR("Event unsetting error!");
        }
        min_event_destroy( event );
        return 0;
}
/* -------------------------------------------------------------------------- */
/** Waits for an event.
 *  @param l [in] the state of the Lua interpreter.
 *  @return number of variables returned to the Lua: 0.
 */
LOCAL int ls_wait( lua_State *l )
{
        minEventIf *event = INITPTR;
        char *eventname = INITPTR;
        int stacktop = lua_gettop(l);
        int state = 0;
        DLListIterator it = DLListNULLIterator;

        if(stacktop==1) {
                if(lua_isstring(l,-1)) {eventname=(char*)lua_tostring(l,-1);} 
                else {
                        MIN_WARN("Wrong first argument type in min.wait:"
                                    " expeced event name");
                        return 0;
                }
        } else if(stacktop==2) {       
                if(lua_isnumber(l,-1)) {state=lua_tonumber(l,-1);} 
                else {
                        MIN_WARN("Wrong first argument type in min.wait:"
                                    " expeced event name");
                        return 0;
                }
                if(lua_isstring(l,-2)) {eventname=(char*)lua_tostring(l,-2);} 
                else {
                        MIN_WARN("Wrong second argument type in min.wait:"
                                    " expeced event type");
                        return 0;
                }
        } else {
                MIN_WARN("Incorrect number of arguments for min.wait:"
                            " expected at least event name");
                return 0;
        }
        it = dl_list_find( dl_list_head(events)
                         , dl_list_tail(events)
                         , _look4event
                         , eventname );
        if(it==DLListNULLIterator) {
                event = min_event_create(eventname,state?EState:EIndication);
                event->SetEventType(event,state?EState:EIndication);
                event->dont_block_ = ESFalse;
                event->SetType(event,EReqEvent);
                Event(event);
                if(event->event_status_ != EventStatOK) {
                        MIN_ERROR("Event: %s - requesting error!",eventname);
                        min_event_destroy(event);
                        return 0;
                }
        } else { event = (minEventIf*)dl_list_data(it); }
        event->SetType(event,EWaitEvent);
        event->dont_block_ = ESFalse;
        Event(event);
        if(event->event_status_ != EventStatOK) {
                MIN_ERROR("Event: %s - waiting error!",eventname);
                min_event_destroy(event);
                return 0;
        }
        if(it==DLListNULLIterator) {
                event->dont_block_ = ESTrue;
                event->SetType(event,ERelEvent);
                Event(event);
                if(event->event_status_ != EventStatOK) {
                        MIN_ERROR("Event: %s - releasing error!",eventname);
                }
                min_event_destroy(event);
        }
        return 0;
}
/* -------------------------------------------------------------------------- */
/** Sleeps
 *  This function is intended to be called from LUA.
 *  @param l [in] the state of the Lua interpreter.
 *  @return number of variables returned to the Lua: 0.
 */
LOCAL int ls_sleep( lua_State *l )
{
        long msec = 0;
        if(lua_gettop(l)>0) {
                msec = lua_tonumber(l,-1);
                msec=msec*1000;
                usleep(msec);
        } else {
                MIN_WARN("Incorrect number of arguments for min.sleep:"
                            " expected one argument");
        }
        return 0;
}
/* -------------------------------------------------------------------------- */
/** Runs a test from specified library.
 *  @param l [in] the state of the Lua interpreter.
 *  @return number of variables returned to the Lua: 1.
 */
LOCAL int ls_run( lua_State *l )
{
        int stacktop  = lua_gettop(l);
        char *tmname  = INITPTR;
        char *cfgfile = INITPTR;
        int tcid      = 0;
        char *tcname  = INITPTR;
        int ret       = 0;
        int tmp       = 0;
        TestCaseInfo *tci = INITPTR;
        test_libl_t testlib;
        TestCaseResult tcr;
        DLList *cases = INITPTR;
        DLListIterator it = DLListNULLIterator;
        if(stacktop<2) {
                ret=1;
                lua_pushnumber(l,ret);
                MIN_WARN("Incorrect number of arguments for min.run:"
                            " expected at least two arguments");
                return 1;
        }
        if(lua_isstring(l,1)) { tmname = (char*)lua_tostring(l,1); }
        if(lua_isnumber(l,2)) { tcid = lua_tonumber(l,2); }
        else {
                if(stacktop==2) {
                        tcname = (char*)lua_tostring(l,2);
                } else {
                        cfgfile = (char*)lua_tostring(l,2);
                        if(stacktop<3) {
                                ret=1;
                                lua_pushnumber(l,ret);
                                MIN_WARN("Incorrect number of arguments"
                                            " for min.run: expected at least"
                                            " two arguments");
                                return 1;
                        }
                        if(lua_isnumber(l,3)) { tcid = lua_tonumber(l,3); }
                        else { tcname = (char*)lua_tostring(l,3); }
                }
        }
        ret = tl_open(&testlib,tmname);
        if (tcname!=INITPTR) {
                tcid = 1;
                cases = dl_list_create();
                testlib.get_cases_fun_( (cfgfile!=INITPTR)?cfgfile:"dummy.cfg"
                                      , &cases );
                it=dl_list_head(cases);
                while(it!=DLListNULLIterator) {
                        tci = (TestCaseInfo*)dl_list_data(it);
                        tmp = strcmp(tci->name_,tcname);
                        if(tmp==0) { break; }
                        it = dl_list_next(it);
                        tcid++;
                }               
                it=dl_list_head(cases);
                while(it!=DLListNULLIterator) {
                        free(dl_list_data(it));
                        dl_list_remove_it(it);
                        it=dl_list_head(cases);
                }
                dl_list_free(&cases);
        }
        if( ret != ENOERR ) {
                ret=1;
                lua_pushnumber(l,ret);
                MIN_ERROR("Error opening: %s",testlib);
                return 1;
        }      
        testlib.run_case_fun_(tcid,(cfgfile!=INITPTR)?cfgfile:"dummy.cfg",&tcr);
        lua_pushnumber(l,tcr.result_);
        return 1;
}
/* -------------------------------------------------------------------------- */
/** Allocates slave device for external interface purposes.
 *  @param l [in] the state of the Lua interpreter.
 *  @return number of variables returned to the Lua: 1.
 */
LOCAL int ls_allocate_slave(lua_State *l)
{
        int stacktop = lua_gettop(l);
        char *slavetype = INITPTR;
        char *slavename = INITPTR;
        int mqid = 0;
        MsgBuffer msg;
        SlaveInfo *si = INITPTR;
        msg.receiver_=getppid();
        msg.sender_=getpid();
        msg.type_=MSG_EXTIF;
        msg.param_=0;
        msg.extif_msg_type_=EAllocateSlave;
        if(stacktop==2) {
                if(lua_isstring(l,1)) { slavetype = (char*)lua_tostring(l,1); }
                else {
                        MIN_WARN("Wrong 1st argument type for"
                                    " min.allocate_slave: expected string"
                                    " (slave type)");
                        return 0;
                }
                if(lua_isstring(l,2)) { slavename = (char*)lua_tostring(l,2); }
                else {
                        MIN_WARN("Wrong 2nd argument type for"
                                    " min.allocate_slave: expected string"
                                    " (slave name)");
                        return 0;
                }
        } else {
                MIN_WARN("Incorrect number of arguments for"
                            " min.allocate_slave: expected two arguments,"
                            "slave type and slave name");
                return 0;
        }
        if(strlen(slavename)==0) {
                MIN_WARN("Slave name should contain at least one char");
                return 0;
        }
        if( strlen(slavetype)==0 ) {
                MIN_WARN("Slave type is wrong!");
                return 0;
        }
        if( strlen(slavename)==0 ) {
                MIN_WARN("Slave name is wrong!");
                return 0;
        }
        STRCPY(msg.desc_,slavetype,MaxDescSize);
        STRCPY(msg.message_,slavename,MaxMsgSize);
        mqid=mq_open_queue('a');
        mq_send_message(mqid,&msg);
        stacktop = mq_read_message(mqid,getpid(),&msg);
        if(stacktop==-1) { ; }

        luaL_openlib(l,slavename,slave_methods,0);
        luaL_newmetatable(l,slavename);
        lua_pushliteral(l,"__index");
        lua_pushvalue(l,-3);
        lua_rawset(l,-3);
        lua_pushliteral(l,"__metatable");
        lua_pushvalue(l,-3);
        lua_rawset(l,-3);
        lua_pop(l,1);

        si = (SlaveInfo*)lua_newuserdata(l,sizeof(SlaveInfo));
        luaL_getmetatable(l,slavename);
        lua_setmetatable(l,-2);
        STRCPY(si->name_,slavename,128);
        return 1;
}
/* -------------------------------------------------------------------------- */
/** Frees allocated slave device.
 *  @param l [in] the state of the Lua interpreter.
 *  @return number of variables returned to the Lua: 0.
 */
LOCAL int ls_free_slave (lua_State *l)
{
        int stacktop = lua_gettop(l);
        SlaveInfo *slave = INITPTR;
        int mqid = 0;
        MsgBuffer msg;
        msg.receiver_=getppid();
        msg.sender_=getpid();
        msg.type_=MSG_EXTIF;
        msg.param_=0;
        msg.extif_msg_type_=EFreeSlave;
        if(stacktop!=1) {
                MIN_WARN("Wrong number of parameters to min.free_slave"
                            ", expected slav, got: %d",stacktop);
                return 0;
        }
        if(lua_isuserdata(l,stacktop)) {
                slave = (SlaveInfo*)lua_touserdata(l,stacktop);
        } else {
                MIN_WARN("Wrong type of parameter min.free_slave"
                            ", expected slave handler");
                return 0;
        }
        if(slave==NULL||slave==INITPTR) {
                MIN_WARN("Wrong type of parameter min.free_slave"
                            ", expected slave handler");
                return 0;
        }
        MIN_DEBUG("");
        STRCPY(msg.message_,slave->name_,MaxMsgSize);
        mqid=mq_open_queue('a');
        mq_send_message(mqid,&msg);
        stacktop = mq_read_message(mqid,getpid(),&msg);
        if(stacktop==-1) { MIN_WARN("Message reading error from MQ"); }
        return 0;
}
/* -------------------------------------------------------------------------- */
/** Requests an event through ext interface
 *  @param l [in] the state of the Lua interpreter.
 *  @return number of variables returned to the Lua: 0.
 */
LOCAL int ls_slave_request (lua_State *l)
{
        int stacktop = lua_gettop(l);
        SlaveInfo *si = INITPTR;
        char *eventname = INITPTR;
        int state = 0;
        Text *command = INITPTR;
        MsgBuffer msg;
        int mqid=-1;
        if(stacktop<1||stacktop>3) {
                MIN_WARN("Wrong number of parameters to <slave>.request"
                            " expected at least two, got: %d",stacktop);
                return 0;
        }
        if(lua_isuserdata(l,1)) { si=(SlaveInfo*)lua_touserdata(l,1); }
        else {
                MIN_WARN("Wrong parameter to <slave>.request, expected"
                            " slave handler");
        }
        if(lua_isstring(l,2)) { eventname=(char*)lua_tostring(l,2); }
        else {
                MIN_WARN("Wrong parameter to <slave>.request, expected"
                            " event name");
        }
        if(stacktop==3) {
                if(lua_isnumber(l,3)) { state=lua_tonumber(l,3); }
                else {
                        MIN_WARN("Wrong parameter to <slave>.request,"
                                    " expected event type (enum)");
                }
        }
        command = tx_create("request ");
        tx_c_append(command,eventname);
        if(state==1) {
                tx_c_append(command," ");
                tx_c_append(command,"state");
        }
        STRCPY(msg.message_,si->name_,MaxMsgSize);
        STRCPY(msg.desc_,tx_share_buf(command),MaxDescSize);
        tx_destroy(&command);

        msg.receiver_=getppid();
        msg.sender_=getpid();
        msg.type_=MSG_EXTIF;
        msg.param_=0;
        msg.extif_msg_type_=ERemoteSlave;
        mqid=mq_open_queue('a');
        mq_send_message(mqid,&msg);

        return 0;
}
/* -------------------------------------------------------------------------- */
/** Releases an event through ext interface
 *  @param l [in] the state of the Lua interpreter.
 *  @return number of variables returned to the Lua: 0.
 */
LOCAL int ls_slave_release (lua_State *l)
{
        int stacktop = lua_gettop(l);
        SlaveInfo *si = INITPTR;
        char *eventname = INITPTR;
        Text *command = INITPTR;
        int mqid=0;
        MsgBuffer msg;
        if(stacktop!=2) {
                MIN_WARN("Wrong number of parameters to <slave>.release"
                            " expected two, got: %d",stacktop);
                return 0;
        }
        if(lua_isuserdata(l,1)) { si=(SlaveInfo*)lua_touserdata(l,1); }
        else {
                MIN_WARN("Wrong parameter to <slave>.release, expected"
                            " slave handler");
        }
        if(lua_isstring(l,2)) { eventname=(char*)lua_tostring(l,2); }
        else {
                MIN_WARN("Wrong parameter to <slave>.release, expected"
                            " event name");
        }
        command = tx_create("release ");
        tx_c_append(command,eventname);
        STRCPY(msg.message_,si->name_,MaxMsgSize);
        STRCPY(msg.desc_,tx_share_buf(command),MaxDescSize);
        tx_destroy(&command);

        msg.receiver_=getppid();
        msg.sender_=getpid();
        msg.type_=MSG_EXTIF;
        msg.param_=0;
        msg.extif_msg_type_=ERemoteSlave;
        mqid=mq_open_queue('a');
        mq_send_message(mqid,&msg);

        return 0;
}
/* -------------------------------------------------------------------------- */
/** Executes test on slave side through external interface.
 *  @param l [in] the state of the Lua interpreter.
 *  @return number of variables returned to the Lua: 0.
 */
LOCAL int ls_slave_run (lua_State *l)
{
        int stacktop = lua_gettop(l);
        SlaveInfo *si = INITPTR;
        char *tmname = INITPTR;
        char *cfgfile = INITPTR;
        char *tcname = INITPTR;
        int tcid = 0;
        char ctcid[5];
        Text *command = INITPTR;
        int mqid=0;
        MsgBuffer msg;
        memset(ctcid,'\0',5);
        if(stacktop<3) {
                MIN_WARN("Incorrect number of arguments for <slave>.run:"
                            " expected at least three arguments, got: %d"
                           ,stacktop);
                return 0;
        }
        if(lua_isuserdata(l,1)) { si=(SlaveInfo*)lua_touserdata(l,1); }
        else {
                MIN_WARN("Wrong parameter to <slave>.request, expected"
                            " slave handler");
        }
        if(lua_isstring(l,2)) { tmname = (char*)lua_tostring(l,2); }
        if(lua_isnumber(l,3)) { tcid = lua_tonumber(l,3); }
        else {
                if(stacktop==3) {
                        tcname = (char*)lua_tostring(l,3);
                } else {
                        cfgfile = (char*)lua_tostring(l,3);
                        if(stacktop<4) {
                                MIN_WARN("Incorrect number of arguments"
                                            " for min.run: expected at least"
                                            " two arguments");
                                return 1;
                        }
                        if(lua_isnumber(l,4)) { tcid = lua_tonumber(l,4); }
                        else { tcname = (char*)lua_tostring(l,4); }
                }
        }
        command = tx_create("run ");
        tx_c_append(command,tmname);
        tx_c_append(command," ");
        tx_c_append(command,(cfgfile==INITPTR)?"dummy.cfg":cfgfile);
        tx_c_append(command," ");
        sprintf(ctcid,"%d",tcid);
        tx_c_append(command,ctcid);
        if(tcname!=INITPTR) {
                tx_c_append(command," title=\"");
                tx_c_append(command,tcname);
                tx_c_append(command,"\"");
        }
        STRCPY(msg.message_,si->name_,MaxMsgSize);
        STRCPY(msg.desc_,tx_share_buf(command),MaxDescSize);
        MIN_DEBUG("command = %s", tx_share_buf(command));
        tx_destroy(&command);

        msg.receiver_=getppid();
        msg.sender_=getpid();
        msg.type_=MSG_EXTIF;
        msg.param_=0;
        msg.extif_msg_type_=ERemoteSlave;
        mqid=mq_open_queue('a');
        mq_send_message(mqid,&msg);

        while(1) {
                usleep(10000);
                stacktop = mq_read_message(mqid,getpid(),&msg);
                if(stacktop==-1) { continue; }
                if(msg.extif_msg_type_==ERemoteSlaveResponse) {
                        lua_pushnumber(l,msg.param_);
                        return 1;
                }
        }

        return 0;
}
/* -------------------------------------------------------------------------- */
/** Waits for variable passed via send/receive mechanism.
 *  @param l [in] the state of the Lua interpreter.
 *  @return number of variables returned to the Lua: 1.
 */
LOCAL int ls_slave_expect (lua_State *l)
{
        int stacktop = lua_gettop(l);
        SlaveInfo *si = INITPTR;
        char *vname = INITPTR;
        Text *command = INITPTR;
        int mqid=0;
        MsgBuffer msg;

        if(stacktop!=2) {
                MIN_WARN("Incorrect number of arguments for <slave>.expect:"
                            " expected two arguments, got: %d",stacktop);
                return 0;
        }
        if(lua_isuserdata(l,1)) { si=(SlaveInfo*)lua_touserdata(l,1); }
        else {
                MIN_WARN("Wrong first parameter to <slave>.expect, expected"
                            " slave handler");
                return 0;
        }
        if(lua_isstring(l,2)) { vname=(char*)lua_tostring(l,2); }
        else {
                MIN_WARN("Wrong second argument type to <slave>.expect"
                            " expected variable name [string]");
                return 0;
        }
        command = tx_create("expect ");
        tx_c_append(command,vname);
        STRCPY(msg.message_,si->name_,MaxMsgSize);
        STRCPY(msg.desc_,tx_share_buf(command),MaxDescSize);
        tx_destroy(&command);

        msg.receiver_=getppid();
        msg.sender_=getpid();
        msg.type_=MSG_EXTIF;
        msg.param_=0;
        msg.extif_msg_type_=ERemoteSlave;
        mqid=mq_open_queue('a');
        mq_send_message(mqid,&msg);
        while(1) {
                usleep(10000);
                stacktop = mq_read_message(mqid,getpid(),&msg);
                if(stacktop==-1) { continue; }
                if(msg.type_!=MSG_SNDRCV) { continue; }
                if(strcmp(msg.message_,vname)!=0) { continue; }
                lua_pushstring(l,msg.desc_);
                return 1;
        }

        return 0;
}
/* -------------------------------------------------------------------------- */
/** Sends value through send/receive mechanism.
 *  @param l [in] the state of the Lua interpreter.
 *  @return number of variables returned to the Lua: 0.
 */
LOCAL int ls_slave_sendrecv (lua_State *l)
{
        int stacktop=lua_gettop(l);
        SlaveInfo *si = INITPTR;
        int ivarval = 0;
        char *varval = INITPTR;
        char *vname = INITPTR;
        char cval[16];
        Text *command = INITPTR;
        int mqid=0;
        MsgBuffer msg;
        memset(cval,'\0',16);
        if(stacktop!=3) {
                MIN_WARN("Incorrect number of arguments for <slave>.sendrcv:"
                            " expected three arguments, got: %d",stacktop);
                return 0;
        }
        if(lua_isuserdata(l,1)) { si=(SlaveInfo*)lua_touserdata(l,1); }
        else {
                MIN_WARN("Wrong parameter to <slave>.request, expected"
                            " slave handler");
        }
        if(lua_isstring(l,2)) { vname=(char*)lua_tostring(l,2); }
        else {
                MIN_WARN("Wrong argument type to <slave>.sendrcv"
                            " expected variable name [string]");
                return 0;
        }
        if(lua_isnumber(l,3)) {
                ivarval=lua_tonumber(l,3);
                sprintf(cval,"%d",ivarval);
                varval=&cval[0];
        } else if(lua_isstring(l,3)) {
                varval = (char*)lua_tostring(l,3);
        } else {
                MIN_WARN("Wrong variable value to <slave>.sendrcv"
                            " expected [string] or [number]");
                return 0;
        }
        command = tx_create("sendreceive ");
        tx_c_append(command,vname);
        tx_c_append(command,"=");
        tx_c_append(command,varval);
        STRCPY(msg.message_,si->name_,MaxMsgSize);
        STRCPY(msg.desc_,tx_share_buf(command),MaxDescSize);
        tx_destroy(&command);

        msg.receiver_=getppid();
        msg.sender_=getpid();
        msg.type_=MSG_EXTIF;
        msg.param_=0;
        msg.extif_msg_type_=ERemoteSlave;
        mqid=mq_open_queue('a');
        mq_send_message(mqid,&msg);

        return 0;
}
/* -------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ======================================= */
/* -------------------------------------------------------------------------- */
/** Runs specified test case.
 *  @param id[in] test case id.
 *  @param scriptfile[in] config file to which test case belongs to.
 *  @param result[out] for test case result returning.
 *  @return ENOERR or -1 in case of failure.
 *
 *  NOTE: This function is a part of Test Module API and it is called by TMC
 */
int tm_run_test_case( unsigned int      id
                    , const char     *  scriptfile
                    , TestCaseResult *  result )
{
        lua_State *l = INITPTR;
        int stacktop = 0;
        Text *tcname = INITPTR;
        int tcres = -2;
        char *tcdesc = INITPTR;
        int luastatus = 0;
        char* errmsg = INITPTR;
        if( scriptfile == INITPTR ) {
                MIN_WARN("Expected parameter: [scriptfile] is not set.");
                return -1;
        }
        if( result == INITPTR ) {
                MIN_WARN("Expected parameter: [result] is not set.");
                return -1;
        }
        /* Init lua environment */
        l = lua_open();
        luaL_openlibs(l);
        /* Load our MIN2Lua API */
        luaL_openlib(l,"min",min_methods,0);
        /* Load our special functions */
        luastatus = luaL_loadfile(l,"/usr/share/min/minbaselib.lua");
        if( luastatus != ENOERR ) {
                errmsg = (char*)lua_tostring(l,-1);
                switch(luastatus)
                {
                case LUA_ERRSYNTAX:
                        MIN_ERROR("Syntax error: %s",errmsg);
                        break;
                case LUA_ERRMEM:
                        MIN_ERROR("Memory error: %s",errmsg);
                        break;
                case LUA_ERRFILE:                        
                        MIN_ERROR("File io error: %s",errmsg);
                        break;
                }
        }
        luastatus = lua_pcall(l,0,LUA_MULTRET,0);
        if( luastatus != ENOERR ) {
                errmsg = (char*)lua_tostring(l,-1);
                switch(luastatus)
                {
                case LUA_ERRRUN:
                        MIN_ERROR("Syntax error: %s",errmsg);
                        break;
                case LUA_ERRMEM:
                        MIN_ERROR("Memory error: %s",errmsg);
                        break;
                }
                goto EXIT;
        }
        /* Obtain name of the test case of a given id... */
        lua_getfield(l,LUA_GLOBALSINDEX,"__testcase_lookup");
        lua_pushstring(l,scriptfile);
        lua_pushnumber(l,id-1);
        luastatus = lua_pcall(l,2,1,0);
        if( luastatus != ENOERR ) {
                errmsg = (char*)lua_tostring(l,-1);
                switch(luastatus)
                {
                case LUA_ERRRUN:
                        MIN_ERROR("Syntax error: %s",errmsg);
                        break;
                case LUA_ERRMEM:
                        MIN_ERROR("Memory error: %s",errmsg);
                        break;
                }
                goto EXIT;
        }
        stacktop = lua_gettop(l);
        tcname = tx_create( (char*)lua_tostring(l,stacktop) );
        /* === 2. Call test case, return result and so on === */
        luastatus = luaL_loadfile(l,scriptfile);
        luastatus = lua_pcall(l,0,LUA_MULTRET,0);
        if( luastatus != ENOERR ) {
                errmsg = (char*)lua_tostring(l,-1);
                switch(luastatus)
                {
                case LUA_ERRRUN:
                        MIN_ERROR("Syntax error: %s",errmsg);
                        break;
                case LUA_ERRMEM:
                        MIN_ERROR("Memory error: %s",errmsg);
                        break;
                }
                goto EXIT;
        }
        lua_getfield(l,LUA_GLOBALSINDEX,tx_share_buf(tcname));
        luastatus = lua_pcall(l,0,2,0);
        if( luastatus != ENOERR ) {
                errmsg = (char*)lua_tostring(l,-1);
                switch(luastatus)
                {
                case LUA_ERRRUN:
                        MIN_ERROR("Syntax error: %s",errmsg);
                        break;
                case LUA_ERRMEM:
                        MIN_ERROR("Memory error: %s",errmsg);
                        break;
                }
                goto EXIT;
        }
        stacktop = lua_gettop(l);
        if(lua_isstring(l,stacktop)) {tcdesc=(char*)lua_tostring(l,stacktop);}
        else { MIN_WARN("Test Case result description is not set."); }
        if(lua_isnumber(l,stacktop-1)) { tcres = lua_tonumber(l,stacktop-1); }
        else { MIN_ERROR("Test Case result is not set."); }
EXIT:
        lua_close(l);
        tx_destroy(&tcname);
        /* === 3. Interprete the test result === */
        RESULT(result,tcres,(tcdesc==INITPTR)?"Description not set":tcdesc);
}
/* -------------------------------------------------------------------------- */
/** Get test cases from the script file.
 *  @param scriptfile[in] config file from which test cases are extracted.
 *  @param cases[out] for test cases returning.
 *  @return ENOERR or -1 in case of failure.
 *
 *  NOTE: This function is a part of Test Module API and it is called by TMC
 */
int tm_get_test_cases( const char * scriptfile, DLList ** cases )
{
        lua_State *l = INITPTR;
        int luastatus = 0;
        char *errmsg = INITPTR;
        if( scriptfile == INITPTR ) {
                MIN_WARN("Expected parameter: [scriptfile] is not set.");
                return -1;
        }
        if( *cases == INITPTR ) {
                MIN_WARN("Expected parameter: [cases] is not set.");
                return -1;
        }
        /* Init lua environment */
        l = lua_open();
        luaL_openlibs(l);
        /* Load our MIN2LUA API */
        luaL_openlib(l,"min",min_methods,0);
        /* Load our special functions */
        luastatus = luaL_loadfile(l,"/usr/share/min/minbaselib.lua");
        if( luastatus != ENOERR ) {
                errmsg = (char*)lua_tostring(l,-1);
                switch(luastatus)
                {
                case LUA_ERRSYNTAX:
                        MIN_ERROR("Syntax error: %s",errmsg);
                        break;
                case LUA_ERRMEM:
                        MIN_ERROR("Memory error: %s",errmsg);
                        break;
                case LUA_ERRFILE:                        
                        MIN_ERROR("File io error: %s",errmsg);
                        break;
                }
                goto EXIT;
        }
        luastatus = lua_pcall(l,0,LUA_MULTRET,0);
        if( luastatus != ENOERR ) {
                errmsg = (char*)lua_tostring(l,-1);
                switch(luastatus)
                {
                case LUA_ERRRUN:
                        MIN_ERROR("Syntax error: %s",errmsg);
                        break;
                case LUA_ERRMEM:
                        MIN_ERROR("Memory error: %s",errmsg);
                        break;
                }
                goto EXIT;
        }
        /* Get test cases from the script... */
        lua_getfield(l, LUA_GLOBALSINDEX, "__get_testcases");
        lua_pushstring(l,scriptfile);
        luastatus = lua_pcall(l,1,0,0);
        if( luastatus != ENOERR ) {
                errmsg = (char*)lua_tostring(l,-1);
                switch(luastatus)
                {
                case LUA_ERRRUN:
                        MIN_ERROR("Syntax error: %s",errmsg);
                        break;
                case LUA_ERRMEM:
                        MIN_ERROR("Memory error: %s",errmsg);
                        break;
                }
                goto EXIT;
        }
EXIT:
        /* Close lua environment. */
        lua_close(l);
        /* Return list of tc */
        *cases = ls_tc_list;
        return ENOERR;
}
/* -------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS =============================== */
/* None */

/* -------------------------------------------------------------------------- */
/* ================= TESTS FOR LOCAL FUNCTIONS ============================== */
#ifdef MIN_UNIT_TEST
#include "lua_scripter_if.tests"
#endif /* MIN_UNIT_TEST */
/* End of file */

