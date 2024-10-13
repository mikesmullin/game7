#include "Wavefront.h"

#include <stdio.h>
#include <string.h>

#include "../../lib/Arena.h"
#include "../../lib/List.h"
#include "../../lib/Log.h"
#include "GLMShim.h"

// Wavefront Format Specification: http://www.martinreddy.net/gfx/3d/OBJ.spec

Wavefront_t* Wavefront__parse_obj(Arena_t* arena, const char* filename) {
  FILE* file;
  fopen_s(&file, filename, "r");
  ASSERT_CONTEXT(file, "Fatal: cannot read OBJ file: %s", filename);

  Wavefront_t* obj = Arena__Push(arena, sizeof(Wavefront_t));
  obj->vertices = List__alloc(arena);
  obj->texcoords = List__alloc(arena);
  obj->normals = List__alloc(arena);
  obj->faces = List__alloc(arena);

  char line[256];
  while (fgets(line, sizeof(line), file)) {
    if (strncmp(line, "v ", 2) == 0) {
      v3* v = Arena__Push(arena, sizeof(v3));
      sscanf_s(line, "v %f %f %f", &v->x, &v->y, &v->z);
      List__append(arena, obj->vertices, v);
    } else if (strncmp(line, "vt ", 3) == 0) {
      v2* vt = Arena__Push(arena, sizeof(v2));
      sscanf_s(line, "vt %f %f", &vt->x, &vt->y);
      List__append(arena, obj->texcoords, vt);
    } else if (strncmp(line, "vn ", 3) == 0) {
      v3* vn = Arena__Push(arena, sizeof(v3));
      sscanf_s(line, "vn %f %f %f", &vn->x, &vn->y, &vn->z);
      List__append(arena, obj->normals, vn);
    } else if (strncmp(line, "f ", 2) == 0) {
      Wavefront__Face_t* f = Arena__Push(arena, sizeof(Wavefront__Face_t));
      u32 matches = sscanf_s(
          line,
          "f %d/%d/%d %d/%d/%d %d/%d/%d",
          &f->vertex_idx[0],
          &f->texcoord_idx[0],
          &f->normal_idx[0],
          &f->vertex_idx[1],
          &f->texcoord_idx[1],
          &f->normal_idx[1],
          &f->vertex_idx[2],
          &f->texcoord_idx[2],
          &f->normal_idx[2]);
      List__append(arena, obj->faces, f);
    }
  }

  fclose(file);
  return obj;
}