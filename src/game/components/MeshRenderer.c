#include "MeshRenderer.h"

#include "../../lib/Engine.h"
#include "../Logic.h"
#include "../utils/Bitmap3D.h"

void MeshRenderer__render(Entity_t* entity, Engine__State_t* state) {
  if (NULL == entity->render) return;

  Bitmap3D__RenderWall(
      state,
      entity->tform->pos.x,
      entity->tform->pos.y,
      entity->tform->pos.z,
      entity->render->tx,
      entity->render->ty,
      // entity->render->useMask,
      // entity->render->mask,
      entity->render->color);
}