#pragma once

#include <vector>
#define Array std::vector
#include "./math/vec.h"
#include "./Allocator/freeListAllocator.h"

#define CHUNK_SIZE 256
#define VISIBLE_CHUNK_RADIUS 1



enum ChunkType{
    CHUNK_MOUNTAIN,
    CHUNK_WATER,
    CHUNK_HILL,
    CHUNK_VALLEY,
    CHUNK_PLAIN
};

typedef struct TerrainChunk{
    int sizex = CHUNK_SIZE;
    int sizez = CHUNK_SIZE;

    
    Vec3f chunkOrigin;
    Vec3f *vertices;
    // float *yValues;
}*TerrainChunkPtr;


struct TerrainGenerator{
    float *noiseMap;
    int noiseMapw;
    int noiseMaph;

    FreeListAllocator allocator;
    TerrainChunkPtr chunkGrid[256][256]={NULL};

    TerrainGenerator();
    ~TerrainGenerator();
};



void addChunk(TerrainGenerator *gen, Vec3f chunkPos, ChunkType type);
uint32_t *getIndices(uint32_t chunksizeX, uint32_t chunksizeZ);
void stitchTerrain(TerrainChunk *a, TerrainChunk *b, int ndepth, float p, float influenceFactorA);
void proceduralGenerate(TerrainGenerator *gen, Vec2f cameraPos, Vec3f cameraFront);

