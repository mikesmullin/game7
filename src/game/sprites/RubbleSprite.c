#include "RubbleSprite.h"

#include "../../lib/Math.h"
#include "../Logic.h"
#include "../components/SpriteRenderer.h"
#include "../utils/Color.h"

void RubbleSprite__init(RubbleSprite* sprite, Engine__State_t* state) {
  sprite->base.pos.x = Math__random(0, 1) - 0.5;
  sprite->base.pos.y = Math__random(0, 1) * 0.8;
  sprite->base.pos.z = Math__random(0, 1) - 0.5;
  sprite->base.tx = 7;
  sprite->base.ty = 0;
  sprite->base.useMask = true;
  sprite->base.mask = BLACK;
  sprite->base.color = TRANSPARENT;
  sprite->xa = Math__random(0, 1) - 0.5;
  sprite->ya = Math__random(0, 1);
  sprite->za = Math__random(0, 1) - 0.5;
}

void RubbleSprite__tick(RubbleSprite* sprite, Engine__State_t* state) {
  if (sprite->removed) return;

  sprite->base.pos.x += sprite->xa * 0.2f;
  sprite->base.pos.y += sprite->ya * 0.2f;
  sprite->base.pos.z += sprite->za * 0.2f;
  sprite->ya -= 0.1f;
  if (sprite->base.pos.y < 0) {
    sprite->base.pos.y = 0;
    sprite->xa *= 0.8f;
    sprite->za *= 0.8f;
    if (Math__random(0, 1) < 0.04f) {
      sprite->removed = true;
    }
  }
}

void RubbleSprite__render(RubbleSprite* sprite, Engine__State_t* state) {
  if (sprite->removed) return;

  SpriteRenderer__render((Sprite_t*)sprite, state);
}