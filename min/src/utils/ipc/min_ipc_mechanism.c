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
 *  @file       min_ipc_mechanism.c
 *  @version    0.1
 *  @brief      This file contains implementation of the IPC Mechanism of MIN
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include "min_ipc_mechanism.h"

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
LOCAL int __shm_handler__ = -1;

LOCAL DLList *__shm_handlers__ = INITPTR;
/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
/* ------------------------------------------------------------------------- */
/** Buffer for messages that are waiting to be send */
LOCAL DLList   *msg_buffer = INITPTR;
/* ------------------------------------------------------------------------- */
/** PID of process that has called mq_open_queue. 
 *  We need to keep a track of process we are in because fork will copy
 *  msg_buffer so it will be redundand. Solution is to flush the buffer when 
 *  pid will change = situation where there is one buffer per process
 */
LOCAL pid_t     lastpid = 0;
/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* None */
/* ------------------------------------------------------------------------- */
/** Wrapps around system call msgsnd, it also has error handling included */
LOCAL int       mq_wrapper_around_msgsnd (int msqid, MsgBuffer * buf);
/* ------------------------------------------------------------------------- */
/** Translates MsgBuffer to _MIBuff */
LOCAL void      mq_msgbuf_to_internal (struct _MIBuff *ibuf,
                                       const MsgBuffer * buf);
/* ------------------------------------------------------------------------- */
/** Translates _MIBuff to MsgBuffer */
LOCAL void      mq_internal_to_msgbuf (MsgBuffer * buf,
                                       const struct _MIBuff *ibuf);
/* ------------------------------------------------------------------------- */
/** Gives the size of internal buffer in bytes */
LOCAL unsigned int mq_internal_length (const struct _MIBuff *ibuf);
/* ------------------------------------------------------------------------- */
LOCAL int _findShmKey(const void *a, const void *b);
/* ------------------------------------------------------------------------- */
LOCAL int _findShmShmid(const void *a, const void *b);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
LOCAL int mq_wrapper_around_msgsnd (int msqid, MsgBuffer * buf)
{
        int             retval = ENOERR;
        int             result = ENOERR;
        unsigned int    length = sizeof (struct _MIBuff) - sizeof (long);
        struct _MIBuff  ibuf;
        struct _MIBuff *ptr2ibf = INITPTR;
        DLListIterator  it = DLListNULLIterator;

        if (buf == INITPTR) {
                retval = -1;
                goto EXIT;
        }

        /* lets try to send some buffered first, buffer has to be FIFO */
        result = 0;
        it = dl_list_head (msg_buffer);
        while ((result != -1) && (it != DLListNULLIterator)) {

                /* try to send message from the buffer */
                ptr2ibf = (struct _MIBuff *)dl_list_data (it);
                length = mq_internal_length (ptr2ibf);
                result = msgsnd (msqid, ptr2ibf, length, IPC_NOWAIT);

                /* if successfull then remove it from the buffer */
                if (result != -1) {
                        free (dl_list_data (it));
                        dl_list_remove_it (it);
                        it = dl_list_head (msg_buffer);
                }
        }

        /* now lets try to send orginal message */
        mq_msgbuf_to_internal (&ibuf, buf);
        length = mq_internal_length (&ibuf);
        result = msgsnd (msqid, &ibuf, length, IPC_NOWAIT);

        /* Sending has failed, now we have to examine errno */
        if (result == -1) {
                switch (errno) {
                case EAGAIN:
                        MIN_NOTICE ("MQ is full - message will be buffered");
                        /* Queue is full, we need to buffer the msg */
                        /* If buffer is not created then allocate it */
                        if (msg_buffer == INITPTR) {
                                msg_buffer = dl_list_create ();
                        }

                        /* Create new message buffer and copy message */
                        ptr2ibf = NEW (struct _MIBuff);
                        memcpy (ptr2ibf, &ibuf, sizeof (struct _MIBuff));

                        /* Put message into the buffer */
                        dl_list_add (msg_buffer, (void *)ptr2ibf);

                        break;
                default:
                        /* EINVAL, EACCES, EFAULT, EINTR */
                        MIN_ERROR ("MQ Error: %s",strerror (errno));
                        retval = -1;
                }
        }
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
LOCAL void mq_msgbuf_to_internal (struct _MIBuff *ibuf, const MsgBuffer * buf)
{
        if (ibuf == INITPTR) {
                return;
        }
        if (buf == INITPTR) {
                return;
        }

        ibuf->receiver_ = buf->receiver_;
        ibuf->sender_ = buf->sender_;
        ibuf->type_ = buf->type_;
        ibuf->param_ = buf->param_;
        ibuf->special_ = buf->special_;
        ibuf->extif_msg_type_ = buf->extif_msg_type_;

        ibuf->msgbeg_ = strlen (buf->desc_) + 1;
        STRCPY (ibuf->desc_, buf->desc_, ibuf->msgbeg_);
        STRCPY (&ibuf->desc_[ibuf->msgbeg_], buf->message_,
                strlen (buf->message_) + 1);
}

/* ------------------------------------------------------------------------- */
LOCAL void mq_internal_to_msgbuf (MsgBuffer * buf, const struct _MIBuff *ibuf)
{
        if (buf == INITPTR) {
                return;
        }
        if (ibuf == INITPTR) {
                return;
        }

        memset (buf, 0x0, sizeof (MsgBuffer));

        buf->receiver_ = ibuf->receiver_;
        buf->sender_ = ibuf->sender_;
        buf->type_ = ibuf->type_;
        buf->param_ = ibuf->param_;
        buf->special_ = ibuf->special_;
        buf->extif_msg_type_ = ibuf->extif_msg_type_;

        STRCPY (buf->desc_, ibuf->desc_, ibuf->msgbeg_);
        STRCPY (buf->message_, &ibuf->desc_[ibuf->msgbeg_], MaxMsgSize);
}

/* ------------------------------------------------------------------------- */
LOCAL unsigned int mq_internal_length (const struct _MIBuff *ibuf)
{
        unsigned int    base_length = sizeof (long) * 2 + sizeof (char)
            + sizeof (int) * 2 + sizeof (unsigned short)
            + sizeof (MINMsgType)
            + sizeof (TMSCommand);
        unsigned int    length = 0;

        if (ibuf == INITPTR) {
                goto EXIT;
        }
        length = base_length
            + strlen (ibuf->desc_) + 1
            + strlen (&ibuf->desc_[ibuf->msgbeg_]) + 1;
      EXIT:
        return length;
}

/* ------------------------------------------------------------------------- */
LOCAL int _findShmKey(const void *a, const void *b)
{
        const key_t *p1 = (const key_t*)b;
        const struct ShmItem *p2 = (const struct ShmItem*)a;

        if ((*p1)==p2->key_) return 0;
        else return -1;
}
/* ------------------------------------------------------------------------- */
LOCAL int _findShmShmid(const void *a, const void *b)
{
        const int *p1 = (const int*)b;
        const struct ShmItem *p2 = (const struct ShmItem*)a;

        if ((*p1)==p2->shmid_) return 0;
        else return -1;
}
/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Checks if queue of a given id already exists.
 *  @param queueid queue id, which is 8bit number. Char can be used instead.
 *  @return 0 when queue exists, 1 when queue does not exists, -1 in case of
 *          failure.
 */
int mq_queue_exists (int queueid)
{
        int             retval = -1;
        char           *path = "/tmp";
        key_t           key = ftok (path, queueid);
        int             msqid = -1;
        if (key == -1) {
                MIN_FATAL ("Key for queue not created");
                errno = EFAULT;
        }
        else {
                msqid = msgget (key, IPC_CREAT | IPC_EXCL | 0660);
        }
        if (msqid == -1) {
                if (errno == EEXIST)
                        retval = 1;
                else
                        retval = -1;
        } else
                retval = 0;
        return retval;
}

/* ------------------------------------------------------------------------- */
void mq_init_buffer ()
{
        DLListIterator  it = DLListNULLIterator;

        /* Prepare the buffer */
        if (msg_buffer == INITPTR) {
                /* If buffer is not created then allocate it */
                msg_buffer = dl_list_create ();
                lastpid = getpid ();
        } else {
                /* Buffer exists, need to check PID. if PID differs
                 * from lastpid then we are a new process and flushing of
                 * buffer is needed. Otherwise it is ok.
                 */
                if (lastpid != getpid ()) {
                        /* correct the lastpid value */
                        lastpid = getpid ();

                        /* we are a new process, flush the buffer */
                        /* but not destroy the buffer */
                        it = dl_list_head (msg_buffer);
                        while (it != DLListNULLIterator) {
                                free (dl_list_data (it));
                                dl_list_remove_it (it);
                                it = dl_list_head (msg_buffer);
                        }
                }
        }
}

/* ------------------------------------------------------------------------- */
/** Creates a new message queue, if queue identified by given key already
 *  exists its id is returned.
 *  @param queueid queue id, which is 8bit number. Char can be used instead
 *  @return queue id number, or -1 in case of failure
 *
 *  Possible Errors:
 *  - ENOSPC Maximum number of allowed message queue has been reached.
 *  - EFAULT Error in opening.
 */
int mq_open_queue (int queueid)
{
        char           *path = "/tmp";
        key_t           key = ftok (path, queueid);
        int             msqid = -1;

        /* create queue */
        if (key == -1) {
                MIN_FATAL ("Key for queue not created");
                errno = EFAULT;
                goto EXIT;
        } else {
                msqid = msgget (key, IPC_CREAT | 0660);
                if (msqid == -1) {
                        MIN_FATAL ("MQ not created: %s",strerror (errno));
                }
        }

        mq_init_buffer ();
      EXIT:
        return msqid;
}

/* ------------------------------------------------------------------------- */
/** Closes message queue. The queue is removed from the system.
 *  @param msqid message queue identifier.
 *  @return 0 on success, -1 on failure
 */
int mq_close_queue (int msqid)
{
        /* flush the buffer */
        mq_flush_msg_buffer ();

        /* close queue */
        if (msgctl (msqid, IPC_RMID, 0) == -1) {
                MIN_ERROR ("Error when closing MQ");
                return -1;
        }
        else
                return 0;
}

/* ------------------------------------------------------------------------- */
/** Sends message to the queue.
 *  @param msqid message queue identifier
 *  @param buf pointer to buffer with data to send
 *  @return 0 when succeded, or -1 in case of failure
 */
int mq_send_message (int msqid, MsgBuffer * buf)
{
        return mq_wrapper_around_msgsnd (msqid, buf);
}

/* ------------------------------------------------------------------------- */
/** Sends message to the queue in a blocking way.
 *  @param msqid message queue identifier
 *  @param buf pointer to buffer with data to send
 *  @return 0 when succeded, or -1 in case of failure
 */
int mq_send_message_block (int msqid, MsgBuffer * buf)
{
        unsigned int    length = sizeof (struct _MIBuff) - sizeof (long);
        struct _MIBuff  ibuf;

        mq_msgbuf_to_internal (&ibuf, buf);

        MIN_DEBUG ("%d -> %d [%d]\n", ibuf.sender_, ibuf.receiver_,
                     ibuf.type_);
        length = mq_internal_length (&ibuf);
        return msgsnd (msqid, &ibuf, length, 0);
}

/* ------------------------------------------------------------------------- */
/** Sends message to the queue.
 *  @param msqid message queue identifier
 *  @param adress pid of the recieving process
 *  @param type message type, @see MINMsgType
 *  @param param message parameter, its value depends on message type
 *  @param msg custom parameter, description of the message, depends on type
 *             of the message.
 *  @return 0 when succeded, or -1 in case of failure
 */
int mq_send_message2 (int msqid, long adress, MINMsgType type, int param,
                      const char *msg)
{
        MsgBuffer       buf;

        /* construct the message */
        buf.receiver_ = adress;
        buf.sender_ = getpid ();
        buf.type_ = type;
        buf.param_ = param;
        STRCPY (buf.desc_, "\0", MaxDescSize);
        STRCPY (buf.message_, msg, MaxMsgSize);

	MIN_DEBUG ("%d -> %d [%d]\n", buf.sender_, buf.receiver_, type);

        return mq_wrapper_around_msgsnd (msqid, &buf);
}

/* ------------------------------------------------------------------------- */
/** Sends message to the queue in a blocking way.
 *  @param msqid message queue identifier
 *  @param adress pid of the recieving process
 *  @param type message type, @see MINMsgType
 *  @param param message parameter, its value depends on message type
 *  @param msg custom parameter, description of the message, depends on type
 *             of the message.
 *  @return 0 when succeded, or -1 in case of failure
 */
int mq_send_message2_block (int msqid, long adress, MINMsgType type,
                            int param, const char *msg)
{
        struct _MIBuff  ibuf;
        unsigned int    length = sizeof (struct _MIBuff) - sizeof (long);

        /* construct the message */
        ibuf.receiver_ = adress;
        ibuf.sender_ = getpid ();
        ibuf.type_ = type;
        ibuf.param_ = param;
        STRCPY (ibuf.desc_, "\0", MaxDescSize);
        STRCPY (ibuf.desc_, msg, MaxMsgSize);

        MIN_DEBUG ("%d -> %d [%d]\n", ibuf.sender_, ibuf.receiver_, type);

        return msgsnd (msqid, &ibuf, length, 0);
}

/* ------------------------------------------------------------------------- */
/** Reads message from queue.
 *  @param msqid message queue identifier
 *  @param msg_type the type of message we are going to read
 *  @param buf pointer to the buffer for incomming message
 *  @return number of bytes recieved, or -1 in case of error
 *
 *  Possible Errors:
 *  - EINVAL invalid value is passed to the function
 *  - EINTR function interrupted by the signal
 *  - EIDRM queue identifier removed from the system
 *  - E2BIG message to big to be recieved
 */
int mq_read_message (int msqid, long msg_type, MsgBuffer * buf)
{
        unsigned int    length = sizeof (struct _MIBuff) - sizeof (long);
        int             retval = ENOERR;
        int             result = 0;
        TSBool          msgread = ESFalse;
        DLListIterator  it = DLListNULLIterator;
        struct _MIBuff *ipom = INITPTR;
        struct _MIBuff  ibuf;

        memset (&ibuf, 0x0, sizeof (struct _MIBuff));

        while (1) {

                /* peek for message in MQ */
                if (mq_peek_message (msqid, msg_type) == 1) {
                        length = sizeof (struct _MIBuff) - sizeof (long);
                        retval = msgrcv (msqid, &ibuf, length, msg_type, 0);
			if (retval==(ssize_t)-1) {
				MIN_FATAL("Message in queue way too big to be recieved, error: %d",errno);
			} else {
				MIN_DEBUG("Read message of size: %d",retval);
			}
                        mq_internal_to_msgbuf (buf, &ibuf);
                        msgread = ESTrue;
                }

                /* try to send buffered */
                result = 0;
                it = dl_list_head (msg_buffer);
                while ((it != DLListNULLIterator) && (result != -1)) {

                        /* try to send message from the buffer */
                        ipom = (struct _MIBuff *)dl_list_data (it);
                        length = mq_internal_length (ipom);
                        result = msgsnd (msqid, ipom, length, IPC_NOWAIT);

                        /* if successfull then remove it from the buffer */
                        if (result != -1) {
                                free (dl_list_data (it));
                                dl_list_remove_it (it);
                                it = dl_list_head (msg_buffer);
                        }
                }

                /* break the loop if msg was read from the queue */
                if (msgread == ESTrue) {
                        break;
                }

                /* slow down a little bit this fast loop */
                usleep (10000);
        }

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Peeks message queue to see if mesage of given type has arrived. This
 *  function is no blocking.
 *  @param msqid message queue identifier
 *  @param msg_type the type of message we are waiting for. If == 0 then
 *         messages of all types will be metched.
 *  @return 1 when message arrives, 0 when there is no message.
 */
int mq_peek_message (int msqid, long msg_type)
{
        int             result =
            msgrcv (msqid, NULL, 0, msg_type, IPC_NOWAIT);
        if (result == -1 && errno == E2BIG)
                return 1;
        else
                return 0;
}

/* ------------------------------------------------------------------------- */
/** Flushes msg_buffer */
void mq_flush_msg_buffer ()
{
        DLListIterator  it = DLListNULLIterator;

        /* check size of the buffer */
        MIN_DEBUG ("There are %d messages buffered."
                     " Buffer will be flushed", dl_list_size (msg_buffer));

        /* flush the buffer and destroy it */
        it = dl_list_head (msg_buffer);
        while (it != DLListNULLIterator) {
                free (dl_list_data (it));
                dl_list_remove_it (it);
                it = dl_list_head (msg_buffer);
        }
        dl_list_free (&msg_buffer);

        /* set lastpid */
        lastpid = 0;
}

/* ------------------------------------------------------------------------- */
/** Resends messages from the buffer. */
void mq_resend_buffered ()
{
        /* obtain id of a queue. It is kind of a hack, queue id should come
         * as a parameter, but to avoid extern we are doing it this way. */
        int             msqid = mq_open_queue ('a');
        int             result = 0;
        unsigned int    length = sizeof (struct _MIBuff) - sizeof (long);
        DLListIterator  it = dl_list_head (msg_buffer);
        struct _MIBuff *ptr2ibf = INITPTR;

        /* sending was successfull, lets try to send some buffered */
        while ((it != DLListNULLIterator)) {

                /* try to send message from the buffer */
                ptr2ibf = (struct _MIBuff *)dl_list_data (it);
                length = mq_internal_length (ptr2ibf);
                result = msgsnd (msqid, ptr2ibf, length, IPC_NOWAIT);

                /* if successfull then remove it from the buffer */
                if (result != -1) {
                        free (dl_list_data (it));
                        dl_list_remove_it (it);
                        it = dl_list_head (msg_buffer);
                }

                usleep (10000);
        }
}

/* ------------------------------------------------------------------------- */
int mq_buffer_size ()
{
        return dl_list_size (msg_buffer);
}

/* ------------------------------------------------------------------------- */
/** Pauses process.
 *  @param pid the id of a process to be paused (>0)
 *  @return 0 on success and -1 on failure
 *
 *  Possible Errors:
 *  - EINVAL invalid value was passed to the funcion
 *  - ESRCH there is no process of a given pid
 *  - EPERM calling process has no permission to send a signal
 */
int sl_pause_process (long pid)
{
        int             retval = -1;
        if (pid != 0)
                retval = kill (pid, SIGSTOP);
        else {
                MIN_WARN ("Given pid is invalid");
                errno = EINVAL;
        }
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Pauses process.
 *  @param pid the id of a process to be resumed (>0)
 *  @return 0 on success and -1 on failure
 *
 *  Possible Errors:
 *  - EINVAL invalid value was passed to the funcion
 *  - ESRCH there is no process of a given pid
 *  - EPERM calling process has no permission to send a signal
 */
int sl_resume_process (long pid)
{
        int             retval = -1;
        retval = kill (pid, SIGCONT);
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Pauses process.
 *  @param pid the id of a process to be killed (>0)
 *  @return 0 on success and -1 on failure
 *
 *  Possible Errors:
 *  - EINVAL invalid value was passed to the funcion
 *  - ESRCH there is no process of a given pid
 *  - EPERM calling process has no permission to send a signal
 */
int sl_kill_process (long pid)
{
        int             retval = -1;
        retval = kill (pid, SIGKILL);
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Sets signal handler for the given signal.
 *  @param signum is the signal id
 *  @param sighal pointer to the signal handler function.
 *  @return -1 on failure, 0 on success.
 */
int sl_set_sighandler (int signum, sighandler_t sighal)
{
        int             retval = -1;
        struct sigaction new_action;
        new_action.sa_handler = sighal;
        sigfillset (&new_action.sa_mask);
        new_action.sa_flags = SA_NOCLDSTOP;
        retval = sigaction (signum, &new_action, NULL);
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Gets shared memory segment
 *  @param shmgetid [in] key used when requesting shared memmory segment. 8bit
 *  @param size [in] the size of shared memory segment.
 *  @return shared memory id or -1 in case of failure.
 *
 *  Possible Errors:
 *  - EFAULT: error in function
 */
int sm_create (int shmgetid, unsigned int size)
{
        char           *path = "/tmp";
        key_t           key;
        struct ShmItem  *tmp = INITPTR;
        DLListIterator it = DLListNULLIterator;

        key = ftok (path, shmgetid);
        if (key == -1) {
                MIN_FATAL ("Key for SM not created");
                errno = EFAULT;
                return -1;
        }

        if (__shm_handlers__==INITPTR) { __shm_handlers__ = dl_list_create(); }
        else {
                it = dl_list_find(dl_list_head(__shm_handlers__),
                                  dl_list_tail(__shm_handlers__),
                                  _findShmKey, &key);
        }

        if (it==DLListNULLIterator) {
                tmp = NEW(struct ShmItem);
                tmp->key_ = key;
                tmp->shmid_ = shmget (key, size, IPC_CREAT | 0660);
                dl_list_add(__shm_handlers__,tmp);
                return tmp->shmid_;
        } else {
                return ((struct ShmItem*)dl_list_data(it))->shmid_;
        }
}

/* ------------------------------------------------------------------------- */
/** Marks shared memory segment to be destroyed.
 *  @param shmid [in] the id of the shared memory segment to be destroyed. 
 *  @return 0 in case of success, -1 otherwise.
 */
int sm_destroy (int shmid)
{
        DLListIterator it = DLListNULLIterator;
        it = dl_list_find(dl_list_head(__shm_handlers__),
                          dl_list_tail(__shm_handlers__),
                          _findShmShmid, &shmid);

        if (it==DLListNULLIterator) {
                MIN_ERROR("Requested to remove not existing SHM %d",
                           shmid);
                return -1;
        } else {
                if (shmctl (shmid, IPC_RMID, 0) == -1)
                        return -1;
                else {
                        free(dl_list_data(it));
                        dl_list_remove_it(it);
                        if (dl_list_size(__shm_handlers__)==0) {
                                dl_list_free(&__shm_handlers__);
                        }
                        return 0;
                }
        }
}

/* ------------------------------------------------------------------------- */
/** Returns pointer to the allocated shared memory segment.
 *  @param shmid [in] id of the shared memory segment we want to be attached to
 *  @return pointer to the SH segment, or INITPTR in case of failure.
 */
void           *sm_attach (int shmid)
{
        int            *retval = (int *)shmat (shmid, (void *)NULL, 0);
        if (*retval == -1) {
                MIN_WARN ("Could not find allocated memory segment");
                return INITPTR;
        }
        else
                return (void *)retval;
}

/* ------------------------------------------------------------------------- */
inline void sm_detach (void *shmaddr)
{
        if (shmaddr != INITPTR)
                shmdt (shmaddr);
}

/* ------------------------------------------------------------------------- */
int sm_read (const void *shmaddr, void *data, unsigned int size)
{
        int             retval = ENOERR;
        if (shmaddr == INITPTR) {
                errno = EINVAL;
                retval = -1;
                goto EXIT;
        }
        if (data == INITPTR) {
                errno = EINVAL;
                retval = -1;
                goto EXIT;
        }

        memcpy (data, shmaddr, size);
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
int sm_write (void *shmaddr, const void *data, unsigned int size)
{
        int             retval = ENOERR;
        if (shmaddr == INITPTR) {
                errno = EINVAL;
                retval = -1;
                goto EXIT;
        }
        if (data == INITPTR) {
                errno = EINVAL;
                retval = -1;
                goto EXIT;
        }

        memcpy (shmaddr, data, size);
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
int sm_get_handler()
{
        return __shm_handler__;
}
/* ------------------------------------------------------------------------- */
/** Returns the size of SHM segment
 *  @param shmid [in] id of the shared memory segment we want to be attached to
 *  @return size of the segment or 0
 */
size_t sm_get_segsz (int shmid)
{
        struct shmid_ds shmid_ds;
        
        if (shmctl (shmid, IPC_STAT, &shmid_ds) == -1) {
                MIN_WARN ("shmget() failed: %s", strerror(errno));
                return 0;
        }
 
        return shmid_ds.shm_segsz;
}
/* ------------------------------------------------------------------------- */

/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "min_ipc_mechanism.tests"
#endif                          /* MIN_UNIT_TEST */
/* ------------------------------------------------------------------------- */
/* End of file */
