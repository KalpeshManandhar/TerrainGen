#pragma once

#include <stdint.h>
#include "math/vec.h"

#define PTABLE_SIZE (4096 * 2)



uint32_t *getPermutationTable(uint32_t n);
float perlinNoise1D(float p, uint32_t *ptable, uint32_t n);
float perlinNoise2D(Vec2f p, uint32_t *permutationTable, uint32_t n);
float perlinNoise3D(Vec3f p, uint32_t *permutationTable, uint32_t n);
float fractionalBrownianMotion2D(Vec2f p, uint32_t *permutationTable, uint32_t ptableSize, uint32_t layers);



