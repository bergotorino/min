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
 *  @file       tec_events.h
 *  @version    0.1
 *  @brief      This is header file for tec_events.c
 */

#ifndef TEC_EVENTS_H
#define TEC_EVENTS_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <unistd.h>
#include <min_test_event_if.h>
#include "scripter_keyword.h"

/* ------------------------------------------------------------------------- */
/* CONSTANTS */

/* ------------------------------------------------------------------------- */
/* MACROS */
#define REM_E_STAT_ACTIVE 0 /** Remote event status active */
#define REM_E_STAT_ERROR  1 /** Remote event status error */
#define REM_E_STAT_SET    2 /** Remote event status set */
/* ------------------------------------------------------------------------- */
/* DATA TYPES */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */

/* ------------------------------------------------------------------------- */
/* Structures */
/** MIN test event description. */
typedef struct {
        TEventType_t    event_type_;
        TEventName_t    event_name_;
} minTestEventDescr_t;
/** Parameter for the event system */
typedef struct {
        TEventReq_t     command_;
        minTestEventDescr_t event;
} minTestEventParam_t;
/** Source for event: "remote" or pid of the test process. */
typedef struct {
        unsigned char   remote;
        pid_t           pid;
} minEventSrc_t;


/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
void            event_system_init (void);
/* ------------------------------------------------------------------------- */
int             int_system_up (void);
/* ------------------------------------------------------------------------- */
int             ind_event_handle_set (minTestEventParam_t * param,
                                      minEventSrc_t * event_src);
/* ------------------------------------------------------------------------- */
int             ind_event_handle_wait (minTestEventParam_t * param,
                                       minEventSrc_t * event_src, int *status
				       );
/* ------------------------------------------------------------------------- */
int             ind_event_handle_request (minTestEventParam_t * param,
                                          minEventSrc_t * event_src);
/* ------------------------------------------------------------------------- */
int             ind_event_handle_release (minTestEventParam_t * param,
                                          minEventSrc_t * event_src);
/* ------------------------------------------------------------------------- */
int             state_event_handle_set (minTestEventParam_t * param,
                                        minEventSrc_t * event_src);
/* ------------------------------------------------------------------------- */
int             state_event_handle_unset (minTestEventParam_t * param,
                                          minEventSrc_t * event_src);
/* ------------------------------------------------------------------------- */
int             state_event_handle_wait (minTestEventParam_t * param,
                                         minEventSrc_t * event_src,
                                         int *status);
/* ------------------------------------------------------------------------- */
int             state_event_handle_release (minTestEventParam_t * param,
                                            minEventSrc_t * event_src);
/* ------------------------------------------------------------------------- */
int             state_event_handle_request (minTestEventParam_t * param,
                                            minEventSrc_t * event_src);
/* ------------------------------------------------------------------------- */
int             handle_remote_event (TScripterKeyword command,
                                     MinItemParser * parameters);
/* ------------------------------------------------------------------------- */
int             handle_remote_event_request_resp (MinItemParser *
                                                  parameters);
/* ------------------------------------------------------------------------- */
#endif                          /* TEC_EVENTS_H */

/* End of file */
