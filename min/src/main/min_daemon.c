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
 *  @file       min_main.c
 *  @version    0.1
 *  @brief      This file contains implementation of MIN daemoin
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <min_logger.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* None */
/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
int rcp_listen_socket;


/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/** Creates a socket to accept tcp connections
 * @return 0 or -1 on error
 */
int create_listen_socket()
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
		close (rcp_listen_socket);
		rcp_listen_socket = -1;
                return -1;
                
        }
        if (listen (rcp_listen_socket, 1)) {
		MIN_FATAL ("Listen failed %s", strerror (errno));
		close (rcp_listen_socket);
		rcp_listen_socket = -1;
		return -1;
	}

        MIN_INFO ("accepting connections on port %d\n", MIN_TCP_PORT);

	return 0;
}


LOCAL int poll_sockets (char *envp[])
{
	fd_set rd, wr, er;
	int nfds = 0;
	unsigned len;
	struct sockaddr_in client_addr;
	struct timeval tv;
        int ret, rcp_socket, pid;
	char args [2][100];

	create_listen_socket ();
	sprintf (args [0], "%s", "/usr/bin/min.bin");
	while (rcp_listen_socket > 0) {
		FD_ZERO (&rd);
		FD_ZERO (&wr);
		FD_ZERO (&er);
		FD_SET (rcp_listen_socket, &rd);
		nfds = MAX(nfds, rcp_listen_socket);
		tv.tv_sec = 0;
		tv.tv_usec = 100000;
		ret = select (nfds + 1, &rd, &wr, &er, &tv);
		
		if (FD_ISSET(rcp_listen_socket, &rd)) {
			memset (&client_addr, 0, len = sizeof(client_addr));
			rcp_socket = accept (rcp_listen_socket, 
					     (struct sockaddr *) &client_addr, 
				     &len);
			if (rcp_socket < 0) {
				MIN_FATAL ("accept() failed %s", 
					   strerror (errno));
				return -1;
			}
			sprintf (args[1], "-f %u", rcp_socket);
			pid = fork ();
			switch (pid) {
			case -1:
				MIN_FATAL ("Failed to create process !");
				break;
			case 0:
				close (rcp_listen_socket);
				execve (args[0], args, envp);
				MIN_FATAL ("Failed to min engine :%s",
					   strerror (errno));
				break;
			default:
				close (rcp_socket);
				break;
			}

			MIN_INFO ("connect from %s\n",
				  inet_ntoa (client_addr.sin_addr));
			
			if (listen (rcp_listen_socket, 1)) {
				MIN_FATAL ("Listen failed %s", 
					   strerror (errno));
				return -1;
			}
		}
		
	}

	return 0;
}


/* ------------------------------------------------------------------------- */
int main (int argc, char *argv[], char *envp[])
{
	int retval;

        min_log_open ("MIND", 3);
	/* daemon */
	MIN_INFO ("started");
        retval = poll_sockets (envp);
	MIN_INFO ("exiting");
	min_log_close();
	return retval;
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* None */

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
