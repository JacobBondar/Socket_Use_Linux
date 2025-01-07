/*
 * Ex #2c: Client Program for Prime Check and Arithmetic Operations
 * =========================================================
 * Written by: Jacob Bondar.
 *
 * This program acts as a client that connects to two separate servers:
 * 1. A server for checking if a number is prime.
 * 2. A server for performing arithmetic operations.
 *
 * The client interacts with the user via a simple menu-driven
 *  interface:
 * - 'p': Sends a number to the prime-check server.
 * - 'a': Sends an arithmetic equation to the arithmetic server.
 * - 'e': Exits the program.
 *
 * Input: Server IP addresses, ports, and user input.
 * Output: Results from the servers.
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

//-------------- const and global section ------------------------------

const int ARR_SIZE = 1000;

//-------------- prototypes section ------------------------------------

void run(int my_socket_prime, int my_socket_act);
void do_prime(int my_socket);
void do_math(int my_socket);
void reset_and_define(struct addrinfo* con_kind);
void get_adress(char* ip, char* port, struct addrinfo* con_kind,
    struct addrinfo** addr_info_res);
int get_socket(struct addrinfo* addr_info_res);
void connect_socket(int my_socket, struct addrinfo* addr_info_res);

//-------------- main --------------------------------------------------

int main(int argc, char* argv[])
{
    if (argc != 5)
    {
        perror("Not enough arguments!");
        exit(EXIT_FAILURE);
    }

    int               my_socket_prime, my_socket_act;
    struct addrinfo   con_kind_prime, con_kind_act,
        * addr_info_res_prime = NULL,
        * addr_info_res_act = NULL;

    reset_and_define(&con_kind_prime);
    reset_and_define(&con_kind_act);

    get_adress(argv[1], argv[2], &con_kind_prime, &addr_info_res_prime);
    get_adress(argv[3], argv[4], &con_kind_act, &addr_info_res_act);

    my_socket_prime = get_socket(addr_info_res_prime);
    my_socket_act = get_socket(addr_info_res_act);

    connect_socket(my_socket_prime, addr_info_res_prime);
    connect_socket(my_socket_act, addr_info_res_act);

    run(my_socket_prime, my_socket_act);

    close(my_socket_prime);
    close(my_socket_act);

    freeaddrinfo(addr_info_res_prime);
    freeaddrinfo(addr_info_res_act);

    return EXIT_SUCCESS;
}

//----------------------------------------------------------------------

/*
 * Handles the main client logic by providing a menu interface to the user.
 * The function allows the user to:
 * - Check if a number is prime ('p').
 * - Perform arithmetic calculations ('a').
 * - Exit the program ('e').
 *
 * Input: Two socket descriptors for prime-check and arithmetic servers.
 * Output: void.
 */
void run(int my_socket_prime, int my_socket_act)
{
    while (1)
    {
        char c;
        scanf("%c", &c);

        if (c == 'p')
        {
            do_prime(my_socket_prime);
        }

        else if (c == 'a')
        {
            do_math(my_socket_act);
        }

        else if (c == 'e')
        {
            break;
        }
    }
}

//----------------------------------------------------------------------

/*
 * Sends a number to the prime-check server and displays the result.
 *
 * Input: Socket descriptor for the prime-check server.
 * Output: void.
 */
void do_prime(int my_socket)
{
    int value = 0;

    puts("Please enter an integer:");
    scanf("%d", &value);

    write(my_socket, &value, sizeof(value));

    if (read(my_socket, &value, sizeof(value)) < 0)
    {
        perror("read() failed:");
        exit(EXIT_FAILURE);
    }

    printf("%d\n", value);
}

//----------------------------------------------------------------------

/*
 * Sends an arithmetic equation to the arithmetic server and displays the result.
 *
 * Input: Socket descriptor for the arithmetic server.
 * Output: void.
 */
void do_math(int my_socket)
{
    char buf[ARR_SIZE];

    puts("Please enter an aritmetic equation:");
    getchar(); // remove enter
    fgets(buf, sizeof(buf), stdin);

    write(my_socket, buf, sizeof(buf));
    if (read(my_socket, buf, sizeof(buf)) < 0)
    {
        perror("read() failed:");
        exit(EXIT_FAILURE);
    }

    printf("%d\n", atoi(buf));
}

//----------------------------------------------------------------------

/*
 * Initializes the addrinfo structure to set the desired connection
 * 	type.
 *
 * Input: Pointer to an addrinfo structure.
 * Output: void.
 */
void reset_and_define(struct addrinfo* con_kind)
{
    memset(con_kind, 0, sizeof(*con_kind));
    con_kind->ai_family = AF_UNSPEC;
    con_kind->ai_socktype = SOCK_STREAM;
}

//----------------------------------------------------------------------

/*
 * Resolves the IP address and port into an addrinfo structure.
 *
 * Input: IP address, port, addrinfo structure, and a pointer to store
 *  results.
 * Output: void.
*/
void get_adress(char* ip, char* port, struct addrinfo* con_kind,
    struct addrinfo** addr_info_res)
{
    int rc = 0;
    if ((rc = getaddrinfo(ip, port,
        con_kind,
        addr_info_res) != 0))
    {
        perror("Can't get adress: ");
        exit(EXIT_FAILURE);
    }
}

//----------------------------------------------------------------------

/*
 * Creates a socket using the provided addrinfo structure.
 *
 * Input: addrinfo structure containing connection information.
 * Output: The socket descriptor.
 */
int get_socket(struct addrinfo* addr_info_res)
{
    int my_socket = socket(addr_info_res->ai_family,
        addr_info_res->ai_socktype,
        addr_info_res->ai_protocol);

    if (my_socket < 0) {
        perror("socket: allocation failed");
        exit(EXIT_FAILURE);
    }
    return my_socket;
}

//----------------------------------------------------------------------

/*
 * Connects a socket to the specified address and port.
 *
 * Input: Socket descriptor and addrinfo structure with connection
 * 	details.
 * Output: void.
 */
void connect_socket(int my_socket, struct addrinfo* addr_info_res)
{
    int rc = connect(my_socket,
        addr_info_res->ai_addr,
        addr_info_res->ai_addrlen);

    if (rc)
    {
        perror("Can't connect: ");
        exit(EXIT_FAILURE);
    }
}
