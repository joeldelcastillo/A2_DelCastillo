// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <pthread.h>
// #include "controller.h"
// #include <string.h>
// #include "list.h"
// #include <semaphore.h>

// int main(int argc, char const *argv[])

// {
//     char buf[20];
//     fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
//     sleep(10);
//     int numRead = read(0, buf, 4);
//     if (numRead > 0) {
//         printf("You said: %s", buf);
//     }
// }
