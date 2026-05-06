#pragma once
#ifndef ARS_TIMORIS__VISUAL__PARTICLE__PARTICLE_DATA_H
#define ARS_TIMORIS__VISUAL__PARTICLE__PARTICLE_DATA_H
#include <SDL3/SDL.h>

namespace ArsTimoris::Visual::Particle {
    struct ParticleData {
    public:
        SDL_FPoint position;
        SDL_FPoint linearVelocity;
        float angle;
        float angularVelocity;
        float scale;
        float timeLeft;
    };
}
#endif