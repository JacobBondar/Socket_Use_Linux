/*
 * Ex #1b: A client-side program for sending values (The client).
 * ==========================================================
 * Written by: Jacob Bondar.
 *
 * This program acts as a client that connects to the server and
 * sends random values within a specified range. It tracks the
 * number of values sent and counts the number of successful
 * insertions (acknowledged by the server).
 *
 * Input: Random values generated locally and server's responses.
 *
 * Output: The total number of values sent and the number of successful
 *         insertions.
 */

 //-------------- include section ---------------------------------------


#include <stdio.h>
#include <stdlib.h>         
#include <string.h>         
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <netdb.h>

//-------------- const section -----------------------------------------

const int ARR_SIZE = 1000;

//-------------- prototypes section ------------------------------------

void run(int my_socket);

//-------------- main --------------------------------------------------

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        perror("Not enough arguments!");
        exit(EXIT_FAILURE);
    }
    srand(atoi(argv[1]));
    char ip[ARR_SIZE];
    char dest_port[ARR_SIZE];

    strcpy(ip, argv[2]);
    strcpy(dest_port, argv[3]);

    int               rc;
    int               my_socket;
    struct addrinfo   con_kind,
        * addr_info_res;

    memset(&con_kind, 0, sizeof(con_kind));
    con_kind.ai_family = AF_UNSPEC;
    con_kind.ai_socktype = SOCK_STREAM;

    if ((rc = getaddrinfo(ip, dest_port,
        &con_kind,
        &addr_info_res) != 0))
    {
        perror("Can't get adress: ");
        exit(EXIT_FAILURE);
    }

    my_socket = socket(addr_info_res->ai_family,
        addr_info_res->ai_socktype,
        addr_info_res->ai_protocol);

    if (my_socket < 0) {
        perror("socket: allocation failed");
        exit(EXIT_FAILURE);
    }

    rc = connect(my_socket,
        addr_info_res->ai_addr,
        addr_info_res->ai_addrlen);
    if (rc)
    {
        perror("Can't connect: ");
        exit(EXIT_FAILURE);
    }

    run(my_socket);

    close(my_socket);
    freeaddrinfo(addr_info_res);

    return EXIT_SUCCESS;
}

//----------------------------------------------------------------------

/* The function communicates with the server, sending random values
 *  and tracking responses. It stops when the server sends a termination
 *  signal (-1).
 * The function receives: the socket descriptor.
 * The function returns: void.
 */
void run(int my_socket)
{
    int temp = -1;

    // Notify server of client presence
    write(my_socket, &temp, sizeof(temp));

    // Wait for server's acknowledgment
    if (read(my_socket, &temp, sizeof(temp)) < 0)
    {
        perror("read() failed:");
        exit(EXIT_FAILURE);
    }

    int result = 0, value = 0, one_got = 0, counter = 0;
    while (result != -1)
    {
        value = rand() % ARR_SIZE;

        write(my_socket, &value, sizeof(value));
        if (read(my_socket, &value, sizeof(value)) < 0)
        {
            perror("read() failed:");
            exit(EXIT_FAILURE);
        }

        result = value;
        counter++;

        if (result == 1) // Server acknowledges successful insertion
        {
            one_got++;
        }
    }

    printf("%d %d\n", counter, one_got);
}
