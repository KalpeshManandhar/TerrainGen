#include "terrain.h"



void addChunk(TerrainGenerator *gen, Vec3f chunkPos, ChunkType type){
    TerrainChunk chunk;
    chunk.vertices = (Vec3f *)malloc(sizeof(*chunk.vertices) * chunk.sizex *chunk.sizez);

    for (int i=0; i< chunk.sizex * chunk.sizez; i++){
        Vec3f vertex;
        vertex.x = i%chunk.sizex;
        vertex.z = i/chunk.sizex;

        // sample y from noise map
        int samplex = (int)(vertex.x * 2 + chunkPos.x)%gen->noiseMapw;
        // int samplez = ((int)(vertex.z + chunkPos.z)/gen->noiseMapw)%gen->noiseMaph;
        int samplez = ((int)(vertex.z * 2 + chunkPos.z)%gen->noiseMaph);
        vertex.y = 1.0f - gen->noiseMap[(samplex + samplez * gen->noiseMapw)];
        vertex.y = vertex.y * 2.0f - 1.0f;

        float yAmplitude = 1.0f;
        switch (type)
        {
        case ChunkType::CHUNK_MOUNTAIN:     yAmplitude = 30.0f; break;
        case ChunkType::CHUNK_HILL:         yAmplitude = 15.0f; break;
        case ChunkType::CHUNK_PLAIN:        yAmplitude = 1.0f; break;
        case ChunkType::CHUNK_VALLEY:       yAmplitude = -5.0f; break;
        default:
            break;
        }

        // vertex.x *= 0.1f;
        // vertex.z *= 0.1f;
        vertex.y *= yAmplitude;

        chunk.vertices[i] = vertex;
    }

    chunk.chunkOrigin = chunkPos;
    gen->chunks.push_back(chunk);
}

uint32_t *getIndices(uint32_t chunksizeX, uint32_t chunksizeZ){
    // (width - 1) * (length - 1) * 6 points
    size_t nVertices = chunksizeX * chunksizeZ; 
    size_t nIndices = (chunksizeX-1) * (chunksizeZ-1) * 6;
    uint32_t *indices = (uint32_t *)malloc(nIndices* sizeof(*indices));
    int j = 0;
    for (int i =0; i<nVertices; i++){
        int x = i%chunksizeX;
        int y = i/chunksizeX;

        // dont compute for points with (x,y) with x and y not both even or both odd
        // avoids overlap of triangles
        if (((y&0x1) && !(x&0x1)) || (!(y&0x1) && (x&0x1)))
            continue;
        

        int up = i - chunksizeX;
        int down = i + chunksizeX;
        int left = i - 1;
        int right = i + 1;

        int upy = (up)/chunksizeX;        
        int downy = (down)/chunksizeX;
        int leftx = (left)%chunksizeX;
        int rightx = (right)%chunksizeX;

        bool upExists = (upy >= 0)&& (up>=0);                       // if up of current point is not less than 0
        bool downExists = downy < chunksizeZ;         // if down of current point is less than the width (sizez)
        bool leftExists = (leftx < x)&&(left >= 0);     // if leftx > x then it is of y-1
        bool rightExists = rightx > x;                  // if right < x then it is of y+1

        if (upExists && leftExists){                              
            indices[j++] = up;
            indices[j++] = left;
            indices[j++] = i;
        }
        if (upExists && rightExists){      
            indices[j++] = up;
            indices[j++] = right;
            indices[j++] = i;
        }
        if (downExists && rightExists){                 
            indices[j++] = down;
            indices[j++] = right;
            indices[j++] = i;
        }
        if (downExists && leftExists){                
            indices[j++] = down;
            indices[j++] = left;
            indices[j++] = i;
        }
    
    }
    return(indices);
}

