#pragma once

#include <stdint.h>

typedef uint32_t u32;
typedef float f32;

typedef struct Arena_t Arena_t;
typedef struct List_t List_t;

typedef struct Wavefront__Face_t {
  // assumes Triangles, not Quads
  u32 vertex_idx[3];
  u32 texcoord_idx[3];
  u32 normal_idx[3];
} Wavefront__Face_t;

typedef struct Wavefront_t {
  List_t* vertices;
  List_t* texcoords;
  List_t* normals;
  List_t* faces;
} Wavefront_t;

Wavefront_t* Wavefront__parse_obj(Arena_t* arena, const char* filename);
void Wavefront__print_obj(Wavefront_t* obj);
