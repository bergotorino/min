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
 *  @brief      This file contains implementation of the Test Module Controller
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <dllist.h>
#include <min_logger.h>
#include <tec_rcp_handling.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
extern DLList *ms_assoc;
/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
int rcp_listen_socket = -1;

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
#define MIN_TCP_PORT 5001


/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
LOCAL int set_active_rcp_sockets (fd_set *rd, fd_set *wr, int nfds);
/* ------------------------------------------------------------------------- */
LOCAL void new_tcp_master (int socket, struct sockaddr_in *master_ip);
/* ------------------------------------------------------------------------- */
LOCAL void rw_rcp_sockets (fd_set *rd, fd_set *rw);
/* ------------------------------------------------------------------------- */
LOCAL void socket_read_rcp (slave_info *slave);
/* ------------------------------------------------------------------------- */
LOCAL void socket_write_rcp (slave_info *slave);
/* ------------------------------------------------------------------------- */
LOCAL slave_info *find_slave_by_fd (int fd);
/* ------------------------------------------------------------------------- */
LOCAL slave_info *find_master (int fd);

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
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
LOCAL void new_tcp_master (int socket, struct sockaddr_in *master_ip)
{
	slave_info *master;
	
	master = NEW (slave_info);
	master->slave_name_ = tx_create ("deadbeef");
	master->slave_type_ = tx_create ("master");
	master->fd_ = socket;
	master->write_queue_ = dl_list_create ();
	master->reserved_ = 1;

	dl_list_add (ms_assoc, master);

	return;
}
/* ------------------------------------------------------------------------- */
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
LOCAL void socket_read_rcp (slave_info *slave)
{
	int bytes_read;
        char *buff, len_buff[2];
        unsigned int len;
        
        bytes_read = read (slave->fd_, &len_buff, 2);
        if (bytes_read != 2) {
                MIN_WARN ("can't read even 2 bytes from socket");
                close (slave->fd_);
		slave->fd_ = -1;
                return;
        }
        len = len_buff [1] << 8  | len_buff [0];
        MIN_INFO ("Message from RCP socket of len %d", len);
        buff = NEW2 (char, len + 1);
        bytes_read = read (slave->fd_, buff, len);
        if (bytes_read != len) {
                MIN_WARN ("failed to read the whole message");
		close (slave->fd_);
		slave->fd_ = -1;
                return;
        }

	buff [len] = '\0';
        tec_extif_message_received (buff, len);

        DELETE (buff);

        return;

}
/* ------------------------------------------------------------------------- */
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

        it = dl_list_head (slave->write_queue_);
        if (it == DLListNULLIterator) {
                return;
        }
        
        tx = dl_list_data (it);

	len_buff[0] = strlen (tx_share_buf(tx));
        len_buff[1] = strlen (tx_share_buf(tx)) >> 8;
        
        write (slave->fd_, &len_buff, 2);

        ret = write (slave->fd_, tx_share_buf (tx), strlen (tx_share_buf (tx)));
        MIN_DEBUG ("sent %d bytes", ret);
        dl_list_remove_it (it);
        tx_destroy (&tx);

        return;
	
}
/* ------------------------------------------------------------------------- */
LOCAL slave_info *find_slave_by_fd (int fd)
{
       DLListIterator it;
       slave_info *ips;
 

       for (it = dl_list_head (ms_assoc); it != INITPTR;
            it = dl_list_next (it)) {
               ips = dl_list_data (it);
               if (ips->fd_ == fd)  {
                       return ips;
               }
       }
       
       return INITPTR;
}
/* ------------------------------------------------------------------------- */
LOCAL slave_info *find_master (int fd)
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
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Handles socket polling
 * @return 0 or  -1 on error
 */
int ec_poll_sockets ()
{
	fd_set rd, wr, er;
	int nfds = 0;
	unsigned len;
	struct sockaddr_in client_addr;
	struct timeval tv;
        int ret, rcp_socket;
	
	if (rcp_listen_socket == -1) {
		MIN_WARN ("listen socket not ready, exiting");
		return -1;
	}

	FD_ZERO (&rd);
	FD_ZERO (&wr);
	FD_ZERO (&er);
	FD_SET (rcp_listen_socket, &rd);
	nfds = MAX(nfds, rcp_listen_socket);
	nfds = set_active_rcp_sockets (&rd, &wr, nfds);
	
	tv.tv_sec = 0;
	tv.tv_usec = 100000;
	ret = select (nfds + 1, &rd, &wr, &er, &tv);

	if (FD_ISSET(rcp_listen_socket, &rd)) {
		memset (&client_addr, 0, len = sizeof(client_addr));
		rcp_socket = accept (rcp_listen_socket, 
				     (struct sockaddr *) &client_addr, 
				     &len);
		if (rcp_socket < 0) {
			MIN_FATAL ("accept() failed %s", strerror (errno));
			return -1;
		}
		MIN_INFO ("connect from %s\n",
			  inet_ntoa (client_addr.sin_addr));
                new_tcp_master (rcp_socket, &client_addr);
		if (listen (rcp_listen_socket, 1)) {
			MIN_FATAL ("Listen failed %s", strerror (errno));
			return -1;
		}
	}

	rw_rcp_sockets (&rd, &wr);

	return 0;
}

/* ------------------------------------------------------------------------- */
/** Creates a socket to accept tcp connections
 * @return 0 or -1 on error
 */
int ec_create_listen_socket()
{
	int s;
	struct sockaddr_in in_addr;
        struct addrinfo hints;
        struct addrinfo *result;
        char hostname [HOST_NAME_MAX];

        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_INET;    /* Allow IPv4 */
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;    
        hints.ai_protocol = 0;          
        hints.ai_canonname = NULL;
        hints.ai_addr = NULL;
        hints.ai_next = NULL;

        gethostname (hostname, HOST_NAME_MAX);
        s = getaddrinfo(hostname, 0, &hints, &result);
        if (s != 0) {
                MIN_FATAL ("getaddrinfo: %s\n", gai_strerror(s));
                return -1;
        }
        memcpy (&in_addr, result->ai_addr, sizeof (struct sockaddr_in));
        in_addr.sin_port = htons (MIN_TCP_PORT); 

	if ((rcp_listen_socket = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
                MIN_FATAL ("Failed to create rcp socket %s", strerror (errno));
                return -1;
        }

        if (bind (rcp_listen_socket, 
		  (struct sockaddr *)&in_addr, sizeof (in_addr))  == -1) {
                MIN_FATAL ("Failed to bind to rcp socket %s", strerror (errno));
                return -1;
                
        }
        if (listen (rcp_listen_socket, 1)) {
		MIN_FATAL ("Listen failed %s", strerror (errno));
		return -1;
	}

        MIN_INFO ("accepting connections on port %d\n", MIN_TCP_PORT);

	return 0;
}

/* ------------------------------------------------------------------------- */
void socket_send_rcp (char *cmd, char *sender, char *rcvr, char* msg, int fd)
{
	Text *tx;
	slave_info *entry;

	tx = tx_create (cmd);
	tx_c_append (tx, " ");
	tx_c_append (tx, sender);
	tx_c_append (tx, " ");
	tx_c_append (tx, rcvr);
	tx_c_append (tx, " ");
	tx_c_append (tx, msg);

	if (fd == 0 && !strcmp (rcvr, "deadbeef"))
		entry = find_master (fd);
	else
		entry = find_slave_by_fd (fd);
	
	if (entry == INITPTR) {
		MIN_WARN ("No entry found for socket %d", fd);
		return;
	}

	dl_list_add (entry->write_queue_, tx);
}

/* ------------------------------------------------------------------------- */
/** Tries to allocate slave entry for IP communication
 *  @param he host address information
 *  @param slavetype type of the slave e.g. "phone"
 *  @return 0 on success, 1 on error
 */
int allocate_ip_slave (char *slavetype, char *slavename)
{
       slave_info *slave;
       DLListIterator it;
       int s, found = 0, sfd;
       struct addrinfo hints;
       struct addrinfo *result, *rp;

       if (ms_assoc == INITPTR)
               return -1;
       
       for (it = dl_list_head (ms_assoc); it != INITPTR;
	    it = dl_list_next (it)) {
	       slave = dl_list_data (it);
	       if (!strcmp (tx_share_buf (slave->slave_type_), slavetype) &&
		   slave->reserved_ == 0) {
		       found = 1;
		       slave->slave_name_ = tx_create (slavename);
		       break;
	       }
       }
       
       if (found == 0) {
	       MIN_WARN ("no slave of type %s found from ip slave pool");
	       return -1;
       }

     
       memset(&hints, 0, sizeof(struct addrinfo));
       hints.ai_family = AF_UNSPEC;    
       hints.ai_socktype = SOCK_STREAM;
       hints.ai_flags = 0;
       hints.ai_protocol = 0;         
       
       s = getaddrinfo(slave->he_.h_name, "5001", &hints, &result);
       if (s != 0) {
	       MIN_WARN ("getaddrinfo: %s\n", gai_strerror(s));
	       return -1;
       }

       for (rp = result; rp != NULL; rp = rp->ai_next) {
	       sfd = socket(rp->ai_family, rp->ai_socktype,
			    rp->ai_protocol);
	       if (sfd == -1)
		       continue;

	       if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
		       break;                  

	       close(sfd);
       }

       if (rp == NULL) {               
	       MIN_WARN ("Could not connect %s", slave->he_.h_name);
	       return -1;
       }

       
       slave->fd_ = sfd;
       slave->reserved_ = 1;

       return 0;
}


/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
