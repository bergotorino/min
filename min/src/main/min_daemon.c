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
#include <sys/wait.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <min_ipc_mechanism.h>
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
#define MIND_LOG(__str__, __arg__) \
	do {\
		tx = tx_create (__str__); \
		tx_c_append (tx, " ");	     \
		tx_c_append (tx, __arg__); \
		syslog (LOG_INFO, "%s", tx_share_buf (tx));	\
		tx_destroy (&tx);\
	} while (0)

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
int eapi_listen_socket, rcp_listen_socket;
int mins_running = 0;
int exit_ = 0;

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
LOCAL int create_listen_socket (unsigned short port);
/* ------------------------------------------------------------------------- */
LOCAL int poll_sockets (char *envp[]);
/* ------------------------------------------------------------------------- */
LOCAL void handle_sigchld (int sig);
/* ------------------------------------------------------------------------- */
LOCAL void handle_sigint (int sig);
      
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/** Creates a sockets to accept tcp connections
 * @return 0 or -1 on error
 */
LOCAL int create_listen_socket(unsigned short port)
{
	struct sockaddr_in in_addr;
        Text *tx;
	int sock;

	memset (&in_addr, 0x0, sizeof (struct sockaddr_in));
        in_addr.sin_port = htons (port); 
	in_addr.sin_family = AF_INET;
	in_addr.sin_addr.s_addr = INADDR_ANY;
	if ((sock = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
                MIND_LOG("Failed to create socket", 
			 strerror (errno));
                return -1;
        }

        if (bind (sock, 
		  (struct sockaddr *)&in_addr, sizeof (in_addr))  == -1) {
                MIND_LOG("Failed to bind to socket", 
			 strerror (errno));
		close (sock);
                return -1;
                
        }
        if (listen (sock, 1)) {
		MIND_LOG ("Listen failed", strerror (errno));
		close (sock);
		return -1;
	}

        MIND_LOG ("accepting"," connections");
	return sock;
}
/* ------------------------------------------------------------------------- */
/** Listens and accepts connections. Forks and execves min for each 
 *  new connection
 * @param envp the environment passed to main()
 */
LOCAL int poll_sockets (char *envp[])
{
	fd_set rd, wr, er;
	int nfds = 0;
	unsigned len;
	struct sockaddr_in client_addr;
	struct timeval tv;
        int ret, rcp_socket, eapi_socket, pid;
	char *args[2];
        Text *tx;

	rcp_listen_socket = create_listen_socket (MIN_TCP_PORT);
	eapi_listen_socket = create_listen_socket (MIN_EAPI_LISTEN_PORT);

	if (rcp_listen_socket < 0 || eapi_listen_socket < 0)
		return -1;

	args [0] = NEW2 (char, 100);
	args [1] = NEW2 (char, 100);

	sprintf (args [0], "%s", "/usr/bin/min");
	while (exit_ == 0 && rcp_listen_socket > 0 &&
	       eapi_listen_socket > 0) {
		nfds = 0;
		FD_ZERO (&rd);
		FD_ZERO (&wr);
		FD_ZERO (&er);
		FD_SET (rcp_listen_socket, &rd);
		FD_SET (eapi_listen_socket, &rd);
		nfds = MAX(nfds, rcp_listen_socket);
		nfds = MAX(nfds, eapi_listen_socket);
		tv.tv_sec = 0;
		tv.tv_usec = 100000;
		ret = select (nfds + 1, &rd, &wr, &er, &tv);
		
		if (FD_ISSET(rcp_listen_socket, &rd)) {
			memset (&client_addr, 0, len = sizeof(client_addr));
			rcp_socket = accept (rcp_listen_socket, 
					     (struct sockaddr *) &client_addr, 
				     &len);
			if (rcp_socket < 0) {
				MIND_LOG ("accept() failed", 
					 strerror (errno));
				return -1;
			}
			sprintf (args[1], "-m %u", rcp_socket);
			pid = fork ();
			switch (pid) {
			case -1:
				MIND_LOG ("Failed to create process !",
					 strerror (errno));
				break;
			case 0:
				close (rcp_listen_socket);
				execve (args[0], args, envp);
				return 0;
				break;
			default:
				sl_set_sighandler (SIGCHLD, handle_sigchld);
				close (rcp_socket);
				mins_running++;
				break;
			}

			MIND_LOG ("connect from",
				  inet_ntoa (client_addr.sin_addr));

			/* if (listen (rcp_listen_socket, 100)) {
				MIND_LOG("Listen failed", 
					 strerror (errno));
				close (rcp_listen_socket);
				return -1;
				} */
		}
		if (FD_ISSET(eapi_listen_socket, &rd)) {

			
			memset (&client_addr, 0, len = sizeof(client_addr));
			
			eapi_socket = accept (eapi_listen_socket, 
					     (struct sockaddr *) &client_addr, 
					      &len);

			if (eapi_socket < 0) {
				MIND_LOG ("accept() for eapi socket failed", 
					 strerror (errno));
				return -1;
			}

			sprintf (args[1], "-f %u", eapi_socket);
			pid = fork ();
			switch (pid) {
			case -1:
				MIND_LOG ("Failed to create process !",
					  strerror (errno));
				break;
			case 0:
				close (eapi_listen_socket);
				execve (args[0], args, envp);
				return 0;
				break;
			default:
				sl_set_sighandler (SIGCHLD, handle_sigchld);
				close (eapi_socket);
				mins_running++;
				break;
			}
			
			MIND_LOG ("EAPI connect from",
				  inet_ntoa (client_addr.sin_addr));


			if (listen (eapi_listen_socket, 100)) {
				MIND_LOG("Listen failed", 
					 strerror (errno));
				close (eapi_listen_socket);
				return -1;
			}
		}
		sleep (1);
	}

	close (eapi_listen_socket);
	close (rcp_listen_socket);

	return 0;
}
/* ------------------------------------------------------------------------- */
/** Signal handler for SIGCHLD
 * @param sig the signal number
 */
LOCAL void handle_sigchld (int sig)
{
        int             pid, status;
        while ((pid = waitpid (-1, &status, 0)) > 0) {
		usleep (500000);
        }

	mins_running--;

}
/* ------------------------------------------------------------------------- */
/** Signal handler for SIGINT and SIGHUP
 * @param sig the signal number
 */
LOCAL void handle_sigint (int sig)
{
	exit_ = 1;
}
/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** The main() function for min daemon
 *  @param argc argument count
 *  @param argv arguments table
 *  @param evnp environment
 */
int main (int argc, char *argv[], char *envp[])
{
	int retval;
	Text *tx;

	retval = daemon (0, 0);
	openlog ("MIND", LOG_PID | LOG_CONS, LOG_LOCAL0);
	if (retval < 0) {
		MIND_LOG ("daemon() failed! %s", strerror (errno));
		return retval;
	}

        sl_set_sighandler (SIGINT,  handle_sigint);
        sl_set_sighandler (SIGHUP,  handle_sigint);
        retval = poll_sockets (envp);
	MIND_LOG ("closing", "...");
	closelog();
	return retval;
}

/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
