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
 *  @file       tec_events.c
 *  @version    0.1
 *  @brief      This file contains implementation of engine part of MIN 
 *              event system
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <tec.h>
#include <tec_events.h>
#include <min_common.h>
#include <min_test_event_if.h>

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
typedef enum {
        EState_UNSET,
        EState_SET,
        EState_UNSET_pending
} Event_state_t;

typedef enum {
        ereg_unset,
        ereg_unset_registered,
        ereg_waiting_registered,        /* Not for state events */
        ereg_set,
        ereg_set_registered
} event_reg_state_t;

typedef struct {
        minTestEventDescr_t descr_;
        Event_state_t   state_;
        DLList         *registrations_;
        minEventSrc_t  controller_;
} min_event_t;

typedef struct {
        minEventSrc_t  registrar_;
        event_reg_state_t regstate_;
        min_event_t   *backp_;
} e_registration_t;


const char     *regstate_str[] = { 
	"unset",
	"unset registered",
	"waiting registered",
	"unset pending",
	"set",
	"set registered"
};

DLList         *state_events, *ind_events;


/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
LOCAL int       event_comp (const void *list_data, const void *user_data);
/* ------------------------------------------------------------------------- */
LOCAL int       event_is_registered (min_event_t * event,
                                     minEventSrc_t * registrar);
/* ------------------------------------------------------------------------- */
LOCAL e_registration_t *event_add_registration (min_event_t * event,
                                                minEventSrc_t * registrar);
/* ------------------------------------------------------------------------- */
LOCAL int       event_del_registration (min_event_t * event,
                                        minEventSrc_t * registrar);
/* ------------------------------------------------------------------------- */
LOCAL e_registration_t *event_find_registration (min_event_t * event,
                                                 minEventSrc_t * registrar,
                                                 DLListIterator * itp);
/* ------------------------------------------------------------------------- */
LOCAL void      del_event (DLListIterator it, min_event_t * event);

/* ------------------------------------------------------------------------- */
LOCAL min_event_t *add_ind_event (minTestEventParam_t * param);
/* ------------------------------------------------------------------------- */
LOCAL min_event_t *add_state_event (minTestEventParam_t * param);
/* ------------------------------------------------------------------------- */
LOCAL min_event_t *find_ind_event (minTestEventParam_t * param,
                                    DLListIterator * it);
/* ------------------------------------------------------------------------- */
LOCAL min_event_t *find_state_event (minTestEventParam_t * param,
                                      DLListIterator * it);
/* ------------------------------------------------------------------------- */
LOCAL void      send_event_ind (minEventSrc_t * reveicer, int status);
/* ------------------------------------------------------------------------- */
LOCAL void      do_set_on_state_reg (void *data);
/* ------------------------------------------------------------------------- */
LOCAL void      do_set_on_ind_reg (void *data);
/* ------------------------------------------------------------------------- */
LOCAL void      registration_state_change (e_registration_t * ereg,
                                           event_reg_state_t next_state);
/* ------------------------------------------------------------------------- */
LOCAL void      remote_event_req_response (char *eventname, int status,
                                           TEventType_t etype);
/* ------------------------------------------------------------------------- */
LOCAL void      remote_event_rel_response (char *eventname, int status);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/** List comparison function for events. Passed as an arg to dl_list_find(). 
 *  The event is searched by event name.
 *  @param list_data list item data to be compared
 *  @param user_data the data searched.
 *  @returns a matching event if found, INITPTR if not found
 *  
 */
LOCAL int event_comp (const void *list_data, const void *user_data)
{
        min_event_t   *e;
        minTestEventParam_t *p;

        e = (min_event_t *) list_data;
        p = (minTestEventParam_t *) user_data;

        return strncmp (e->descr_.event_name_, p->event.event_name_,
                        MaxMinEventName);
}
/* ------------------------------------------------------------------------- */
/** The state change of an event registrations is done by calling this func
 *  @param ereg the event registration entry
 *  @param next_state the state we are making the transition to
 */
LOCAL void
registration_state_change (e_registration_t * ereg,
                           event_reg_state_t next_state)
{
        event_reg_state_t cur_state;

        cur_state = ereg->regstate_;
        MIN_DEBUG ("Event reg state change: %s -> %s\n",
                     regstate_str[cur_state], regstate_str[next_state]);

        ereg->regstate_ = next_state;
}
/* ------------------------------------------------------------------------- */
/**  Searches for registration entry on the event
 *   @param event the event to be searched
 *   @param registrar the pid of searched process
 *   @returns pointer to the event registration if found 
 *            INITPTR if no registration is found.
 */
LOCAL e_registration_t *event_find_registration (min_event_t * event,
                                                 minEventSrc_t * registrar,
                                                 DLListIterator * itp)
{
        DLListIterator  it;
        e_registration_t *reg;

        for (it = dl_list_head (event->registrations_);
             it != DLListNULLIterator; it = dl_list_next (it)) {
                reg = (e_registration_t *) dl_list_data (it);
                if (reg->registrar_.pid == registrar->pid) {
                        if (itp != NULL)
                                *itp = it;
                        return reg;
                }
        }
        return INITPTR;
}
/* ------------------------------------------------------------------------- */
/**  Checks if the pid is already registered for the event.
 *   @param event the event registartion list of which is searched
 *   @param registrar the pid to be searched
 *   @returns 1 if registrar is already registered for the event, 0 if not
 */
LOCAL int
event_is_registered (min_event_t * event, minEventSrc_t * registrar)
{
        e_registration_t *reg;

        reg = event_find_registration (event, registrar, NULL);
        if (reg != INITPTR)
                return 1;

        return 0;
}
/* ------------------------------------------------------------------------- */
/** Handles the SET event on an indication event registration.
 *  @param data pointer to event registrations
 */
LOCAL void do_set_on_ind_reg (void *data)
{
        e_registration_t *ereg;

        ereg = (e_registration_t *) data;

        switch (ereg->regstate_) {
        case ereg_waiting_registered:
#ifndef MIN_UNIT_TEST
                send_event_ind (&ereg->registrar_, EventStatOK);
#endif
                if (ereg->registrar_.remote)
                        remote_event_req_response (ereg->backp_->descr_.
                                                   event_name_,
                                                   REM_E_STAT_SET,
                                                   EIndication);
		else
			registration_state_change (ereg, ereg_unset_registered);
                break;
        case ereg_unset_registered:
                registration_state_change (ereg, ereg_set_registered);

                break;
        default:
                MIN_WARN ("unhandled state %s",
                           regstate_str[ereg->regstate_]);
                return;
        }
}
/* ------------------------------------------------------------------------- */
/** Handles the SET event on a state event registration.
 *  @param data pointer to event registrations
 */
LOCAL void do_set_on_state_reg (void *data)
{
        e_registration_t *ereg;

        ereg = (e_registration_t *) data;

        switch (ereg->regstate_) {
        case ereg_waiting_registered:
#ifndef MIN_UNIT_TEST
                send_event_ind (&ereg->registrar_, EventStatOK);
#endif
                if (ereg->registrar_.remote)
                        remote_event_req_response (ereg->backp_->descr_.
                                                   event_name_,
                                                   REM_E_STAT_SET, EState);
		else
			registration_state_change (ereg, ereg_set_registered);
                break;
        case ereg_unset_registered:
                registration_state_change (ereg, ereg_set_registered);

                break;
        default:
                MIN_WARN ("%s unhandled state %s", __FUNCTION__,
                             regstate_str[ereg->regstate_]);
                return;
        }
}
/* ------------------------------------------------------------------------- */
/** Add event to the list of indication events
 *  @param param the description of event to be added
 *  @returns a pointer to the event or INITPTR if event by that name exists
 */
LOCAL min_event_t *add_ind_event (minTestEventParam_t * param)
{
        min_event_t   *e;

        if (param->event.event_type_ != EIndication) {
                MIN_WARN ("invalid event type");
                return INITPTR;
        }
        e = NEW (min_event_t);
        e->registrations_ = dl_list_create ();
        e->descr_ = param->event;
        param->event.event_name_ = NULL;
        dl_list_add (ind_events, e);

        return e;
}
/* ------------------------------------------------------------------------- */
/** Add event to the list of state events
 *  @param param the description of event to be added
 *  @returns a pointer to the event or INITPTR if event by that name exists
 */
LOCAL min_event_t *add_state_event (minTestEventParam_t * param)
{
        min_event_t   *e;

        if (param->event.event_type_ != EState) {
                MIN_WARN ("%s: invalid event type", __FUNCTION__);
                return INITPTR;
        }
        e = NEW (min_event_t);
        if (e == NULL) {
                MIN_FATAL ("%s OOM situation!");
                return INITPTR;
        }
        e->registrations_ = dl_list_create ();
        e->state_ = EState_UNSET;
        e->descr_ = param->event;
        param->event.event_name_ = NULL;

        dl_list_add (state_events, e);

        return e;
}
/* ------------------------------------------------------------------------- */
/**  Adds a registration for the event
 *   @param event the event to be registered for
 *   @param registrar the pid of registrating process
 *   @returns pointer to event registration entry
 */
LOCAL e_registration_t *event_add_registration (min_event_t * event,
                                                minEventSrc_t * registrar)
{
        e_registration_t *reg;

        reg = NEW (e_registration_t);
        reg->registrar_ = *registrar;
        reg->regstate_ = ereg_unset_registered;
        reg->backp_ = event;
        dl_list_add (event->registrations_, reg);
        return reg;

}
/* ------------------------------------------------------------------------- */
/**  Removes a registration from the event
 *   @param event the event to be unregistered for
 *   @param registrar the pid of unregistrating process
 *   @returns 0 if the registration is found and removed, 
 *            1 if no registration is found.
 */
LOCAL int
event_del_registration (min_event_t * event, minEventSrc_t * registrar)
{
        DLListIterator  it, next_it;
        e_registration_t *reg;

        it = dl_list_head (event->registrations_);
        while (it != DLListNULLIterator) {
                next_it = dl_list_next (it);
                reg = (e_registration_t *) dl_list_data (it);
                if (reg->registrar_.remote ==  registrar->remote &&
                    reg->registrar_.pid == registrar->pid) {
                        dl_list_remove_it (it);
                        MIN_DEBUG ("removing registration for %d", 
                                     registrar->pid);

                        DELETE (reg);

                        return 0;
                }
                it = next_it;
        }

        return 1;
}
/* ------------------------------------------------------------------------- */
/** Searches for indication event
 *  @param param the event describtion of the event to be searched
 *  @param [IN/OUT] it used to return also the address of the list iterator
 *  @returns pointer to the event if found, INITPTR if not 
 */
LOCAL min_event_t *find_ind_event (minTestEventParam_t * param,
                                    DLListIterator * itp)
{
        DLListIterator  it;
        it = dl_list_find (ind_events->head_, ind_events->tail_, event_comp,
                           (void *)param);
        if (itp != NULL)
                *itp = it;
        return (min_event_t *) dl_list_data (it);
}
/* ------------------------------------------------------------------------- */
/** Searches for state event
 *  @param param the event describtion of the event to be searched
 *  @param [IN/OUT] it used to return also the address of the list iterator
 *  @returns pointer to the event if found, INITPTR if not 
 */
LOCAL min_event_t *find_state_event (minTestEventParam_t * param,
                                      DLListIterator * itp)
{
        DLListIterator  it;
        it = dl_list_find (state_events->head_, state_events->tail_,
                           event_comp, (void *)param);
        if (itp != NULL)
                *itp = it;
        return (min_event_t *) dl_list_data (it);
}

LOCAL void free_event (void *event)
{
        dl_list_free_data (&((min_event_t *) (event))->registrations_);
        dl_list_free (&((min_event_t *) (event))->registrations_);

        DELETE (((min_event_t *) (event))->descr_.event_name_);
        DELETE (event);
}
/* ------------------------------------------------------------------------- */
/** Delete event from the list of events
 *  @param it the list iterator containing the event
 *  @param event event to removed
 */
LOCAL void del_event (DLListIterator it, min_event_t * event)
{
        if (dl_list_data (it) != event) {
                MIN_FATAL ("Fatal pointer error");
                return;
        }
        dl_list_remove_it (it);
        free_event ((void *)event);

        return;
}
/* ------------------------------------------------------------------------- */
/** Sends EVENT_IND IPC message to TMC with pid mathing the argument
 *  @param receiver pid of the receiving process
 *  @param status ok/err to be informed to TP 
 */
LOCAL void send_event_ind (minEventSrc_t * receiver, int status)
{
        MsgBuffer       message;

        if (receiver->remote == 0) {
                MIN_DEBUG ("Sending EVENT_IND to %d\n", receiver->pid);
                message.type_ = MSG_EVENT_IND;
                message.sender_ = getpid ();
                message.receiver_ = receiver->pid;
                message.param_ = status;
                STRCPY (message.desc_, "\0", MaxDescSize);
                STRCPY (message.message_, "\0", MaxMsgSize);
                mq_send_message (mq_id, &message);
        } else
                MIN_DEBUG ("remote event (status %d)", status);
}

/* ------------------------------------------------------------------------- */
/** Build and send remote event request response
 *  @param eventname name of the event
 *  @param status set, active, error
 *  @param etype state or indication
 */
LOCAL void
remote_event_req_response (char *eventname, int status, TEventType_t etype)
{
        char           *buff;

        buff =
            NEW2 (char,
                  strlen ("remote request active type state") +
                  MaxMinEventName);

        sprintf (buff, "remote request %s %s",
                 status == REM_E_STAT_SET ? "set" :
                 (status == REM_E_STAT_ACTIVE ? "active" : "error"),
                 eventname);
        if (etype == EState)
                sprintf (buff + strlen (buff), " type=state");

        send_to_master (0, buff);

        DELETE (buff);
}
/* ------------------------------------------------------------------------- */
/** Build and send remote event release response
 *  @param eventname name of the event
 *  @param status set, active, error
 */
LOCAL void remote_event_rel_response (char *eventname, int status)
{
        char           *buff;

        buff = NEW2 (char, strlen ("remote release ") + MaxMinEventName);

        sprintf (buff, "remote release %s", eventname);

        send_to_master (0, buff);

        DELETE (buff);

}

/* ======================== FUNCTIONS ====================================== */

/* ------------------------------------------------------------------------- */
/** Initializes the Event System on engine. 
 *  Create lists of indication and state events.
 */
void event_system_init (void)
{
        ind_events = dl_list_create ();
        state_events = dl_list_create ();
}
/* ------------------------------------------------------------------------- */
/** See if event System is initialized
 *  @returns 1 if event system is initialize, 0 if not
 */
int event_system_up (void)
{
        if (ind_events == INITPTR)
                return 0;
        if (state_events == INITPTR)
                return 0;
        return 1;

}
/* ------------------------------------------------------------------------- */
/** Removes the Event System from the engine. 
 *  Delete lists of indication and state events.
 */
void event_system_cleanup (void)
{
        if (dl_list_size (ind_events) > 0)
                dl_list_foreach (ind_events->head_, ind_events->tail_,
                                 free_event);
        dl_list_free (&ind_events);

        if (dl_list_size (state_events) > 0)
                dl_list_foreach (state_events->head_, state_events->tail_,
                                 free_event);
        dl_list_free (&state_events);
}
/* ------------------------------------------------------------------------- */
/** Handles a SET for Indication Event.
 *  @param param contains the event description
 *  @param event_src pid of the setting process.
 *  @returns 0 on ok, 1 on error
 */
int
ind_event_handle_set (minTestEventParam_t * param,
                      minEventSrc_t * event_src)
{
        min_event_t   *e;
        DLListIterator  it;
        e = find_ind_event (param, &it);
        if (e == INITPTR) {
#ifndef MIN_UNIT_TEST
                send_event_ind (event_src, EventStatOK);
#endif
                return 1;

        }

        dl_list_foreach (e->registrations_->head_, e->registrations_->tail_,
                         do_set_on_ind_reg);
#ifndef MIN_UNIT_TEST
        send_event_ind (event_src, EventStatOK);
#endif
        return 0;
}
/* ------------------------------------------------------------------------- */
/** Handles a WAIT for Indication Event.
 *  @param param contains the event description
 *  @param event_src pid of the waiting process.
 *  @returns 0 on ok, 1 on error
 */
int
ind_event_handle_wait (minTestEventParam_t * param,
                       minEventSrc_t * event_src, int *status)
{
        min_event_t   *e;
        e_registration_t *ereg = INITPTR;

        e = find_ind_event (param, NULL);
        if (e == INITPTR) {
                MIN_WARN ("Indication event: no event %s found to wait\n",
                           param->event.event_name_);
                goto iehw_err_out;
        }

        ereg = event_find_registration (e, event_src, NULL);

        if (ereg == INITPTR) {
                MIN_WARN ("Indication event: no regisration found %d for "
                           "event %s\n", event_src->pid,
                           param->event.event_name_);
                goto iehw_err_out;

        }

        switch (ereg->regstate_) {
        case ereg_set_registered:
#ifndef MIN_UNIT_TEST
                send_event_ind (&ereg->registrar_, EventStatOK);
#endif
                *status = REM_E_STAT_SET;
                registration_state_change (ereg, ereg_unset_registered);
                break;
        case ereg_unset_registered:
                registration_state_change (ereg, ereg_waiting_registered);
                break;
        default:
                MIN_WARN ("%s unhandled state %s", __FUNCTION__,
                           regstate_str[ereg->regstate_]);
                goto iehw_err_out;
                break;

        }
        return 0;
      iehw_err_out:
#ifndef MIN_UNIT_TEST
        send_event_ind (event_src, EventStatErr);
#endif

        return 1;
}
/* ------------------------------------------------------------------------- */
/** Handles a REQUEST for Indication Event.
 *  @param param contains the event description
 *  @param event_src pid of the registering event.
 *  @returns 0 if the request can be handled.
 */
int
ind_event_handle_request (minTestEventParam_t * param,
                          minEventSrc_t * event_src)
{
        min_event_t   *e = INITPTR;
        e = find_ind_event (param, NULL);
        if (e == INITPTR) {
                e = add_ind_event (param);
        } else {
                if (event_is_registered (e, event_src)) {
                        MIN_WARN ("process %d already registered for "
                                     "event %s\n", event_src->pid,
                                     param->event.event_name_);
#ifndef MIN_UNIT_TEST
                        send_event_ind (event_src, EventStatErr);
#endif
                        return 1;
                }
        }

        event_add_registration (e, event_src);

#ifndef MIN_UNIT_TEST
        send_event_ind (event_src, EventStatOK);
#endif


        return 0;
}
/* ------------------------------------------------------------------------- */
/** Handles a RELEASE of Indication Event.
 *  @param param contains the event description
 *  @param event_src pid of the releasing event.
 *  @returns 0 if the release can be handled.
 */
int
ind_event_handle_release (minTestEventParam_t * param,
                          minEventSrc_t * event_src)
{
        min_event_t   *e = INITPTR;
        DLListIterator  it;
        e = find_ind_event (param, &it);
        if (e == INITPTR) {
                MIN_WARN ("no event %s found\n",
                           param->event.event_name_);
#ifndef MIN_UNIT_TEST
                send_event_ind (event_src, EventStatErr);
#endif
                return 1;
        }
        if (!event_is_registered (e, event_src)) {
                MIN_WARN ("process %d not registered for event %s\n",
                           event_src->pid, param->event.event_name_);
#ifndef MIN_UNIT_TEST
                send_event_ind (event_src, EventStatErr);
#endif
                return 1;
        }

        event_del_registration (e, event_src);

        if (dl_list_size (e->registrations_) == 0)
                del_event (it, e);

#ifndef MIN_UNIT_TEST
        send_event_ind (event_src, EventStatOK);
#endif

        return 0;
}
/* ------------------------------------------------------------------------- */
/** Handles a SET for State Event.
 *  @param param contains the event description
 *  @param event_src pid of the setting process.
 *  @returns 0 on ok, 1 on error
 */
int
state_event_handle_set (minTestEventParam_t * param,
                        minEventSrc_t * event_src)
{
        min_event_t   *e;
        DLListIterator  it;
        e = find_state_event (param, &it);
        if (e == INITPTR) {
                e = add_state_event (param);
        }
        if (e->state_ != EState_UNSET) {
                MIN_DEBUG ("State Event %s in state %s\n",
                            e->descr_.event_name_,
                            (e->state_ ==
                             EState_SET) ? "SET" : "UNSET PENDING");
#ifndef MIN_UNIT_TEST
                send_event_ind (event_src, EventStatOK);
#endif
                return 1;
        }
        e->state_ = EState_SET;
        dl_list_foreach (e->registrations_->head_, e->registrations_->tail_,
                         do_set_on_state_reg);

#ifndef MIN_UNIT_TEST
        send_event_ind (event_src, EventStatOK);
#endif

        return 0;
}
/* ------------------------------------------------------------------------- */
/** Handles a UNSET for State Event.
 *  @param param contains the event description
 *  @param event_src pid of the setting process.
 *  @returns 0 on ok, 1 on error
 */
int
state_event_handle_unset (minTestEventParam_t * param,
                          minEventSrc_t * event_src)
{
        min_event_t   *e;
        DLListIterator  it;
        e = find_state_event (param, &it);

        if (e == INITPTR) {
                MIN_WARN ("%s no State Event %s found\n", __FUNCTION__,
                             param->event.event_name_);

                goto sehu_err_out;
        }

        if (e->state_ != EState_SET) {
                MIN_WARN ("%s State Event %s in state %s\n",
                             __FUNCTION__,
                             param->event.event_name_,
                             (e->state_ ==
                              EState_UNSET) ? "UNSET" : "UNSET PENDING");
                goto sehu_err_out;
        }

        if (dl_list_size (e->registrations_)) {
                MIN_WARN ("State Event %s still has %d registrations "
                             "UNSET pending",
                             param->event.event_name_,
                             dl_list_size (e->registrations_));
                e->state_ = EState_UNSET_pending;
                e->controller_ = *event_src;
        } else {
#ifndef MIN_UNIT_TEST
                send_event_ind (event_src, EventStatOK);
#endif
                del_event (it, e);
        }
        return 0;
      sehu_err_out:
#ifndef MIN_UNIT_TEST
        send_event_ind (event_src, EventStatErr);
#endif
        return 1;
}
/* ------------------------------------------------------------------------- */
/** Handles a WAIT for State Event.
 *  @param param contains the event description
 *  @param event_src pid of the waiting process.
 *  @returns 0 on ok, 1 on error
 */
int
state_event_handle_wait (minTestEventParam_t * param,
                         minEventSrc_t * event_src, int *status)
{
        min_event_t   *e;
        e_registration_t *ereg;

        e = find_state_event (param, NULL);
        if (e == INITPTR) {
                MIN_WARN ("State event: no event %s found to wait\n",
                           param->event.event_name_);
                goto sehw_err_out;
        }

        ereg = event_find_registration (e, event_src, NULL);

        if (ereg == INITPTR) {
                MIN_WARN ("State event: no regisration found %d for "
                           "event %s\n", event_src,
                           param->event.event_name_);
                goto sehw_err_out;
        }

        if (ereg->regstate_ != ereg_unset_registered &&
            ereg->regstate_ != ereg_set_registered) {
                MIN_WARN ("State event: %d already waiting for event %s\n",
                           event_src->pid, param->event.event_name_);
                goto sehw_err_out;
        }

        switch (e->state_) {
        case EState_SET:
        case EState_UNSET_pending:
#ifndef MIN_UNIT_TEST
                send_event_ind (&ereg->registrar_, EventStatOK);
#endif
                *status = REM_E_STAT_SET;
                registration_state_change (ereg, ereg_set_registered);
                break;
        case EState_UNSET:
                registration_state_change (ereg, ereg_waiting_registered);
                break;
        default:
                MIN_WARN ("%s unhandled state %d\n", __FUNCTION__);
                goto sehw_err_out;
                break;
        }
        return 0;
      sehw_err_out:
#ifndef MIN_UNIT_TEST
        send_event_ind (event_src, EventStatErr);
#endif
        return 1;
}
/* ------------------------------------------------------------------------- */
/** Handles a REQUEST for State Event.
 *  @param param contains the event description
 *  @param event_src pid of the registering event.
 *  @returns 0 if the request can be handled.
 */
int
state_event_handle_request (minTestEventParam_t * param,
                            minEventSrc_t * event_src)
{
        min_event_t   *e = INITPTR;
        e_registration_t *ereg;

        e = find_state_event (param, NULL);
        if (e == INITPTR) {
                e = add_state_event (param);
        } else {
                if (event_is_registered (e, event_src)) {
                        MIN_WARN ("process %d already registered for "
                                   "event %s\n", event_src->pid,
                                   param->event.event_name_);
#ifndef MIN_UNIT_TEST
                        send_event_ind (event_src, EventStatErr);
#endif
                        return 1;
                }
        }

        ereg = event_add_registration (e, event_src);
        if (e->state_ == EState_SET) {
                registration_state_change (ereg, ereg_set_registered);
        }
#ifndef MIN_UNIT_TEST
        send_event_ind (event_src, EventStatOK);
#endif

        return 0;
}
/* ------------------------------------------------------------------------- */
/** Handles a RELEASE of State Event.
 *  @param param contains the event description
 *  @param event_src pid of the releasing event.
 *  @returns 0 if the release can be handled.
 */
int
state_event_handle_release (minTestEventParam_t * param,
                            minEventSrc_t * event_src)
{
        min_event_t   *e = INITPTR;
        DLListIterator  it;
        e = find_state_event (param, &it);
        if (e == INITPTR) {
                MIN_WARN ("%s: no event %s found\n",
                           __FUNCTION__, param->event.event_name_);

#ifndef MIN_UNIT_TEST
                send_event_ind (event_src, EventStatErr);
#endif
                return 1;
        }
        if (!event_is_registered (e, event_src)) {
                MIN_WARN ("process %d not registered for event %s\n",
                           event_src->pid, param->event.event_name_);

#ifndef MIN_UNIT_TEST
                send_event_ind (event_src, EventStatErr);
#endif
                return 1;
        }

        event_del_registration (e, event_src);

        if (dl_list_size (e->registrations_) == 0 && e->state_ != EState_SET) {
                if (e->state_ == EState_UNSET_pending)
                        send_event_ind (&e->controller_, EventStatOK);
                del_event (it, e);
        }
#ifndef MIN_UNIT_TEST
        send_event_ind (event_src, EventStatOK);
#endif
        return 0;
}
/* ------------------------------------------------------------------------- */
/** Handles remote event commands arriving over external controller
 *  @param command Event command
 *  @param parameters command parameters
 */
int
handle_remote_event (TScripterKeyword command, MinItemParser * parameters)
{
        char           *token = INITPTR, *token2 = INITPTR;
        int             status = 0;
        minTestEventParam_t param;
        minEventSrc_t  event_src;
        min_event_t   *e;

        event_src.pid = -1;
        event_src.remote = 1;

        mip_get_next_string (parameters, &token);

        if (token == INITPTR) {
                MIN_WARN ("%s: missing event name", __FUNCTION__);
                goto err_out;
        }

        param.event.event_name_ = NEW2 (char, MaxMinEventName + 1);
        STRCPY (param.event.event_name_, token, MaxMinEventName);

        switch (command) {
        case EKeywordRequest:
                param.command_ = EReqEvent;


                if (mip_get_next_tagged_string (parameters, "type", &token2)
                    == ENOERR) {
                        if (strcmp (token2, "state"))
                                MIN_WARN ("unknown type=%s", token2);
                        else {
                                param.event.event_type_ = EState;
                                status =
                                    state_event_handle_request (&param,
                                                                &event_src);
                                if (!status) {
                                        /*
					 * Remote request is 
					 * also implicit wait
                                         */
                                        param.command_ = EWaitEvent;
                                        if (!param.event.event_name_) {
                                                param.event.event_name_ =
                                                NEW2 (char, MaxMinEventName + 1);
                                                STRCPY (param.event.event_name_,
                                                        token, 
                                                        MaxMinEventName);
                                        }
                                        state_event_handle_wait (&param,
                                                                 &event_src,
                                                                 &status);
                                }

                        }
                } else {
                        param.event.event_type_ = EIndication;
                        status =
                            ind_event_handle_request (&param, &event_src);
                        if (!status) {
                                param.command_ = EWaitEvent;
                                if (!param.event.event_name_) {
                                        param.event.event_name_ =
                                            NEW2 (char, MaxMinEventName + 1);
                                        STRCPY (param.event.event_name_,
                                                token, MaxMinEventName);
                                }
                                ind_event_handle_wait (&param, &event_src,
                                                       &status);
                        }
                }

                remote_event_req_response (token, status,
                                           param.event.event_type_);
                break;
        case EKeywordRelease:
                param.command_ = ERelEvent;
                /*
                 ** The protocol is brain damaged...
                 ** We need to first see if the release is for 
                 ** state or indication event
                 */
                e = find_state_event (&param, NULL);
                if (e != INITPTR) {
                        param.event.event_type_ = EState;
                        status =
                            state_event_handle_release (&param, &event_src);
                } else {
                        param.event.event_type_ = EIndication;
                        status =
                            ind_event_handle_release (&param, &event_src);
                }
                remote_event_rel_response (token, status);
                break;
        case EKeywordSet:
                param.command_ = ESetEvent;
                if (mip_get_next_tagged_string (parameters, "type", &token2)
                    == ENOERR) {
                        if (strcmp (token2, "state"))
                                MIN_WARN ("%s: unknown type=%s",
                                             __FUNCTION__, token2);
                        else {
                                param.event.event_type_ = EState;
                                state_event_handle_set (&param, &event_src);
                        }
                } else {
                        param.event.event_type_ = EIndication;
                        ind_event_handle_set (&param, &event_src);
                }

                break;
        case EKeywordUnset:
                param.command_ = EUnsetEvent;
                break;
        default:
                break;
        }

        DELETE (param.event.event_name_);
	if (token != INITPTR)
		DELETE (token);
	if (token2 != INITPTR)
		DELETE (token2);

        return 0;
      err_out:
        return 1;
}


/** Handles remote event request response commands arriving 
 *  from external controller
 *  @param parameters command parameters
 */
int handle_remote_event_request_resp (MinItemParser * parameters)
{
        char           *token = NULL, *token2 = NULL, *token3 = NULL;
        minTestEventParam_t param;
        minEventSrc_t  event_src;

        event_src.pid = -1;
        event_src.remote = 1;

        if (mip_get_next_string (parameters, &token) != ENOERR) {
                MIN_WARN ("%s: missing parameters!", __FUNCTION__);
                return 1;
        }

        if (!strcasecmp (token, "active")) {
                DELETE (token);
                return 0;       /* Just an ack, no need to handle further */
        }

        if (!strcasecmp (token, "set")) {

                if (mip_get_next_string (parameters, &token2) != ENOERR) {
			DELETE (token);
                        MIN_WARN ("%s: event name missing!", __FUNCTION__);
                        return 1;
                }
                param.event.event_name_ = NEW2 (char, MaxMinEventName + 1);
                STRCPY (param.event.event_name_, token2, MaxMinEventName);

                if (mip_get_next_tagged_string (parameters, "type", &token3)
                    == ENOERR) {
                        if (strcmp (token3, "state"))
                                MIN_WARN ("%s: unknown type=%s",
                                             __FUNCTION__, token3);
                        else {
                                param.event.event_type_ = EState;
                                state_event_handle_set (&param, &event_src);
                        }
                } else {
                        param.event.event_type_ = EIndication;
                        ind_event_handle_set (&param, &event_src);
                }
                DELETE (param.event.event_name_);
        } else {
                MIN_WARN ("%s: unhandled: %s", __FUNCTION__, token);
        }

        DELETE (token);
        DELETE (token2);
        DELETE (token3);
        return 0;
}


/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */


/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "tec_events.tests"
#endif                          /* MIN_UNIT_TEST */

/* End of file */
