#include <stdio.h>
#include <stdlib.h>
#include "controller.h"
#include<sys/socket.h>

int MY_PORT;
int OTHER_PORT;

int main(int argc, char *argv[])
{
    printf("\n\n");
    printf("============================================= \n");
    printf("CMPT300 - Assignment 2 \n");
    printf("@author: Joel del Castillo \n");
    printf("============================================= \n");
    printf("\n");


    if( argc == 4 ) {
        MY_PORT = atoi(argv[1]);
        OTHER_PORT = atoi(argv[3]);
        printf("The argument supplied is %d  %s  %d\n", MY_PORT, argv[2], OTHER_PORT);
    }
    else if( argc > 4 ) {
        printf("Too many arguments supplied.\n");
    }
    else {
        printf("Too few argument expected.\n");
    }


   

    

    SETUP_SOCKET_SERVER();

    return 0;
}