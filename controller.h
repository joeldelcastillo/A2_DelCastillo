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

void SETUP_SOCKET_SERVER();

#endif