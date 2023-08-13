#pragma once

#include "./math/vec.h"
#include "./Allocator/freeListAllocator.h"
#include "./Graphics/renderer.h"

#include <vector>
#define Array std::vector

#define CHUNK_SIZE 256
#define VISIBLE_CHUNK_RADIUS 1



enum ChunkType{
    CHUNK_MOUNTAIN,
    CHUNK_HILL,
    CHUNK_VALLEY,
    CHUNK_PLAIN,

    CHUNK_TYPE_COUNT
};

typedef struct TerrainChunk{
    Vec3f *vertices;
    Vec3f chunkOrigin;
};

struct TerrainGenerator{
    float *noiseMap;
    int noiseMapw;
    int noiseMaph;

    int sizex;
    int sizez;
    float scaleX;
    float scaleZ;

    
    FreeListAllocator allocator;
    TerrainChunk *chunkGrid;
    bool isChunkGenerated[256][256] = {false};

    Array<Object3D> chunkObjects;
    uint32_t *indices;

    TerrainGenerator(int dim, float scaleX, float scaleZ);
    ~TerrainGenerator();
    
};




TerrainChunk addChunk(TerrainGenerator *gen, Vec3f chunkY, float amplMultiplier);
uint32_t *getIndices(uint32_t chunksizeX, uint32_t chunksizeZ);
void stitchTerrain(TerrainGenerator *gen, TerrainChunk *a, TerrainChunk *b, int ndepth, float p, float influenceFactorA);
void proceduralGenerate(TerrainGenerator *gen, Vec3f cameraPos, Vec3f cameraFront);
void generateNoiseMap(TerrainGenerator *gen, int w, int h);


#undef Array

