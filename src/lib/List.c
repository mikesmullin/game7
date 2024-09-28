#include "List.h"

#include "Arena.h"

List_t* List__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(List_t));
}

List__Node_t* List__Node__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(List__Node_t));
}

void List__init(List_t* list) {
  list->len = 0;
  list->head = NULL;
  list->tail = NULL;
}

void List__Node__init(List__Node_t* node, void* data) {
  node->data = data;
  node->next = NULL;
}

void List__append(Arena_t* arena, List_t* list, void* data) {
  List__Node_t* node = List__Node__alloc(arena);
  List__Node__init(node, data);

  if (NULL == list->tail) {
    list->head = node;
    list->tail = node;
  } else {
    list->tail->next = node;
    list->tail = node;
  }
  list->len++;
}

void* List__getOne(List_t* list, u32 index) {
  List__Node_t* c = list->head;
  u32 i = 0;
  while (NULL != c) {
    if (i == index) {
      return c->data;
    }
    i++;
    c = c->next;
  }
  return NULL;
}