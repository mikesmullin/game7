#ifndef LIST_H
#define LIST_H

#include <stdint.h>
typedef uint32_t u32;

typedef struct Arena_t Arena_t;

typedef struct List__Node_t {
  struct List__Node_t* next;
  void* data;
} List__Node_t;

typedef struct List_t {
  u32 len;
  List__Node_t* head;
  List__Node_t* tail;
} List_t;

List_t* List__alloc(Arena_t* arena);
List__Node_t* List__Node__alloc(Arena_t* arena);
void List__init(List_t* list);
void List__Node__init(List__Node_t* node, void* data);
void List__append(Arena_t* arena, List_t* list, void* data);
void* List__getOne(List_t* list, u32 index);

#endif  // LIST_H