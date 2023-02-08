// #include <stdio.h>
// #include <stdlib.h>
// #include "monitor.h"

// int main()
// {
//     printf("\n\n");
//     printf("============================================= \n");
//     printf("CMPT300 - Assignment 2 \n");
//     printf("@author: Joel del Castillo \n");
//     printf("============================================= \n");
//     printf("\n");

//     SETUP_SOCKET_SERVER();
//     int otherPort;
//     printf("Type a port: ");
//     scanf("%d", &otherPort);
//     sendMessage(otherPort);
    


//     return 0;
// }

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
  
// Let us create a global variable to change it in threads
int g = 0;
  
// The function to be executed by all threads
void *myThreadFun(void *vargp)
{
    // Store the value argument passed to this thread
    int *myid = (int *)vargp;
  
    // Let us create a static variable to observe its changes
    int s = 0;
  
    // Change static and global variables
    ++s; ++g;
  
    // Print the argument, static and global variables
    printf("Thread ID: %d, Static: %d, Global: %d\n", *myid, ++s, ++g);
}
  
int main()
{
    int i;
    pthread_t tid;
  
    // Let us create three threads
    for (i = 0; i < 3; i++)
        pthread_create(&tid, NULL, myThreadFun, (void *)&tid);
  
    pthread_exit(NULL);
    return 0;
}