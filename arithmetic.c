/*
 * Ex #2b: Arithmetic Operations Server (The host).
 * ==========================================================
 * Written by: Jacob Bondar.
 *
 * This program acts as a server that accepts arithmetic operation
 * requests from clients. It listens for client connections, processes
 * arithmetic expressions, and sends the calculated results back to the
 * clients. The server runs until terminated by a SIGINT signal.
 *
 * Input: Port number provided as a command-line argument.
 *        Clients send arithmetic expressions as strings.
 *
 * Output: Results of the arithmetic operations sent back to clients.
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

#define  MY_PORT   "19001"
#define  IP        "127.0.0.1"
const int NUM_OF_CLIENTS = 5;
const int MAX_SIZE = 1000;

//-------------- prototypes section ------------------------------------

void signal_handler(int sig);
void run(int main_socket);
int calc_math(const char buf[]);
int do_action(int first_value, int second_value, char action);

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
 * The function manages the server's main operations, including
 * accepting client connections, reading arithmetic expressions,
 * calculating results, and sending them back to the clients.
 * The function receives: the main socket descriptor.
 * The function returns: void.
 */
void run(int main_socket)
{
    fd_set 			   rfd;
    fd_set 			   c_rfd;
    int				   fd = 0;
    int                serving_socket = 0;
    int                rc = 0;
    int				   value = 0;
    char			   buf[MAX_SIZE];

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
                rc = read(fd, buf, sizeof(buf));

                if (rc == 0)
                {
                    close(fd);
                    FD_CLR(fd, &rfd);
                }

                else if (rc > 0)
                {
                    value = calc_math(buf);
                    snprintf(buf, MAX_SIZE, "%d", value);
                    write(fd, buf, sizeof(buf));
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
 * Parses an arithmetic expression and calculates the result.
 * The function receives: a string containing an arithmetic expression.
 * The function returns: the result of the arithmetic operation.
 */
int calc_math(const char buf[])
{
    int index = 0;
    int first_value = 0, second_value = 0;
    char action;
    char temp_hold_value[MAX_SIZE];

    first_value = atoi(buf);

    while (buf[index] != '+' &&
        buf[index] != '-' &&
        buf[index] != '*' &&
        buf[index] != '/')
    {
        index++;
    }

    action = buf[index];

    index += 2;
    int index_temp = 0;
    while (buf[index] != '\0')
    {
        temp_hold_value[index_temp] = buf[index];
        index_temp++;
        index++;
    }
    temp_hold_value[index_temp] = '\0';
    second_value = atoi(temp_hold_value);

    int num = do_action(first_value, second_value, action);

    return num;
}

//----------------------------------------------------------------------

/*
 * Performs the specified arithmetic operation on two integers.
 * The function receives: two integers and an operator character.
 * The function returns: the result of the arithmetic operation.
 */
int do_action(int first_value, int second_value, char action)
{
    switch (action)
    {
    case '+':
    {
        return first_value + second_value;
        break;
    }

    case '-':
    {
        return first_value - second_value;
        break;
    }

    case '*':
    {
        return first_value * second_value;
        break;
    }

    case '/':
    {
        return first_value / second_value;
        break;
    }
    }
    return 0;
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
