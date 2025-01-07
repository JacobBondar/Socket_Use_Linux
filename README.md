# Socket_Use_Linux

File: connect_Values_server.c 

Ex #1a: A server-side program for counting occurrences (The host).
==================================================================
Written by: Jacob Bondar.

This program acts as a server that accepts connections from clients 
 and processes incoming data. It counts occurrences of values sent 
 by the clients in an array. The program ends when the array is 
 completely filled. It also manages client connections using select()
 and ensures proper cleanup upon termination.

Compile: gcc -Wall connect_Values_server.c –o connect_Values_server
Run: ./connect_Values_server

Input: Values sent by clients over socket communication connections.
 
Output: The minimum and maximum values in the array.

------------------------------------------------------------------

File: connect_Values_client.c 

Ex #1b: A client-side program for sending values (The client).
==============================================================
Written by: Jacob Bondar.

This program acts as a client that connects to the server and 
 sends random values within a specified range. It tracks the 
 number of values sent and counts the number of successful 
 insertions (acknowledged by the server).

Compile: gcc -Wall connect_Values_client.c –o connect_Values_client
Run: ./connect_Values_client <seed> <ip of server> <port>

Input: Random values generated locally and server's responses.
 
Output: The total number of values sent and the number of successful 
         insertions.

------------------------------------------------------------------

File: prime.c 

Ex #2a: Prime Number Checker (The host).
==========================================================
Written by: Jacob Bondar.

This program acts as a host server for checking whether numbers 
 sent by clients are prime. It listens for client connections and 
 processes incoming numbers by determining their primality. 
 The program continues until terminated with a signal (SIGINT).

Compile: gcc -Wall prime.c –o prime
Run: ./prime <port>

Input: Port number provided as a command-line argument.

Output: Results sent back to the client indicating whether a 
         number is prime (1) or not (0).

------------------------------------------------------------------

File: arithmetic.c 

Ex #2b: Arithmetic Operations Server (The host).
==========================================================
Written by: Jacob Bondar.

This program acts as a server that accepts arithmetic operation 
 requests from clients. It listens for client connections, processes 
 arithmetic expressions, and sends the calculated results back to the 
 clients. The server runs until terminated by a SIGINT signal.

Compile: gcc -Wall arithmetic.c –o arithmetic
Run: ./arithmetic <port>

Input: Port number provided as a command-line argument.
        Clients send arithmetic expressions as strings.
 
Output: Results of the arithmetic operations sent back to clients.

------------------------------------------------------------------

File: prime_Arithmetic_client.c 

Ex #2c: Client Program for Prime Check and Arithmetic Operations
==========================================================
Written by: Jacob Bondar.

This program acts as a client that connects to two separate servers:
 1. A server for checking if a number is prime.
 2. A server for performing arithmetic operations.

 The client interacts with the user via a simple menu-driven 
  interface:
 - 'p': Sends a number to the prime-check server.
 - 'a': Sends an arithmetic equation to the arithmetic server.
 - 'e': Exits the program.

Compile: gcc -Wall prime_Arithmetic_client.c –o prime_Arithmetic_client
Run: ./prime_Arithmetic_client <ip prime> <port prime> <ip arithmetic> <port arithmetic>

Input: Server IP addresses, ports, and user input.
 
Output: Results from the servers.
