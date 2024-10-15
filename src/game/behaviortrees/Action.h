#pragma once

typedef struct Entity_t Entity_t;
typedef struct Action_t Action;

void Action__PerformBuffered(Entity_t* entity, Action* action);