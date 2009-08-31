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
 *  @file       tec_tcp_handling.c
 *  @version    0.1
 *  @brief      RCP handling routines for tcp based master slave
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <limits.h>

#include <min_parser.h>
#include <min_logger.h>
#include <tec.h>
#include <tec_events.h>
#include <min_common.h>
#include <dllist.h>
#include <tec_rcp_handling.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
extern DLList *ms_assoc;
extern int slave_exit;
/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
int rcp_listen_socket = -1;
pthread_mutex_t socket_write_mutex_ = PTHREAD_MUTEX_INITIALIZER;
int current_slave_fd;
/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
LOCAL int set_active_rcp_sockets (fd_set *rd, fd_set *wr, int nfds);
/* ------------------------------------------------------------------------- */
LOCAL void rw_rcp_sockets (fd_set *rd, fd_set *rw);
/* ------------------------------------------------------------------------- */
LOCAL void socket_read_rcp (slave_info *slave);
/* ------------------------------------------------------------------------- */
LOCAL void socket_write_rcp (slave_info *slave);
/* ------------------------------------------------------------------------- */
LOCAL slave_info *find_slave_by_fd (int fd, DLListIterator *itp);
/* ------------------------------------------------------------------------- */
LOCAL slave_info *find_master ();
/* ------------------------------------------------------------------------- */
LOCAL void free_tcp_slave (slave_info *slave);
/* ------------------------------------------------------------------------- */
LOCAL int splithex (char *hex, int *dev_id, int *case_id);

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/** Goes through the list of slaves and sets sockets that are available
 *  for reading or writing to the fd sets
 *  @param rd set of read sockets
 *  @param wr set of write sockets
 *  @param nfds highest fd in sets before calling this func
 *  @return the value of highest fd in set
 */
LOCAL int set_active_rcp_sockets (fd_set *rd, fd_set *wr, int nfds)
{
	DLListIterator it;
	slave_info *slave;
	
	for (it = dl_list_head (ms_assoc); it != INITPTR;
	     it = dl_list_next (it)) {
		slave = dl_list_data (it);
		if (slave->fd_ != 0 && slave->fd_ != -1) {
			FD_SET (slave->fd_, rd);
			FD_SET (slave->fd_, wr);
			nfds = MAX (nfds, slave->fd_);
		}
	}
       
	return nfds;
}
/* ------------------------------------------------------------------------- */
/** Read/write sockets  
 *  @param rd set of read sockets
 *  @param wr set of write sockets
 */
LOCAL void rw_rcp_sockets (fd_set *rd, fd_set *wr)
{
	DLListIterator it;
	slave_info *slave;
	
	for (it = dl_list_head (ms_assoc); it != INITPTR;
	     it = dl_list_next (it)) {
		slave = dl_list_data (it);
		if (slave->fd_ > 0) {
			if (FD_ISSET (slave->fd_, rd))
				socket_read_rcp (slave);
			if (slave->fd_ > 0 && FD_ISSET (slave->fd_, wr))
				socket_write_rcp (slave);
		}
	}
       

	return;
}
/* ------------------------------------------------------------------------- */
/** Frees the tcp type slave - closes slave socket, marks the slave free
 *  @param slave slave to be freed
 */
LOCAL void free_tcp_slave (slave_info *slave)
{
	slave_info *master;
	DLListIterator it;
	if (!strcmp (tx_share_buf(slave->slave_type_), "master")) {
		master = find_slave_by_fd (slave->fd_, &it);
		close (master->fd_);
		tx_destroy (&master->slave_type_);
		tx_destroy (&master->slave_name_);
		dl_list_free (&master->write_queue_);
		dl_list_remove_it (it);
		DELETE (master);
		slave_exit = 1;
		return;
	} 

	close (slave->fd_);
	slave->fd_ = -1;
	slave->status_ = SLAVE_STAT_FREE;
	slave->slave_id_ = 0;
	tx_destroy (&slave->slave_name_);
	return;
}
/* ------------------------------------------------------------------------- */
/** Reads the socket bound to the slave_info
 *  @param slave_info the slave with a socket available for reading
 */
LOCAL void socket_read_rcp (slave_info *slave)
{
	int bytes_read;
        char *buff, len_buff[4];
        unsigned int len;
        
        bytes_read = read (slave->fd_, &len_buff, 2);
        if (bytes_read != 2) {
                MIN_WARN ("can't read even 2 bytes from socket");
		free_tcp_slave (slave);
                return;
        }
        len = len_buff [1] << 8  | len_buff [0];
	if (len >= (INT_MAX - 1)) {
		MIN_WARN ("Invalid msg len %d", len);
		return;
	}
        MIN_INFO ("Message from RCP socket of len %d", len);

        buff = NEW2 (char, len + 1);
        bytes_read = read (slave->fd_, buff, len);
        if (bytes_read != len) {
                MIN_WARN ("failed to read the whole message");
		DELETE (buff);
		free_tcp_slave (slave);
                return;
        }

	buff [len] = '\0';
	current_slave_fd = slave->fd_; 
        tec_extif_message_received (buff, len);

        DELETE (buff);

        return;

}
/* ------------------------------------------------------------------------- */
/** Writes to socket bound to slave_info
 *  @param slave_info the slave with a socket available for writing
 */
LOCAL void socket_write_rcp (slave_info *slave)
{
        Text *tx;
	DLListIterator it;
        int ret;
	char len_buff[5];

        if (slave->write_queue_ == NULL || slave->write_queue_ == INITPTR) {
		MIN_WARN ("write queue does not exist");
                return;
	}

	pthread_mutex_lock (&socket_write_mutex_);

        it = dl_list_head (slave->write_queue_);
        if (it == DLListNULLIterator) {
		pthread_mutex_unlock (&socket_write_mutex_);

                return;
        }
        
        tx = dl_list_data (it);

	len_buff[0] = strlen (tx_share_buf(tx));
        len_buff[1] = strlen (tx_share_buf(tx)) >> 8;
        
        ret = write (slave->fd_, &len_buff, 2);

	MIN_DEBUG ("SENDING TO EXTIF :%s", tx_share_buf (tx));
        ret = write (slave->fd_, tx_share_buf (tx), strlen (tx_share_buf (tx)));

        dl_list_remove_it (it);
        tx_destroy (&tx);
	pthread_mutex_unlock (&socket_write_mutex_);

        return;
	
}
/* ------------------------------------------------------------------------- */
/** Finds a slave with fd
 *  @param fd search key
 *  @param itp out used to pass also the DLListIterator to caller
 *  @return slave_info if found, INITPTR if not
 */
LOCAL slave_info *find_slave_by_fd (int fd, DLListIterator *itp)
{
       DLListIterator it;
       slave_info *ips;
 
       *itp = INITPTR;

       for (it = dl_list_head (ms_assoc); it != INITPTR;
            it = dl_list_next (it)) {
               ips = dl_list_data (it);
               if (ips->fd_ == fd)  {
                       *itp = it;
                       return ips;
               }
       }
       
       return INITPTR;
}
/* ------------------------------------------------------------------------- */
/** Searches for master (a "special" slave)
 *  @return slave_info if found, INITPTR if not
 */
LOCAL slave_info *find_master ()
{
       DLListIterator it;
       slave_info *ips;
 

       for (it = dl_list_head (ms_assoc); it != INITPTR;
            it = dl_list_next (it)) {
               ips = dl_list_data (it);
               if (!strcmp (tx_share_buf (ips->slave_type_), "master"))  {
		       return ips;
	       }
       }
       
       return INITPTR;
}
/* ------------------------------------------------------------------------- */
/** Splits RCP's adress string fields into two ints.
 * @param hex [in] string to be split, has to be 8 characters, otherwise
 *  function fails
 * @param dev_id [out] pointer to int that will hold device id
 * @param case_id [out] pointer to int that will hold case id
 * @return result of operation, 0 if ok
 */
LOCAL int splithex (char *hex, int *dev_id, int *case_id)
{
        char            dev_id_c[5];
        char            case_id_c[5];
        char           *endptr;

        if (strlen (hex) != 8) {
                return -1;
        }

        snprintf (dev_id_c, 5, "%s", hex);
        snprintf (case_id_c, 5, "%s", hex + 4);
        *dev_id = strtol (dev_id_c, &endptr, 16);
        *case_id = strtol (case_id_c, &endptr, 16);

        return 0;
}
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Handles socket polling thread
 *  @param arg not used
 *  @return NULL
 */
void *ec_poll_sockets (void *arg)
{
	fd_set rd, wr, er;
	int nfds = 0;
	struct timeval tv;
        int ret;

	
	while (1) {
		FD_ZERO (&rd);
		FD_ZERO (&wr);
		FD_ZERO (&er);
		nfds = set_active_rcp_sockets (&rd, &wr, nfds);
		
		tv.tv_sec = 0;
		tv.tv_usec = 100000;
		ret = select (nfds + 1, &rd, &wr, &er, &tv);
		
		rw_rcp_sockets (&rd, &wr);
	}

	return NULL;
}

/* ------------------------------------------------------------------------- */
/** Build rcp message and adds it to the write queue of slave
 *  @param cmd command e.g. "reserve"
 *  @param sender RCP sender identifier
 *  @param rcvr RCP receiver identifier
 *  @param msg the rest of message
 *  @param fd the socket of the destination
 */
void socket_send_rcp (char *cmd, char *sender, char *rcvr, char* msg, int fd)
{
	Text *tx;
	slave_info *entry;
	DLListIterator it;

	MIN_DEBUG ("cmd:%s sender:%s rcvr:%s msg:%s fd:%d",
		   cmd, sender, rcvr, msg, fd);

	tx = tx_create (cmd);
	tx_c_append (tx, " ");
	tx_c_append (tx, sender);
	tx_c_append (tx, " ");
	tx_c_append (tx, rcvr);
	tx_c_append (tx, " ");
	tx_c_append (tx, msg);

	if (fd == 0 && !strcmp (rcvr, "deadbeef"))
		entry = find_master ();
	else
		entry = find_slave_by_fd (fd, &it);
	
	if (entry == INITPTR) {
		MIN_WARN ("No entry found for socket %d", fd);
		tx_destroy (&tx);
		return;
	}
	pthread_mutex_lock (&socket_write_mutex_);

	dl_list_add (entry->write_queue_, tx);

	pthread_mutex_unlock (&socket_write_mutex_);

}

/* ------------------------------------------------------------------------- */
/** Tries to allocate slave entry for IP communication
 *  @param he host address information
 *  @param slavetype type of the slave e.g. "phone"
 *  @param pid process identifier of the slave test case
 *  @return 0 on success, 1 on error
 */
int allocate_ip_slave (char *slavetype, char *slavename, pid_t pid)
{
       slave_info *slave;
       static int slave_id = 0;
       DLListIterator it;
       int found = 0, sfd;
       struct addrinfo *rp;

       if (ms_assoc == INITPTR)
               return -1;
       
       for (it = dl_list_head (ms_assoc); it != INITPTR;
	    it = dl_list_next (it)) {
	       slave = dl_list_data (it);
	       if (!strcmp (tx_share_buf (slave->slave_type_), slavetype) &&
		   slave->status_ == SLAVE_STAT_FREE) {
		       found = 1;
		       slave->slave_name_ = tx_create (slavename);
		       break;
	       }
       }
       
       if (found == 0) {
	       MIN_WARN ("no slave of type %s found from ip slave pool");
	       return -1;
       }

       for (rp = slave->addrinfo_; rp != NULL; rp = rp->ai_next) {
	       sfd = socket(rp->ai_family, rp->ai_socktype,
			    rp->ai_protocol);
	       if (sfd == -1)
		       continue;

	       if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
		       break;                  

	       close(sfd);
       }

       if (rp == NULL) {               
	       MIN_WARN ("Could not connect %s", strerror(errno));
	       return -1;
       }
       
       if (slave_id >= 0xffff) {
	       MIN_WARN ("Slave id wraparound");
	       slave_id = 1;
       } else
	       slave_id ++;
       
       slave->fd_ = sfd;
       slave->slave_id_ = slave_id;
       slave->status_ = SLAVE_STAT_RESERVED;
       slave->pid_ = pid;

       return 0;
}

/* ------------------------------------------------------------------------- */
/** Creates a new master entry.
 *  @param socket the socket of the master
 */
void new_tcp_master (int socket)
{
	slave_info *master;
	
	master = NEW (slave_info);
	master->slave_name_ = tx_create ("deadbeef");
	master->slave_type_ = tx_create ("master");
	master->fd_ = socket;
	master->write_queue_ = dl_list_create ();
	master->status_ = SLAVE_STAT_RESERVED;

	dl_list_add (ms_assoc, master);

	return;
}

/* ------------------------------------------------------------------------- */
/** Function to close slave connection to slave.
 * @param slave the slave_info struck
 */
void tcp_slave_close (slave_info *slave) {
	close (slave->fd_);
	slave->fd_ = -1;
	slave->status_ = SLAVE_STAT_FREE ;
	tx_destroy (&slave->slave_name_);
}

/* ------------------------------------------------------------------------- */
/**Function handles "response" message coming from external controller over tcp
 * @param extif_message pointer to item parser. It is assumed that 
 * mip_get_string was  executed once for this parser to get first "word"
 * @return result of operation, 0 if ok.
 */
int tcp_msg_handle_response (MinItemParser * extif_message)
{
        char           *command = INITPTR;
        char           *param1 = NULL;
        char           *srcid = INITPTR;
        char           *destid = INITPTR;
        int             result = 666;
        int             retval = 666;
        int             slave_id = 0;
        int             case_id = 0;
        MsgBuffer       ipc_message;
        slave_info     *slave_entry = INITPTR;
        DLListIterator  slave_entry_item;

        mip_get_next_string (extif_message, &srcid);

        mip_get_next_string (extif_message, &destid);

        mip_get_next_string (extif_message, &command);
	
	slave_entry = find_slave_by_fd (current_slave_fd, &slave_entry_item);
	if (slave_entry == INITPTR) {
		MIN_FATAL ("no slave found bound to fd %d!",
			   current_slave_fd);
	}

	current_slave_fd = -1;

        if (strcasecmp (command, "reserve") == 0) {
		retval = mip_get_next_int (extif_message, &result);
		
                splithex (srcid, &slave_id, &case_id);
                slave_entry->slave_id_ = slave_id;
                /*it seems that result was not send, assume success */
                if (retval == -1)
                        result = 0;
		slave_entry->status_ = SLAVE_STAT_RESERVED;

                ipc_message.sender_ = ec_settings.engine_pid_;
                ipc_message.receiver_ = slave_entry->pid_;
                ipc_message.type_ = MSG_EXTIF;
                ipc_message.special_ = 0;
                ipc_message.extif_msg_type_ = EResponseSlave;
                ipc_message.param_ = result;
                MIN_DEBUG ("ipc sending with result %d", result);
                mq_send_message (mq_id, &ipc_message);
                retval = 0;
        } else if (strcasecmp (command, "release") == 0) {
                retval =
                    mip_get_next_tagged_int (extif_message, "result=", &result);
                if (retval == -1)
                        mip_get_next_int (extif_message, &result);
                /*result was not tagged int */
                if (retval == -1)
                        result = 0;
                /*it seems that result was not send, assume success */
                ipc_message.sender_ = ec_settings.engine_pid_;
                ipc_message.receiver_ = slave_entry->pid_;
                ipc_message.type_ = MSG_EXTIF;
                ipc_message.param_ = result;
                ipc_message.special_ = 0;
                ipc_message.extif_msg_type_ = EResponseSlave;
                MIN_DEBUG ("ipc sending with result %d", result);
                mq_send_message (mq_id, &ipc_message);
                /*slave released, remove it from ms_assoc */
                splithex (srcid, &slave_id, &case_id);
		MIN_DEBUG ("slave_entry = %x, run count = %d",
			   slave_entry, slave_entry->run_cnt_);
		if (slave_entry->run_cnt_ == 0) {
			/* we have a result - can close */
			tcp_slave_close (slave_entry);
		} else
			slave_entry->status_ &= 0xe;
		
                retval = 0;
        } else if (strcasecmp (command, "remote") == 0) {
                mip_get_next_string (extif_message, &command);
                if (strcasecmp (command, "run") == 0) {
                        splithex (srcid, &slave_id, &case_id);
                        /*translate received caseid into proper 
                        number that can be stored in scripter*/
                        case_id = (slave_id<<16) + case_id;
                        mip_get_next_string (extif_message, &param1);
                        if (strcasecmp (param1, "started") == 0) {
                                ipc_message.sender_ = ec_settings.engine_pid_;
                                ipc_message.receiver_ = slave_entry->pid_;
                                ipc_message.type_ = MSG_EXTIF;
                                ipc_message.special_ = case_id;
                                ipc_message.param_ = 0;
                                ipc_message.extif_msg_type_ = EResponseSlave;
                                MIN_DEBUG ("ipc sending with result 0");
                                mq_send_message (mq_id, &ipc_message);
                                retval = 0;
                        } else if (strcasecmp (param1, "ready") == 0) {
                                mip_get_int (extif_message, "result=", &result);
                                ipc_message.sender_ = ec_settings.engine_pid_;
                                ipc_message.receiver_ = slave_entry->pid_;
                                ipc_message.type_ = MSG_EXTIF;
                                ipc_message.special_ = case_id;
                                ipc_message.param_ = result;
                                ipc_message.extif_msg_type_ =
                                    ERemoteSlaveResponse;
                                MIN_DEBUG ("ipc sending with result %d",
                                             result);
                                mq_send_message (mq_id, &ipc_message);
                                retval = 0;
				if (slave_entry != INITPTR && 
				    slave_entry->status_ 
				    & SLAVE_STAT_RESERVED) {
				        slave_entry->run_cnt_ -= 1; 
					
				} else {
					/* we have are free - can close */
					tcp_slave_close (slave_entry);
				}
                        } else if (strcasecmp (param1, "error") == 0) {
                                mip_get_int (extif_message, "result=", &result);
                                ipc_message.sender_ = ec_settings.engine_pid_;
                                ipc_message.receiver_ = slave_entry->pid_;
                                ipc_message.type_ = MSG_EXTIF;
                                ipc_message.special_ = case_id;
                                ipc_message.param_ = result;
                                ipc_message.extif_msg_type_ = EResponseSlave;
                                MIN_DEBUG ("ipc sending with result %d",
                                             result);
                                mq_send_message (mq_id, &ipc_message);
                                retval = 0;
				if (slave_entry != INITPTR &&
				    slave_entry->status_ & 
				    SLAVE_STAT_RESERVED) {
				        slave_entry->run_cnt_ -= 1; 

				} else {
					/* we have are free - can close */
					tcp_slave_close (slave_entry);
				}

                        }
                } else if (strcasecmp (command, "request") == 0) {
                        retval =
                            handle_remote_event_request_resp (extif_message);
                } else if (strcasecmp (command, "release") == 0)
                        retval = 0;
        }
        DELETE (command);
        DELETE (param1);
        DELETE (srcid);
        DELETE (destid);

        return retval;
}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
