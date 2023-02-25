
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "controller.h"
#include <string.h>
#include "list.h"
#include <semaphore.h>

#define SERVER "127.0.0.1"
#define BUFLEN 512 // Max length of buffer
int MY_PORT;       // The port on which to send data
int OTHER_PORT;
char OTHER_CPU[20];

struct sockaddr_in si_me;
struct sockaddr_in si_other;
int s, i, slen = sizeof(si_other), recv_len;

bool STOP = false;

sem_t empty, full;
pthread_mutex_t mutex;

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

void init_semaphores()
{
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, 1);
    sem_init(&full, 0, 0);
}

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
        // Return the first one;
        strcpy(ip, inet_ntoa(*addr_list[i]));
        return 0;
    }

    return 1;
}

void SETUP_OTHER_PORT(int port, char *cpu)
{
    OTHER_PORT = port;
    printf("CPU: %s \n", cpu);
    char host[] = "asb9700u-g01.csil.sfu.ca";
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

void SETUP_SOCKET_SERVER(int MYPORT, int OTHERPORT, char *OTHERCPU)
{
    init_semaphores();

    pthread_t tid1;
    pthread_t tid2;
    pthread_t tid3;
    pthread_t tid4;
    // create a UDP socket
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        die("socket");

    SETUP_OTHER_PORT(OTHERPORT, OTHERCPU);
    SETUP_MY_PORT(MYPORT);

    // bind socket to port
    if (bind(s, (struct sockaddr *)&si_me, sizeof(si_me)) == -1)
    {
        die("bind");
    }

    printf("MY_PORT: %d \n", MY_PORT);

    pthread_create(&tid1, NULL, await_Input, (void *)&tid1);
    pthread_create(&tid2, NULL, send_Message, (void *)&tid2);
    pthread_create(&tid3, NULL, receive_Message, (void *)&tid3);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
}

void *await_Input(void *vargp)
{

    while (STOP == false)
    {
        char *message = malloc(sizeof(char[256]));
        // printf("pointer: %p ",message);

        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        {
            fgets(message, 256, stdin);
            // fflush(stdout);

            if (strcmp("!\n", message) == 0)
                STOP = true;
            List_append(&buffer.messages_send, message);
            // print_List(&buffer.messages_send);
        }

        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}

void *send_Message(void *vargp)
{
    // printf("%d \n", buffer.messages_send.size);
    // print_List(&buffer.messages_send);
    // void *message = List_pop(&buffer.messages_send);
    // printf("%s \n", (char *) message);
    // print_List(&buffer.messages_send);
    // printf("%d \n", buffer.messages_send.size);

    while (STOP == false)
    {
        // printf("BUF size: %d \n", buffer.messages_send.size);
        // printf("%d \n", buffer.messages_send.size);
        char *message;

        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        {
            message = List_pop(&buffer.messages_send);
        }

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);

        if (sendto(s, message, strlen(message), 0, (struct sockaddr *)&si_other, slen) == -1)
        {
            printf("died");
            die("sendto()");
        }

        // receive a reply and print it
        // clear the buffer by filling null, it might have previously received data
        memset(message, '\0', BUFLEN);
        free(message);
    }

    // close(s);
}

void *receive_Message(void *vargp)
{
    while (true)
    {
        char *message = malloc(sizeof(char[256]));

        // receive a reply and print it
        // clear the buffer by filling null, it might have previously received data
        memset(message, '\0', BUFLEN);

        // try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, message, BUFLEN, 0, (struct sockaddr *)&si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }

        // puts(message);
        // List_append(&buffer.messages_receive, message);

        // print details of the client/peer and the data received
        // printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("------------- %s\n", message);

        free(message);
    }
    close(s);
}

void *print_Output(void *vargp)
{
    while (buffer.messages_receive.size = !0)
    {
        char *message = List_pop(&buffer.messages_receive);
        printf("%s \n", message);
    }
}

// void *send(void *vargp)
// {

//     // keep listening for data
//     while (1)
//     {
//         char message[1024];
//         printf("Enter message : ");
//         scanf("%1023[\n]", message);
//         fflush(stdout);

//         // send the message
//         if (sendto(s, message, strlen(message), 0, (struct sockaddr *)&si_other, slen) == -1)
//         {
//             die("sendto()");
//         }
//         // receive a reply and print it
//         // clear the buffer by filling null, it might have previously received data
//         memset(buf, '\0', BUFLEN);

//         // try to receive some data, this is a blocking call
//         if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&si_other, &slen)) == -1)
//         {
//             die("recvfrom()");
//         }

//         puts(buf);

//         // print details of the client/peer and the data received
//         printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
//         printf("Data: %s\n", buf);
//     }
//     close(s);
// }

// Message receive_Message(char message[]){

// }

// decode message received
// Message decode_Message(char[] message);

// Send Message
// int send_Message(Message message);

// bool keyboard[256] = {0};
