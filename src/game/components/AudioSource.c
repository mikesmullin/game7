#include "AudioSource.h"

#include <math.h>

#include "../../lib/Engine.h"
#include "../../lib/Log.h"
#include "../../lib/Math.h"
#include "../Logic.h"

extern Engine__State_t* g_engine;

void AudioSource__play(Entity_t* entity, AUDIO_FILES sound) {
  f32 MAX_HEAR_DIST = 15.0f;
  Player_t* player = (Player_t*)g_engine->local->game->curPlyr;
  // only play if listener is near
  if (NULL != player->base.hear) {
    // calc pan value
    vec3 p1_minus_p0, forward, right, up = {0, 1, 0};
    f32 pan_value;

    // Get the direction vector from player to source
    glms_vec3_sub((vec3*)&player->base.tform->pos, (vec3*)&entity->tform->pos, (vec3*)&p1_minus_p0);

    // Calculate forward vector (assuming rot0_y is the yaw in radians)
    f32 rY = player->base.tform->rot.y;
    forward[0] = cosf(rY);
    forward[1] = 0;
    forward[2] = -sinf(rY);
    glms_vec3_normalize(forward);

    // Calculate the right vector (player's right side)
    glms_vec3_cross(forward, up, right);
    glms_vec3_normalize(right);

    // Project vector onto the right direction
    pan_value = glms_v3_dot(
        (v3){right[0], right[1], right[2]},
        (v3){p1_minus_p0[0], p1_minus_p0[1], p1_minus_p0[2]});

    // Normalize pan value within [-1, 1]
    pan_value = MATH_CLAMP(pan_value / MAX_HEAR_DIST, -1.0f, 1.0f);

    // calc distance/dropoff between source and listener, set volume/pan

    f32 d1 = glms_v3_distance((vec3*)&player->base.tform->pos, (vec3*)&entity->tform->pos);
    f32 d2 = Math__map(
        MATH_CLAMP(2, fabs(d1), MAX_HEAR_DIST),  //
        2,
        MAX_HEAR_DIST,  //
        1,
        0);

    LOG_DEBUGF("d1 %f, d2 %f, pan %f", d1, d2, pan_value);
    g_engine->Audio__ResumeAudio(g_engine->audio, sound, false, d2, pan_value);
  }
}