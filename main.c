#include <stdio.h>
#include <stdlib.h>
#include "monitor.h"

int main()
{
    printf("\n\n");
    printf("============================================= \n");
    printf("CMPT300 - Assignment 2 \n");
    printf("@author: Joel del Castillo \n");
    printf("============================================= \n");
    printf("\n");

    SETUP_SOCKET_SERVER();
    int otherPort;
    printf("Type a port: ");
    scanf("%d", &otherPort);
    sendMessage(otherPort);
    


    return 0;
}