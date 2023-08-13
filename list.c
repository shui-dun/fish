#include "list.h"
#include <stdio.h>
#include <stdlib.h>

LinkedList list_init() {
    LinkedList list = (LinkedList)malloc(sizeof(LNode));
    list->next = NULL;
    return list;
}

int list_insert(LinkedList list, int pos, ListType data) {
    for (int i = 0; i < pos; i++) {
        list = list->next;
        if (!list) return 0;
    }
    LNode* node = (LNode*)malloc(sizeof(LNode));
    node->data = data;
    node->next = list->next;
    list->next = node;
    return 1;
}

void list_delete_next(LinkedList list, LNode *node) {
    LNode *rem=node->next;
    node->next=rem->next;
    free(rem);
}

void list_destroy(LNode** list) {
    LNode *cur = *list, *temp = *list;
    while (cur) {
        temp = temp->next;
        free(cur);
        cur = temp;
    }
    *list = NULL;
}