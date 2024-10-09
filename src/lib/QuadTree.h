#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "Arena.h"
#include "Geometry.h"

typedef uint32_t u32;
typedef float f32;

#define QUAD_TREE_MAX_CAPACITY 4

typedef struct QuadTreeNode_t {
  Rect boundary;  // The boundary (2D space) of this node
  Point points[QUAD_TREE_MAX_CAPACITY];  // Points contained in this node
  void* data[QUAD_TREE_MAX_CAPACITY];  // data the points represent
  u32 point_count;  // Number of points in this node
  bool subdivided;  // Has this node been subdivided?
  struct QuadTreeNode_t* northwest;  // Pointers to children
  struct QuadTreeNode_t* northeast;
  struct QuadTreeNode_t* southwest;
  struct QuadTreeNode_t* southeast;
} QuadTreeNode;

QuadTreeNode* QuadTreeNode_create(Arena_t* arena, Rect boundary);
void QuadTreeNode_subdivide(Arena_t* arena, QuadTreeNode* node);
bool QuadTreeNode_insert(Arena_t* arena, QuadTreeNode* node, Point point, void* data);
void QuadTreeNode_query(
    QuadTreeNode* node, Rect range, u32 limit, void* matchData[], u32* matchCount);