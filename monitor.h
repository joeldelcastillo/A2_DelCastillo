#ifndef _MONITOR_H_
#define _MONITOR_H_
#include <stdbool.h>
#include "list.h"

typedef struct Monitor_s Monitor;
struct Monitor_s
{
    List messages_send;
    List messages_receive;
};

#endif