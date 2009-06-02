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
 *  @file       min_ipc_mechanism.h
 *  @version    0.1
 *  @brief      This file contains header file of the IPC Mechanism of MIN
 */

#ifndef MIN_IPC_MECHANISM_H
#define MIN_IPC_MECHANISM_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <sys/ipc.h>            /*message queues stuff */
#include <sys/msg.h>            /*ftok */
#include <sys/shm.h>            /*shared memory */
#include <unistd.h>             /*getppid,getpid */
#include <errno.h>              /*errno */
#include <stdlib.h>
#include <string.h>             /*strncpy */
#include <stdarg.h>             /*variable length of arguments */
#include <signal.h>             /*signal definitions */
#include <sys/types.h>          /*kill */

#include <min_common.h>
#include <min_logger.h>
#include <dllist.h>

/* ------------------------------------------------------------------------- */
/* CONSTANTS */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */
/** Max length of the description field */
#define MaxDescSize   MAX4( MaxTestResultDescription    \
                          , MaxTestCaseName             \
                          , MaxFileName                 \
                          , MaxUsrMessage )
/** Max length of the message field */
#define MaxMsgSize    MAX4( MaxTestResultDescription    \
                          , MaxTestCaseName             \
                          , MaxFileName                 \
                          , MaxUsrMessage )
/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/** Message types which are available in communication between MIN components
 */
typedef enum {
        MSG_OK = 1,               /**< ( E<---G ) All OK                     */
        MSG_KO = 2,               /**< ( E<---G ) Sth went wrong             */
        MSG_GTC = 3,              /**< ( E--->G ) Get Test Cases             */
        MSG_TCD = 5,              /**< ( E<---G ) Test Case Details          */
        MSG_EXE = 7,              /**< ( E--->G ) Execute test               */
        MSG_RET = 11,             /**< ( E<---G ) Retval                     */
        MSG_PAUSE = 13,           /**< ( E--->G ) Pause test                 */
        MSG_RESUME = 17,          /**< ( E--->G ) Resume test                */
        MSG_STOP = 19,            /**< ( E--->G ) Abort test                 */
        MSG_USR = 23,             /**< ( E<---G ) User message               */
        MSG_END = 29,             /**< ( E--->G ) Quit                       */
        MSG_EVENT = 31,           /**< ( E<---G ) Event                      */
        MSG_EVENT_IND = 37,       /**< ( E--->G ) Event Indication           */
        MSG_RUN = 41,             /**< ( G--->T ) Run Testclass func.        */
        MSG_EXTIF = 43,           /**< ( uE--->G--->E ) Master/Slave command */
        MSG_SNDRCV = 47,          /**< ( E--->G--->uE ) sendreceive response */
	MSG_RUN_ID = 59           /**< ( G--->E ) Identifier for test run    */
} MINMsgType;

typedef struct _MsgBuffer MsgBuffer;

/** Typedef for the signal handler */
typedef void    (*sighandler_t) (int);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/** Buffer for sending/recieving messages via message queues. */
struct _MsgBuffer {
        /** PID of recieving process */
        long            receiver_;
        /** PID of sending process */
        long            sender_;
        /** Message type */
        MINMsgType     type_;
        /** Parameter of the message, it depends on message type. */
        int             param_;
        /** Short description */
        char            desc_[MaxDescSize];
        /** Parameter of the message */
        char            message_[MaxMsgSize];
        /** Special parameter, used for external controller error codes 
         *  it is also used as a flag that indocates if TMC should send 
         *  TCD. */
        int             special_;
        /** Type of external controller command in message. */
        TMSCommand      extif_msg_type_;
};
/* ------------------------------------------------------------------------- */
struct _MIBuff {
        long            receiver_;
        char            mtext[1];
        long            sender_;
        MINMsgType     type_;
        int             param_;
        unsigned short  msgbeg_;
        int             special_;
        TMSCommand      extif_msg_type_;
        char            desc_[2 + MaxMsgSize + MaxDescSize];
};
/* ------------------------------------------------------------------------- */
struct ShmItem {
        key_t key_;
        int shmid_;        
};
/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
key_t           mq_key ();
/* ------------------------------------------------------------------------- */
void            mq_init_buffer ();
/* ------------------------------------------------------------------------- */
int             mq_queue_exists (int queueid);
/* ------------------------------------------------------------------------- */
int             mq_open_queue (int queueid);
/* ------------------------------------------------------------------------- */
int             mq_close_queue (int msqid);
/* ------------------------------------------------------------------------- */
int             mq_send_message (int msqid, MsgBuffer * buf);
/* ------------------------------------------------------------------------- */
int             mq_send_message_block (int msqid, MsgBuffer * buf);
/* ------------------------------------------------------------------------- */
int             mq_send_message2 (int msqid, long adress, MINMsgType type,
                                  int param, const char *msg);
/* ------------------------------------------------------------------------- */
int             mq_read_message (int msqid, long msg_type, MsgBuffer * buf);
/* ------------------------------------------------------------------------- */
int             mq_peek_message (int msqid, long msg_type);
/* ------------------------------------------------------------------------- */
int             mq_buffer_size ();
/* ------------------------------------------------------------------------- */
void            mq_flush_msg_buffer ();
/* ------------------------------------------------------------------------- */
void            mq_resend_buffered ();
/* ------------------------------------------------------------------------- */
int             sl_pause_process (long pid);
/* ------------------------------------------------------------------------- */
int             sl_resume_process (long pid);
/* ------------------------------------------------------------------------- */
int             sl_kill_process (long pid);
/* ------------------------------------------------------------------------- */
int             sl_set_sighandler (int signum, sighandler_t sighal);
/* ------------------------------------------------------------------------- */
int             sm_create (int shmgetid, unsigned int size);
/* ------------------------------------------------------------------------- */
int             sm_destroy (int shmid);
/* ------------------------------------------------------------------------- */
void           *sm_attach (int shmid);
/* ------------------------------------------------------------------------- */
void            sm_detach (void *shmaddr);
/* ------------------------------------------------------------------------- */
int             sm_read (const void *shmaddr, void *data, unsigned int size);
/* ------------------------------------------------------------------------- */
int             sm_write (void *shmaddr, const void *data, unsigned int size);
/* ------------------------------------------------------------------------- */
int             sm_get_handler();
/* ------------------------------------------------------------------------- */
size_t          sm_get_segsz(int shmid);
/* ------------------------------------------------------------------------- */
#endif                          /* MIN_IPC_MECHANISM_H */
/* End of file */
