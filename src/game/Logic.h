#ifndef LOGIC_H
#define LOGIC_H

typedef struct {
  int example;
} State;

typedef void (*logic_init_t)(State*);
typedef void (*logic_update_t)(State*);
typedef void (*logic_draw_t)(const State*);

#endif  // LOGIC_H
