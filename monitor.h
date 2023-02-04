#ifndef _MONITOR_H_
#define _MONITOR_H_
#include "list.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUFLEN 512 // Max length of buffer
#define PORT 8888  // The port on which to listen for incoming data


typedef struct Message_s Message;
struct Message_s
{
    int portNumber_from;
    int portNumber_to;
    // char[BUFLEN] machineName_from;
    // char[BUFLEN] machineName_to;
    // char[BUFLEN] data;
    
};

typedef struct Monitor_s Monitor;
struct Monitor_s
{
    
    List messages_send;
    List messages_receive;
    
};

// set the server environment
void setUp_Socket_Server();

void sendMessage(int PORT_OTHER);


// listening data
void listen_Socket();

// send error
void die(char *s);

// decode message received
// Message decode_Message(char[] message);

// Send Message
// int send_Message(Message message);







#endif