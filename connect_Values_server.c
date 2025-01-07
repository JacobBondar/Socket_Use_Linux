/*
 * Ex #1a: A server-side program for counting occurrences (The host).
 * =====================================================================
 * Written by: Jacob Bondar.
 *
 * This program acts as a server that accepts connections from clients
 * and processes incoming data. It counts occurrences of values sent
 * by the clients in an array. The program ends when the array is
 * completely filled. It also manages client connections using select()
 * and ensures proper cleanup upon termination.
 *
 * Input: Values sent by clients over socket communication connections.
 *
 * Output: The minimum and maximum values in the array.
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

//-------------- const and define section ------------------------------

#define  MY_PORT   "19000"
#define  IP        "127.0.0.1"
const int ARR_SIZE = 1000;
const int NUM_OF_CLIENTS = 3;

//-------------- prototypes section ------------------------------------

void run(int main_socket);
bool get_clients(int value, int* num_of_clients, int main_socket,
	int fd);
bool finised_fill(int counter, int fd, int main_socket, int value,
	bool* end);
void print_result(int array[]);

//-------------- main --------------------------------------------------

int main()
{
	int                rc;
	int                main_socket;
	struct addrinfo    con_kind,
		* addr_info_res;

	memset(&con_kind, 0, sizeof(con_kind));
	con_kind.ai_family = AF_UNSPEC;
	con_kind.ai_socktype = SOCK_STREAM;
	con_kind.ai_flags = AI_PASSIVE;

	if ((rc = getaddrinfo(IP, MY_PORT,
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
	freeaddrinfo(addr_info_res);

	return(EXIT_SUCCESS);
}

//----------------------------------------------------------------------

/* The function runs the main server loop, managing client connections
 *  and processing their inputs. It uses select() to monitor multiple
 *  sockets and handles reading, writing, and closing sockets as needed.
 * The function receives: the main socket descriptor.
 * The function returns: void.
 */
void run(int main_socket)
{
	fd_set 			   rfd;
	fd_set 			   c_rfd;
	int				   fd = 0;
	int				   value = 0;
	int				   counter = 0;
	int                serving_socket = 0;
	int                rc = 0;
	int 			   num_of_clients = 0;
	int array[ARR_SIZE];
	struct sockaddr_storage client_info;
	socklen_t 		   client_info_size;

	FD_ZERO(&rfd);
	FD_SET(main_socket, &rfd);

	memset(array, 0, sizeof(array));
	bool end = false;


	while (!end)
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

				if (rc > 0)
				{
					if (get_clients(value, &num_of_clients, main_socket,
						fd)) {
					}

					else
					{
						if (finised_fill(counter, fd, main_socket, value,
							&end))
						{
							break;
						}

						else
						{
							if (array[value]++ == 0)
							{
								value = 1;
								counter++;
							}

							else
							{
								value = 0;
							}
						}
						write(fd, &value, sizeof(value));
					}
				}
				else
				{
					perror("read() failed:");
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	print_result(array);
}

//----------------------------------------------------------------------

/* The function tracks the number of connected clients. Once the
 *  required number of clients is reached, it notifies all clients.
 * The function receives: the value read, pointer to client counter,
 *  main socket descriptor, and the file descriptor of the client.
 * The function returns: true if the client count is reached;
 *  otherwise false.
 */
bool get_clients(int value, int* num_of_clients, int main_socket,
	int fd)
{
	if (value == -1)
	{
		(*num_of_clients)++;
		if ((*num_of_clients) == NUM_OF_CLIENTS)
		{
			for (fd = main_socket + 1; fd < getdtablesize(); fd++)
			{
				write(fd, &value, sizeof(value));
			}
		}
		return true;
	}
	return false;
}

//----------------------------------------------------------------------

/* The function checks if the array is fully populated and ends the
 *  server loop if true. It also closes all client connections.
 * The function receives: the current counter, file descriptor,
 *  main socket, the value, and a pointer to the end flag.
 * The function returns: true if the loop ends; otherwise false.
 */
bool finised_fill(int counter, int fd, int main_socket, int value,
	bool* end)
{
	if (counter == ARR_SIZE)
	{
		for (fd = main_socket + 1; fd < getdtablesize(); fd++)
		{
			value = -1;
			write(fd, &value, sizeof(value));
			close(fd);
		}
		*end = true;
		return true;
	}
	return false;
}

//----------------------------------------------------------------------

/* The function prints the minimum and maximum counts from the array.
 * The function receives: the array of counts.
 * The function returns: void.
 */
void print_result(int array[])
{
	int min_value = 2, max_value = 0;

	for (int cell = 0; cell < ARR_SIZE; cell++)
	{
		if (array[cell] < min_value)
		{
			min_value = array[cell];
		}

		else if (array[cell] > max_value)
		{
			max_value = array[cell];
		}
	}
	printf("%d %d\n", min_value, max_value);
}
