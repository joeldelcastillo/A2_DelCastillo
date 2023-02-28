#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "controller.h"
#include <string.h>
#include "list.h"

int MY_PORT;
int OTHER_PORT;
char OTHER_CPU[20];

struct sockaddr_in si_me;
struct sockaddr_in si_other;
int s, i, slen = sizeof(si_other), recv_len;

static pthread_t tid1, tid2, tid3, tid4, tid5;

static pthread_cond_t s_buffAvailtoSend_CondVar = PTHREAD_COND_INITIALIZER;
static pthread_cond_t s_itemAvailtoSend_CondVar = PTHREAD_COND_INITIALIZER;
static pthread_cond_t s_buffAvailtoReceive_CondVar = PTHREAD_COND_INITIALIZER;
static pthread_cond_t s_itemAvailtoReceive_CondVar = PTHREAD_COND_INITIALIZER;

static pthread_mutex_t s_syncOkToSend_Mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_syncOkToReceive_Mutex = PTHREAD_MUTEX_INITIALIZER;
static bool CONTINUE = true;

typedef struct Controller_s Controller;
struct Controller_s
{
    List messages_send;
    List messages_receive;
};

Controller buffer;

void die(char *s)
{
    perror(s);
    exit(1);
}

void freeItem(void *pItem)
{
    free(pItem);
    pItem = NULL;
}

// gets the IP of FDQN CSIL server
int hostname_to_ip(char *hostname, char *ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ((he = gethostbyname(hostname)) == NULL)
    {
        // get the host info
        herror("gethostbyname");
        return 1;
    }

    addr_list = (struct in_addr **)he->h_addr_list;

    for (i = 0; addr_list[i] != NULL; i++)
    {
        strcpy(ip, inet_ntoa(*addr_list[i]));
        return 0;
    }

    return 1;
}

void SETUP_OTHER_PORT(int port, char *cpu)
{
    OTHER_PORT = port;
    printf("CPU: %s \n", cpu);
    char host[20];
    strcpy(host, cpu);
    strcat(host, ".cs.surrey.sfu.ca");
    char ip[100];
    hostname_to_ip(host, ip);
    printf("%s resolved to %s", host, ip);

    printf("\n");

    memset((char *)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_addr.s_addr = inet_addr(ip);
    si_other.sin_port = htons(port);
}

void SETUP_MY_PORT(int port)
{
    MY_PORT = port;
    memset((char *)&si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port);
    // Adress to accept any incoming messages
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
}

// Thread that only waits for Keyboard Input and adds it to the shared list
void *await_Input_Thread(void *vargp)
{
    char *message = NULL;
    while (true)
    {
        message = malloc(sizeof(char[BUFLEN]));
        fgets(message, BUFLEN, stdin);

        // Critical Section
        pthread_mutex_lock(&s_syncOkToSend_Mutex);
        {
            while (buffer.messages_send.count == LIST_MAX_NUM_NODES)
            {
                pthread_cond_wait(&s_buffAvailtoSend_CondVar, &s_syncOkToSend_Mutex);
            }
            List_append(&buffer.messages_send, message);
            pthread_cond_signal(&s_itemAvailtoSend_CondVar);
        }
        pthread_mutex_unlock(&s_syncOkToSend_Mutex);
        // End of critical section

        if (strcmp("!\n", message) == 0)
            CONTINUE = false;
    }
}

// Thread that sends a message via UDP once the shared list has an item
void *send_Message_Thread(void *vargp)
{
    char *pMessage = NULL;
    char message[BUFLEN] = "";
    while (true)
    {
        // Critical Section
        pthread_mutex_lock(&s_syncOkToSend_Mutex);
        {
            while (buffer.messages_send.count == 0)
            {
                pthread_cond_wait(&s_itemAvailtoSend_CondVar, &s_syncOkToSend_Mutex);
            }
 
            pMessage = List_trim(&buffer.messages_send);

            for (int i = 0; i < BUFLEN; i++)
                message[i] = pMessage[i];

            free(pMessage);
            pMessage = NULL;
            pthread_cond_signal(&s_buffAvailtoSend_CondVar);
        }
        pthread_mutex_unlock(&s_syncOkToSend_Mutex);
        // End of Critical Section

        if (sendto(s, message, strlen(message), 0, (struct sockaddr *)&si_other, slen) == -1)
        {
            printf("died");
            die("sendto()");
        }
    }
}

// Thread that listen for incoming UDP messages and adds it to shared List
void *receive_Message_Thread(void *vargp)
{

    while (true)
    {
        char *message = malloc(sizeof(char[BUFLEN]));

        if ((recv_len = recvfrom(s, message, BUFLEN, 0, (struct sockaddr *)&si_other, &slen)) == -1)
            die("recvfrom()");

        // Critical Section
        pthread_mutex_lock(&s_syncOkToReceive_Mutex);
        {
            while (buffer.messages_receive.count == LIST_MAX_NUM_NODES)
            {
                pthread_cond_wait(&s_buffAvailtoReceive_CondVar, &s_syncOkToReceive_Mutex);
            }
            List_append(&buffer.messages_receive, message);
            pthread_cond_signal(&s_itemAvailtoReceive_CondVar);
        }
        pthread_mutex_unlock(&s_syncOkToReceive_Mutex);
        // End of Critical Section

        if (strcmp("!\n", message) == 0)
            CONTINUE = false;
    }
}

void *print_Output_Thread(void *vargp)
{
    char *pMessage = NULL;
    char message[BUFLEN] = "";
    while (true)
    {

        // Critical Section
        pthread_mutex_lock(&s_syncOkToReceive_Mutex);
        {
            while (buffer.messages_receive.count == 0)
            {
                pthread_cond_wait(&s_itemAvailtoReceive_CondVar, &s_syncOkToReceive_Mutex);
            }
            List_first(&buffer.messages_receive);
            pMessage = List_remove(&buffer.messages_receive);
            for (int i = 0; i < BUFLEN; i++)
            {
                message[i] = pMessage[i];
            }
            free(pMessage);
            pMessage = NULL;

            pthread_cond_signal(&s_buffAvailtoReceive_CondVar);
        }
        pthread_mutex_unlock(&s_syncOkToReceive_Mutex);
        // End of Critical Section

        printf("%s", message);
    }
}

void threads_ShutDown()
{

    pthread_cancel(tid1);
    pthread_cancel(tid2);
    pthread_cancel(tid3);
    pthread_cancel(tid4);
    pthread_cancel(tid5);

    List_free(&buffer.messages_send, freeItem);
    List_free(&buffer.messages_receive, freeItem);

    close(s);

    pthread_mutex_destroy(&s_syncOkToSend_Mutex);
    pthread_mutex_destroy(&s_syncOkToReceive_Mutex);

    pthread_cond_destroy(&s_buffAvailtoReceive_CondVar);
    pthread_cond_destroy(&s_buffAvailtoSend_CondVar);
    pthread_cond_destroy(&s_itemAvailtoReceive_CondVar);
    pthread_cond_destroy(&s_itemAvailtoSend_CondVar);
}

void *thread_Manager_Thread(void *vargp)
{

    while (CONTINUE == true);
    threads_ShutDown();
}

void threads_SetUP()
{
    pthread_create(&tid1, NULL, await_Input_Thread, (void *)&tid1);
    pthread_create(&tid2, NULL, send_Message_Thread, (void *)&tid2);
    pthread_create(&tid3, NULL, receive_Message_Thread, (void *)&tid3);
    pthread_create(&tid4, NULL, print_Output_Thread, (void *)&tid4);
    pthread_create(&tid5, NULL, thread_Manager_Thread, (void *)&tid5);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    pthread_join(tid4, NULL);
    pthread_join(tid5, NULL);
}

void SETUP_SOCKET_SERVER(int MYPORT, int OTHERPORT, char *OTHERCPU)
{
    List *messages_receive = List_create();
    List *messages_send = List_create();

    // create a UDP socket
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        die("socket");

    SETUP_OTHER_PORT(OTHERPORT, OTHERCPU);
    SETUP_MY_PORT(MYPORT);

    // bind socket to port
    if (bind(s, (struct sockaddr *)&si_me, sizeof(si_me)) == -1)
        die("bind");

    threads_SetUP();
}
