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
 *  @file       min_test_event_if.h
 *  @version    0.1
 *  @brief      This file contains header file of the MIN Event interface
 */

#ifndef MIN_EVENT_INTERFACE_H
#define MIN_EVENT_INTERFACE_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* ------------------------------------------------------------------------- */
/** Maximum length of event Name */
#define MaxMinEventName MaxUsrMessage
/** Event status after Event() from engine : ok  */
#define EventStatOK   0
/** Event status after Event() from engine : error  */
#define EventStatErr  1
/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* ------------------------------------------------------------------------- */
/** Event Request Types */
typedef enum {
        EReqEvent = 1,
        EWaitEvent,
        ERelEvent,
        ESetEvent,
        EUnsetEvent,
        EEnable
} TEventReq_t;
/* ------------------------------------------------------------------------- */
/** Event Types */
typedef enum {
        EIndication = 1,
        EState
} TEventType_t;
/* ------------------------------------------------------------------------- */
/** Event name type */
typedef char   *TEventName_t;
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* Structures */

typedef struct min_event_if_ {
       /**
        * Get request type.
        */
        TEventReq_t (*Type) (struct min_event_if_ * e);

        /** 
         * Get event name.
         */
        TEventName_t (*Name) (struct min_event_if_ * e);

        /**
         * Get event type.
         */
        TEventType_t (*EventType) (struct min_event_if_ * e);

        /**
         * Set request type.
         */
        void            (*SetType) (struct min_event_if_ * e,
                                    TEventReq_t aType);

        /** 
	 * Set event name.
	 */
        void            (*SetName) (struct min_event_if_ * e,
                                    TEventName_t aName);

        /**
	 * Set event type.
	 */
        void            (*SetEventType) (struct min_event_if_ * e,
                                         TEventType_t aEventType);

        /**
	 * Set request type, event name and event type.
	 */
        void            (*Set) (struct min_event_if_ * e, TEventReq_t aType,
                                TEventName_t aName, TEventType_t aEventType);


        /* private */
        /** Type of the event; state or indication */
        TEventType_t    event_type_;
        /** Name of the event */
        TEventName_t    event_name_;
        /** Event Request type; SET, UNSET .. */
        TEventReq_t     event_req_type_;
        /** Event Status after Event() */
        int             event_status_;
        /** Set if we can not block in event wait */
        int             dont_block_;
} minEventIf;

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */

/** Creates a MIN event.
 *  @param aName name of the event (can be NULL) 
 *  @param aType type of the event (state or indication)
 *  @returns a pointer to the created event,
 */
minEventIf    *min_event_create (const TEventName_t aName,
                                   TEventType_t aType);
/** MIN Event handling for TP
 *  @param event_param event description and command
 *
 *  This function
 *  - validates the event 
 *  - sends event related message to engine
 *  - waits for SIGUSR (blocking)
 */
void            Event (minEventIf * event);

/** Destroys an event. The memory allocated for the event is freed
 * @param event pointer to the event to be deleted
 */
void            min_event_destroy (minEventIf * event);


/* ------------------------------------------------------------------------- */


#endif                          /* MIN_EVENT_INTERFACE_H */
