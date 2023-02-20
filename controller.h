#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_
#include "list.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h> //For errno - the error number
#include <netdb.h>	//hostent

#define BUFLEN 512 // Max length of buffer
#define PORT 8888  // The port on which to listen for incoming data


typedef struct Message_s Message;
struct Message_s
{
    int portNumber_from;
    int portNumber_to;
    char data[BUFLEN];
};

// set the server environment
void setUp_Socket_Server();

// One of the threads does nothing other than await input from the keyboard
void *await_Input(void *vargp);

// Does nothing other than await a UDP datagram
void *await_UDP_Datagram(void *vargp);

//  Prints characters to the screen. 
void *print_Characters(void *vargp);

// Sends data to the remote UNIX process over the network using UDP
void *send_Message(void *vargp);

//
void SETUP_SOCKET_SERVER();

// listening data
void listen_Socket();

// send error
void die(char *s);

// decode message received
Message receive_Message(char message[]);








#endif