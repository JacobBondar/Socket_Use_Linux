/*
 * Ex #2a: Prime Number Checker (The host).
 * ==========================================================
 * Written by: Jacob Bondar.
 *
 * This program acts as a host server for checking whether numbers
 * sent by clients are prime. It listens for client connections and
 * processes incoming numbers by determining their primality.
 * The program continues until terminated with a signal (SIGINT).
 *
 * Input: Port number provided as a command-line argument.
 *
 * Output: Results sent back to the client indicating whether a
 *         number is prime (1) or not (0).
 */

 //-------------- include section ---------------------------------------

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <netdb.h> 
#include <arpa/inet.h> 
#include <stdbool.h>
#include <signal.h>


//-------------- const and global section ------------------------------

struct addrinfo* addr_info_res;

#define  IP        "127.0.0.1"
const int NUM_OF_CLIENTS = 5;
const int MAX_SIZE = 1000;
const int PRIME = 1;
const int NOTPRIME = 0;

//-------------- prototypes section ------------------------------------

void signal_handler(int sig);
void run(int main_socket);
int find_if_prime(int value);

//-------------- main --------------------------------------------------

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		perror("Not enough arguments");
		exit(EXIT_FAILURE);
	}

	signal(SIGINT, signal_handler);

	int                rc;
	int                main_socket;
	struct addrinfo    con_kind;

	memset(&con_kind, 0, sizeof(con_kind));
	con_kind.ai_family = AF_UNSPEC;
	con_kind.ai_socktype = SOCK_STREAM;
	con_kind.ai_flags = AI_PASSIVE;

	if ((rc = getaddrinfo(IP, argv[1],
		&con_kind,
		&addr_info_res)) != 0)
	{
		perror("Can't get adress: ");
		exit(EXIT_FAILURE);
	}

	main_socket = socket(addr_info_res->ai_family,
		addr_info_res->ai_socktype,
		addr_info_res->ai_protocol);

	if (main_socket < 0)
	{
		perror("socket: allocation failed");
		exit(EXIT_FAILURE);
	}

	rc = bind(main_socket, addr_info_res->ai_addr,
		addr_info_res->ai_addrlen);
	if (rc)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	rc = listen(main_socket, NUM_OF_CLIENTS);
	if (rc)
	{
		perror("listen failed");
		exit(EXIT_FAILURE);
	}

	run(main_socket);
	return(EXIT_SUCCESS);
}

//----------------------------------------------------------------------

/*
 * The function manages the server's main operations. It listens for
 * client connections, reads data, checks for primality, and sends
 * the result back to the client.
 * The function receives: the main socket descriptor.
 * The function returns: void.
 */
void run(int main_socket)
{
	fd_set 			   rfd;
	fd_set 			   c_rfd;
	int				   fd = 0;
	int				   value = 0;
	int                serving_socket = 0;
	int                rc = 0;

	struct sockaddr_storage client_info;
	socklen_t 		   client_info_size;

	FD_ZERO(&rfd);
	FD_SET(main_socket, &rfd);

	while (1)
	{
		c_rfd = rfd;

		rc = select(getdtablesize(), &c_rfd, NULL, NULL, NULL);

		if (rc < 0)
		{
			perror("select failed:");
			exit(EXIT_FAILURE);
		}

		if (FD_ISSET(main_socket, &c_rfd))
		{
			serving_socket = accept(main_socket,
				(struct sockaddr*)&client_info,
				&client_info_size);
		}

		if (serving_socket >= 0)
		{
			FD_SET(serving_socket, &rfd);
		}

		for (fd = main_socket + 1; fd < getdtablesize(); fd++)
		{
			if (FD_ISSET(fd, &c_rfd)) // if exists
			{
				rc = read(fd, &value, sizeof(value));

				if (rc == 0)
				{
					close(fd);
					FD_CLR(fd, &rfd);
				}

				else if (rc > 0)
				{
					value = find_if_prime(value);
					write(fd, &value, sizeof(value));
				}
				else
				{
					perror("read() failed:");
					exit(EXIT_FAILURE);
				}
			}
		}
	}
}


//----------------------------------------------------------------------

/*
 * Determines whether a number is prime.
 * The function receives: an integer to check.
 * The function returns: 1 if the number is prime, 0 otherwise.
 */
int find_if_prime(int value)
{
	for (int num = 2; num <= value / 2; num++)
	{
		if (value % num == 0)
		{
			return NOTPRIME;
		}
	}
	return PRIME;
}

//----------------------------------------------------------------------

/*
 * Handles SIGINT signals to free allocated resources and terminate
 * the program gracefully.
 * The function receives: the signal number.
 * The function returns: void.
 */
void signal_handler(int sig)
{
	freeaddrinfo(addr_info_res);
	exit(EXIT_SUCCESS);
}
