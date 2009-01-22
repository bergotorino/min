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
 *  @file       min_test_event_if.c
 *  @version    0.1
 *  @brief      This file contains implementation of Event System for 
 *              Test Process
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <signal.h>
#include <errno.h>
#include <tmc.h>
#include <tmc_ipc.h>
#include <min_logger.h>
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

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
static int      mqid = -1;

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */

/** Check that the event paramter seems valid.
 *  @param e event  to be validate
 *  @return 0 if the event is valid, 1 if not
 */
LOCAL int       validate_event (minEventIf * e);
/** Sends event request towards the engine.
 *  @param event the event.
 */
LOCAL void      send_event (minEventIf * e);

/** Get the event request type from the event,
 * @param e the event
 * @returns the event request type
 */
inline LOCAL TEventReq_t get_event_req_type (minEventIf * e);

/** Set the event request type for the event.
 * @param e the the event
 * @param aReqType the event request type
 */
inline LOCAL void set_event_req_type (minEventIf * e, TEventReq_t aReqType);

/** Get the name from event.
 *  @param e the event name of which is to be returned 
 *  @returns event name or INITPTR if name is not set for the event
 */
inline LOCAL TEventName_t get_event_name (minEventIf * e);

/** Set the name for the event.
 * @param e the event
 * @param aName event name
 */
inline LOCAL void set_event_name (minEventIf * e, TEventName_t aName);

/** Get event type from event
 *  @param e the event
 *  @returns event type
 */
inline LOCAL TEventType_t get_event_type (minEventIf * e);

/** Set the event type for event.
 *  @param e the event
 *  @param aType the event type
 */
inline LOCAL void set_event_type (minEventIf * e, TEventType_t aType);

/** Set request type, name and even type for event.
 *  @param e the event
 *  @param aType event requst type
 *  @param aName event name
 *  @param aEventType event type
 */
LOCAL void      set_event_stuff (minEventIf * e, TEventReq_t aType,
                                 TEventName_t aName, TEventType_t aEventType);


/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */

int validate_event (minEventIf * e)
{

        if (!e || e == INITPTR)
                goto invalid_event;

        if (e->event_type_ != EIndication && e->event_type_ != EState)
                goto invalid_event;

        if (!e->event_name_ ||
            e->event_name_ == INITPTR || strlen (e->event_name_) == 0)
                goto invalid_event;

        if (e->event_req_type_ < EReqEvent
            || e->event_req_type_ > EUnsetEvent)
                goto invalid_event;

        return 0;
      invalid_event:
        return 1;
}

/* ------------------------------------------------------------------------- */

LOCAL void send_event (minEventIf * e)
{
        int             retval = -1;
        MsgBuffer       buff;

        buff.receiver_ = getppid ();
        buff.sender_ = getpid ();
        buff.special_ = getpid ();
        buff.type_ = MSG_EVENT;
        buff.param_ = e->event_req_type_ | (e->event_type_ << 8);

        STRCPY (buff.message_, e->event_name_, MaxUsrMessage);

        if (mqid == -1)
                mqid = mq_open_queue ('a');
        if (mqid != -1) {
                retval = 0;
                retval = mq_send_message_block (mqid, &buff);
        }
}

LOCAL void wait_event (minEventIf * e)
{
        int             retval;
        MsgBuffer       msg;

      again:
        retval = mq_read_message (mqid, getpid ()
                                  , &msg);
        if (retval == -1) {
                switch (errno) {
                case EINTR:
                        MIN_WARN ("Reading MQ interrupted by signal");
                        goto again;
                        return;

                case EIDRM:
                        MIN_FATAL ("MQ id removed from the system");
                        return;

                case E2BIG:
                        MIN_WARN ("Recieved message too big");
                        return;

                case EINVAL:
                        MIN_ERROR ("Invalid value: mq_read_message");
                        return;
                }
        } else if (retval < 8) {
                /* ignore too short messages */
                MIN_WARN ("Recieved message is too small");
                goto again;
        }

        if (msg.type_ == MSG_EVENT_IND) {
                e->event_status_ = msg.param_;
        } else {
                MIN_WARN ("%s: unexpected message type %d", __FUNCTION__,
                             msg.type_);
                goto again;
        }
        return;
}


inline LOCAL TEventReq_t get_event_req_type (minEventIf * e)
{
        return e->event_req_type_;
}

inline LOCAL void set_event_req_type (minEventIf * e, TEventReq_t aReqType)
{
        e->event_req_type_ = aReqType;
}

inline LOCAL TEventName_t get_event_name (minEventIf * e)
{
        return e->event_name_;
}

inline LOCAL void set_event_name (minEventIf * e, TEventName_t aName)
{
        if (e->event_name_ != INITPTR) {
                DELETE (e->event_name_);
                e->event_name_ = NULL;
        }
        e->event_name_ = NEW2 (char, strlen (aName) + 1);
        STRCPY (e->event_name_, aName, strlen (aName) + 1);
}

inline LOCAL TEventType_t get_event_type (minEventIf * e)
{
        return e->event_type_;
}

inline LOCAL void set_event_type (minEventIf * e, TEventType_t aType)
{
        e->event_type_ = aType;
        return;
}


LOCAL void set_event_stuff (minEventIf * e, TEventReq_t aType,
                            TEventName_t aName, TEventType_t aEventType)
{
        set_event_type (e, aEventType);
        set_event_name (e, aName);
        set_event_req_type (e, aType);
}

/* ======================== FUNCTIONS ====================================== */

minEventIf    *min_event_create (const TEventName_t aName,
                                   TEventType_t aType)
{
        minEventIf    *event;

        event = NEW (minEventIf);
        event->event_name_ = INITPTR;
        event->event_status_ = EventStatOK;
        event->dont_block_ = 0;

        if (!aType)
                event->event_type_ = EIndication;
        else
                event->event_type_ = aType;

        if (aName != NULL)
                set_event_name (event, aName);
        else
                event->event_name_ = INITPTR;

        event->event_req_type_ = EEnable;

        event->Type = get_event_req_type;
        event->Name = get_event_name;
        event->EventType = get_event_type;
        event->SetType = set_event_req_type;
        event->SetName = set_event_name;
        event->SetEventType = set_event_type;
        event->Set = set_event_stuff;

        return event;
}

/* ------------------------------------------------------------------------- */

void min_event_destroy (minEventIf * event)
{
        if (event->event_name_ && event->event_name_ != INITPTR)
                DELETE (event->event_name_);
        DELETE (event);
}

/* ------------------------------------------------------------------------- */

void Event (minEventIf * e)
{
        sigset_t        waitset;

        /* 1) see that the event seems ok */
        if (validate_event (e)) {
                MIN_WARN ("invalid event\n");
                return;
        }
        sigemptyset (&waitset);
        sigaddset (&waitset, SIGUSR1);
        sigprocmask (SIG_BLOCK, &waitset, NULL);

        /* 2) send the event to engine */
        send_event (e);

        /* 3) wait for the event */
        if (!e->dont_block_)    /* unless dont_block_ is set (the scripter) */
                wait_event (e);

        return;
}

/* ------------------------------------------------------------------------- */

/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */


/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "min_test_event_if.tests"
#endif                          /* MIN_UNIT_TEST */

/* End of file */
