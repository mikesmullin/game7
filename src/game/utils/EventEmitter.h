#pragma once

typedef struct EventEmitter_t EventEmitter;
typedef enum EventType_t EventType;
typedef enum ListenerFnId_t ListenerFnId;

void EventEmitter__init(EventEmitter* emitter);
void EventEmitter__on(EventEmitter* emitter, EventType event, ListenerFnId listener);
void EventEmitter__emit(EventEmitter* emitter, EventType event, void* self, void* data);
