#include <stdio.h>
#include <stdlib.h>
#include "list.h"

// Pool of Nodes
Node Pool_of_Nodes[LIST_MAX_NUM_NODES];
//  Pool of Lists
List Pool_of_List[LIST_MAX_NUM_HEADS];

// First time to set the values
bool first_Time = false;

// Keep track of Nodes from the Pool
int nextNode = 0;
int numUsedNodes = 0;
int poolHead_Nodes = 0;
int num_Init_Nodes = 0;

// Keep track of Lists from the Pool
int nextList = 0;
int numUsedHeads = 0;
int poolHead_List = 0;
int num_Init_List = 0;

// ------------------------------------------ Helper Functions

// Set the values of both pools for the first time
void initPools()
{
    if (first_Time == true)
        return;

    first_Time = false;

    for (int i = 0; i < LIST_MAX_NUM_NODES; i++)
    {
        Pool_of_Nodes[i].poolID = i;
        Pool_of_Nodes[i].poolNext = -1;
        Pool_of_Nodes[i].element = NULL;
    }

    for (int i = 0; i < LIST_MAX_NUM_HEADS; i++)
    {
        Pool_of_List[i].poolID = i;
        Pool_of_List[i].poolNext = -1;
        Pool_of_List[i].size = 0;
        Pool_of_List[i].inUse = false;
    }
}

// Allocate a node from the Pool and return its pointer
Node *Node_Allocate()
{
    Node *new_Node = NULL;

    // Nodes are allocated in order
    if (num_Init_Nodes == poolHead_Nodes && num_Init_Nodes < LIST_MAX_NUM_NODES)
    {
        new_Node = &Pool_of_Nodes[num_Init_Nodes];
        poolHead_Nodes++;
        num_Init_Nodes++;
        numUsedNodes++;
        return new_Node;
    }
    // Node is allocated in the last position that was deallocated after a deletion
    else if (numUsedNodes < LIST_MAX_NUM_NODES)
    {
        new_Node = &Pool_of_Nodes[poolHead_Nodes];
        poolHead_Nodes = new_Node->poolNext;
        new_Node->poolNext = -1;
        numUsedNodes++;
        return new_Node;
    }
    else
    {
        return new_Node;
    }
}

// Allocate a List from the Pool and return its pointer
List *List_Allocate()
{

    List *newList = NULL;

    // Lists are allocated in order
    if (num_Init_List == poolHead_List && num_Init_List < LIST_MAX_NUM_HEADS)
    {
        newList = &Pool_of_List[num_Init_List];
        newList->inUse = true;
        num_Init_List++;
        poolHead_List++;
        numUsedHeads++;
        return newList;
    }
    // List is allocated in the last position that was deallocated after a deletion
    if (numUsedHeads < LIST_MAX_NUM_HEADS)
    {
        newList = &Pool_of_List[poolHead_List];
        newList->inUse = true;
        poolHead_List = newList->poolNext;
        newList->poolNext = -1;
        numUsedHeads++;
        return newList;
    }
    else
    {
        return newList;
    }
}

// Deallocate a node to the Pool and return its pointer value
void *Node_DeAllocate(Node *pNode)
{
    // Set the pool Head to the last Node deleted
    pNode->poolNext = poolHead_Nodes;
    poolHead_Nodes = pNode->poolID;
    pNode->next = NULL;
    pNode->prev = NULL;
    // pNode->element = NULL;]
    printf("dealoc %s\n", (char *) pNode->element);
    numUsedNodes--;
    return pNode->element;
}

// Deallocate a List to the Pool and return its pointer value
void *List_DeAllocate(List *pList)
{
    // Set the pool Head to the last List deleted
    pList->poolNext = poolHead_List;
    poolHead_List = pList->poolID;
    pList->head = NULL;
    pList->tail = NULL;
    pList->current = NULL;
    pList->size = 0;
    pList->inUse = false;
    numUsedHeads--;
    return pList;
}

void decrease_Heads()
{
    numUsedHeads--;
}

List *getListFromPool(int id)
{
    return &Pool_of_List[id];
}

Node *getNodeFromPool(int id)
{
    return &Pool_of_Nodes[id];
}

bool isOBB_Start(List *pList)
{
    return ((pList->current == NULL) && (pList->LIST_OOB == LIST_OOB_START));
}

bool isOBB_End(List *pList)
{
    return ((pList->current == NULL) && (pList->LIST_OOB == LIST_OOB_END));
}

// ------------------------------------------

List *List_create()
{
    if (first_Time == true)
        initPools();
    first_Time = false;
    List *new_List = List_Allocate();
    return new_List;
}

int List_count(List *pList)
{
    return pList->size;
}

void *List_first(List *pList)
{
    void *x = pList->head;
    pList->current = pList->head;
    return x;
}

void *List_last(List *pList)
{
    void *x = pList->tail;
    pList->current = pList->tail;
    return x;
}

void *List_next(List *pList)
{
    if (pList->current == NULL)
    {
        if (pList->LIST_OOB == LIST_OOB_START)
            return List_first(pList);
        else
            return NULL;
    }
    if (pList->current->next == NULL)
        pList->LIST_OOB = LIST_OOB_END;

    pList->current = pList->current->next;
    return pList->current;
}

void *List_prev(List *pList)
{
    if (pList->current == NULL)
    {
        if (pList->LIST_OOB == LIST_OOB_END)
            return List_last(pList);
        else
            return NULL;
    }
    if (pList->current->prev == NULL)
        pList->LIST_OOB = LIST_OOB_START;

    pList->current = pList->current->prev;
    return pList->current;
}

void *List_curr(List *pList)
{
    return pList->current;
}

int List_add_on_empty(List *pList, Node *new_Node, void *pItem)
{
    if (new_Node != NULL)
    {
        new_Node->element = pItem;
        new_Node->prev = NULL;
        new_Node->next = NULL;

        pList->head = new_Node;
        pList->current = new_Node;
        pList->tail = new_Node;
        pList->size++;
        return LIST_SUCCESS;
    }

    return LIST_FAIL;
}

int List_add_on_head(List *pList, Node *new_Node, void *pItem)
{

    if (new_Node != NULL)
    {
        new_Node->element = pItem;
        new_Node->prev = NULL;

        // set current item to head node
        List_first(pList);

        pList->head->prev = new_Node;
        new_Node->next = pList->head;

        //---
        pList->head = new_Node;
        pList->current = new_Node;
        pList->size++;
        return LIST_SUCCESS;
    }
    return LIST_FAIL;
}

int List_add_on_tail(List *pList, Node *new_Node, void *pItem)
{

    if (new_Node != NULL)
    {
        new_Node->element = pItem;
        new_Node->next = NULL;

        // set current item to tail node
        pList->tail->next = new_Node;
        new_Node->prev = pList->tail;
        //---
        pList->tail = new_Node;
        pList->current = new_Node;
        pList->size++;
        return LIST_SUCCESS;
    }
    return LIST_FAIL;
}

int List_insert_after(List *pList, void *pItem)
{

    if (pList == NULL)
        return LIST_FAIL;

    Node *new_Node = Node_Allocate();

    if (new_Node == NULL)
        return LIST_FAIL;

    else if (pList->size == 0)
        return List_add_on_empty(pList, new_Node, pItem);

    // add to tail if current item is in tail or beyond
    else if ((pList->current == pList->tail) || (isOBB_End(pList)))
        return List_add_on_tail(pList, new_Node, pItem);

    // add to head if current item is before start
    else if (pList->current->next == pList->head->prev)
        return List_add_on_head(pList, new_Node, pItem);

    else
    {
        new_Node->element = pItem;

        // save the reference of current.next in an aux node
        Node *aux_Ref = pList->current->next;

        // change left pointers
        pList->current->next = new_Node;
        new_Node->prev = pList->current;

        // change right pointers
        new_Node->next = aux_Ref;
        aux_Ref->prev = new_Node;

        // update new current node
        pList->current = new_Node;

        // free node replaced by aux (old current.next)
        pList->size++;
        return LIST_SUCCESS;
    }
}

int List_insert_before(List *pList, void *pItem)
{

    if (pList == NULL)
        return LIST_FAIL;

    Node *new_Node = Node_Allocate();

    if (new_Node == NULL)
        return LIST_FAIL;

    if (pList->size == 0)
        return List_add_on_empty(pList, new_Node, pItem);

    // add to head if current item is before start
    else if (pList->current == pList->head || isOBB_Start(pList))
        return List_add_on_head(pList, new_Node, pItem);

    // add to tail if current item is after end
    else if (pList->current->prev == pList->tail->next)
        return List_add_on_tail(pList, new_Node, pItem);

    else
    {
        // assign node --> Find a way to allocate
        new_Node->element = pItem;

        // save the reference current.prev
        Node *aux_Node = pList->current->prev;

        // change right pointers
        pList->current->prev = new_Node;
        new_Node->next = pList->current;

        // change left pointers
        new_Node->prev = aux_Node;
        aux_Node->next = new_Node;

        // update new current node
        pList->current = new_Node;

        // free node replaced by aux (old current.prev)
        pList->size++;
        return LIST_SUCCESS;
    }
}

int List_append(List *pList, void *pItem)
{
    List_last(pList);
    return List_insert_after(pList, pItem);
}

int List_prepend(List *pList, void *pItem)
{
    List_first(pList);
    return List_insert_before(pList, pItem);
}

void *List_remove(List *pList)
{
    if (pList == NULL)
        return NULL;
    if (pList->current == NULL)
        return NULL;
    if (pList->size == 1)
    {
        Node *ptr = Node_DeAllocate(pList->head);
        pList->head = NULL;
        pList->tail = NULL;
        pList->current = NULL;
        pList->size--;
        return ptr;
    }

    Node *deletedNode = pList->current;

    // Erase node from head;
    if (pList->current == pList->head)
    {
        pList->head = pList->current->next;
        pList->head->prev = NULL;
        List_next(pList);
        pList->size--;
        return Node_DeAllocate(deletedNode);
    }

    // Erase node from tail
    else if (pList->current == pList->tail)
    {
        pList->tail = pList->current->prev;
        pList->tail->next = NULL;
        List_next(pList);
        pList->size--;
        return Node_DeAllocate(deletedNode);
    }
    else
    {
        pList->current->next->prev = pList->current->prev;
        pList->current->prev->next = pList->current->next;
        List_next(pList);
        pList->size--;
        return Node_DeAllocate(deletedNode);
    }
}

void *List_trim(List *pList)
{
    List_last(pList);
    return List_remove(pList);
}

void *List_pop(List *pList)
{
    List_first(pList);
    return List_remove(pList);
}

void List_concat(List *pList1, List *pList2)
{
    if (pList1 == NULL || pList2 == NULL)
        return;

    // in case pList1 is empty
    if (pList1->head == NULL)
    {
        pList1->head = pList2->head;
        pList1->tail = pList2->tail;
        pList1->current = pList2->current;
        pList1->size = pList2->size;
    }

    if (pList2->head != NULL)
    {
        pList1->tail->next = pList2->head;
        pList2->head->prev = pList1->tail;
        pList1->tail = pList2->tail;
        pList1->size += pList2->size;
        decrease_Heads();
    }
    List_DeAllocate(pList2);
}

void List_free(List *pList, FREE_FN pItemFreeFn)
{

    List_first(pList);

    while (pList->current != NULL)
    {
        pItemFreeFn(pList->current->element);
        List_remove(pList);
    }

    List_DeAllocate(pList);
}

void *List_search(List *pList, COMPARATOR_FN pComparator, void *pComparisonArg)
{
    if (pList == NULL)
        return NULL;

    if (pList->current == NULL)
        pList->current = pList->head;
    while (pList->current != NULL)
    {
        if (pComparator(pList->current->element, pComparisonArg))
            return pList->current;
        List_next(pList);
    }
    return NULL;
}

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
        printf("%s ", (char *)pList->current->element);
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