#pragma once

#include <vector>
#define Array std::vector
#include "./math/vec.h"


#define CHUNK_SIZE 256
#define VISIBLE_CHUNK_RADIUS 1


enum ChunkType{
    CHUNK_MOUNTAIN,
    CHUNK_WATER,
    CHUNK_HILL,
    CHUNK_VALLEY,
    CHUNK_PLAIN
};

struct TerrainChunk{
    int sizex = CHUNK_SIZE;
    int sizez = CHUNK_SIZE;

    
    Vec3f chunkOrigin;
    Vec3f *vertices;

    
};


struct TerrainGenerator{
    float *noiseMap;
    int noiseMapw;
    int noiseMaph;

    Array<TerrainChunk> chunks;
    bool chunkGrid[256][256]={false};
};



void addChunk(TerrainGenerator *gen, Vec3f chunkPos, ChunkType type);
uint32_t *getIndices(uint32_t chunksizeX, uint32_t chunksizeZ);


