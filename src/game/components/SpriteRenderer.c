#include "SpriteRenderer.h"

#include "../../lib/Engine.h"
#include "../Logic.h"
#include "../utils/Bitmap3D.h"

void SpriteRenderer__render(Sprite_t* sprite, Engine__State_t* state) {
  Bitmap3D__RenderSprite(
      state,
      sprite->pos.x,
      sprite->pos.y,
      sprite->pos.z,
      sprite->tx,
      sprite->ty,
      sprite->useMask,
      sprite->mask,
      sprite->color);
}

void SpriteRenderer__render2(Entity_t* entity, Engine__State_t* state) {
  if (NULL == entity->render) return;

  Bitmap3D__RenderSprite(
      state,
      entity->tform->pos.x,
      entity->tform->pos.y,
      entity->tform->pos.z,
      entity->render->tx,
      entity->render->ty,
      entity->render->useMask,
      entity->render->mask,
      entity->render->color);
}