#include <stdio.h>
#include <stdlib.h>
#include "controller.h"

int main(int argc, char *argv[])
{

    printf("\n\n");
    printf("============================================= \n");
    printf("CMPT300 - Assignment 2 \n");
    printf("@author: Joel del Castillo \n");
    printf("============================================= \n");
    printf("\n");

    if (argc == 4){
        int MY_PORT = atoi(argv[1]);
        int OTHER_PORT = atoi(argv[3]);
        printf("MY PORT: %d, OTHER_PORT: %d, OTHER_MACHINE: %s \n", MY_PORT, OTHER_PORT, argv[2]);
        SETUP_SOCKET_SERVER(MY_PORT, OTHER_PORT, argv[2]);
    }
    else
        printf("3 arguments were expected, %d were given\n", argc -1);

    return 0;
}
