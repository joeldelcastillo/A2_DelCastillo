
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "controller.h"
#include <string.h>
#include "list.h"


#define SERVER "127.0.0.1"
#define BUFLEN 512 // Max length of buffer
int MY_PORT;  // The port on which to send data
int OTHER_PORT;
char OTHER_CPU[20];
int otherPort;

struct sockaddr_in si_me;
struct sockaddr_in si_other;
int s, i, slen = sizeof(si_other), recv_len;
char buf[BUFLEN];
char message[BUFLEN];
bool IS_READY = false;

typedef struct Controller_s Controller;
struct Controller_s
{
    List messages_send;
    List messages_receive;
};

void die(char *s)
{
	perror(s);
	exit(1);
}

int hostname_to_ip(char * hostname , char* ip)
{
	struct hostent *he;
	struct in_addr **addr_list;
	int i;
		
	if ( (he = gethostbyname( hostname ) ) == NULL) 
	{
		// get the host info
		herror("gethostbyname");
		return 1;
	}

	addr_list = (struct in_addr **) he->h_addr_list;
	
	for(i = 0; addr_list[i] != NULL; i++) 
	{
		//Return the first one;
		strcpy(ip , inet_ntoa(*addr_list[i]) );
		return 0;
	}
	
	return 1;
}



void SETUP_OTHER_PORT(int port, char *cpu){
    OTHER_PORT = port;
    printf("CPU: %s \n" , cpu);
    char host[] = "asb9700u-g05.csil.sfu.ca";
    char ip[100];
    hostname_to_ip(host , ip);
	printf("%s resolved to %s" , host , ip);
	
	printf("\n");

    // zero out the structure
    memset((char *)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_addr.s_addr = inet_addr(ip);
    si_other.sin_port = htons(port);

    // if (inet_aton(SERVER, &si_other.sin_addr) == 0)
    // {
    //     fprintf(stderr, "inet_aton() failed\n");
    //     // exit(1);
    // }
}


void SETUP_MY_PORT(int port){
    MY_PORT = port;
	memset((char *)&si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(port);
    // Adress to accept any incoming messages
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
}


void SETUP_SOCKET_SERVER(int MYPORT, int OTHERPORT, char *OTHERCPU ){

    pthread_t tid;
    // create a UDP socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		die("socket");

    SETUP_OTHER_PORT(OTHERPORT, OTHERCPU);
    SETUP_MY_PORT(MYPORT);

    // bind socket to port
    if(bind(s, (struct sockaddr *)&si_me, sizeof(si_me)) == -1){
       die("bind"); 
    }
    
    printf("MY_PORT: %d \n", MY_PORT);

    // Create threads
    pthread_create(&tid, NULL, await_Input, (void *)&tid);

    pthread_exit(NULL);
}


void *await_Input(void *vargp){
    pthread_t tid;
    pthread_create(&tid, NULL, send_Message, (void *)&tid);
}


void *send_Message(void *vargp){

	// keep listening for data
	while (1)
	{
		printf("Enter message : ");
        gets(message);
		fflush(stdout);

        // send the message
        if (sendto(s, message, strlen(message), 0, (struct sockaddr *)&si_other, slen) == -1)
        {
            die("sendto()");
        }
        // receive a reply and print it
        // clear the buffer by filling null, it might have previously received data
        memset(buf, '\0', BUFLEN);

		// try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&si_other, &slen)) == -1)
		{
			die("recvfrom()");
		}

        puts(buf);

		// print details of the client/peer and the data received
		printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		printf("Data: %s\n", buf);
	}
    close(s);
}



// Message receive_Message(char message[]){

// }




// decode message received
// Message decode_Message(char[] message);

// Send Message
// int send_Message(Message message);

// bool keyboard[256] = {0};

int TEST_SUCCESS = 0;
List *myLists[3];

bool compare(void *pItem, void *pComparisonArg)
{
    return pItem == pComparisonArg;
}

void freeItem(void *pItem)
{
    free(pItem);
}

void print_Pool_of_Nodes()
{
    printf("Pool of Nodes: ");
    for (int i = 0; i < LIST_MAX_NUM_NODES; i++)
    {
        if (i % 10 == 0)
            printf("\n");
        if (getNodeFromPool(i)->element == NULL)
            printf("N  ");
        else
            printf("%-2d ", *(int *)getNodeFromPool(i)->element);
    }
    printf("\n\n");
}

void print_Pool_of_Lists()
{
    printf("Pool of Lists:  ");
    printf("\n");
    printf("[ ");
    for (int i = 0; i < LIST_MAX_NUM_HEADS; i++)
    {
        if (getListFromPool(i)->inUse == true)
            printf("%-3d ", getListFromPool(i)->size);
        else
            printf("N   ");
    }
    printf(" ]");
    printf("\n\n");
}

void print_List(List *pList)
{
    Node *pNode = List_curr(pList);
    List_first(pList);
    printf("\n{");
    while (pList->current != NULL)
    {
        if (pNode == pList->current)
        {
            printf("*");
        }
        printf("%-2d ", *(int *)pList->current->element);
        List_next(pList);
    }
    printf("} \n");
    pList->current = pNode;
}

void print_Backwards(List *pList)
{
    Node *pNode = List_curr(pList);
    List_last(pList);
    printf("\n {");
    while (pList->current != NULL)
    {
        printf(" %d ", *(int *)pList->current->element);
        List_prev(pList);
    }
    printf("} \n");
    pList->current = pNode;
}