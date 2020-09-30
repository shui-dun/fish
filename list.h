#pragma once

typedef struct ListType {
    int nu;
    int x;
    int y;
} ListType;

typedef struct LNode {
    ListType data;
    struct LNode *next;
} LNode, *LinkedList;

LinkedList list_init();

int list_insert(LinkedList list, int pos, ListType data);

void list_delete_next(LinkedList list, LNode *node);

void list_destroy(LNode **list);