#include <stdio.h>
#include <stdlib.h>
#include "monitor.h"
#include "list.h"

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