#include "terrain.h"
#include "Allocator/allocator.h"

#define INDEX_ARRAY(arr,x,z,sizez) (arr[x + z * sizez])


void addChunk(TerrainGenerator *gen, Vec3f chunkPos, ChunkType type){
    TerrainChunk chunk;
    chunk.vertices = (Vec3f *)malloc(sizeof(*chunk.vertices) * chunk.sizex *chunk.sizez);

    for (int i=0; i< chunk.sizex * chunk.sizez; i++){
        Vec3f vertex;
        vertex.x = i%chunk.sizex;
        vertex.z = i/chunk.sizex;

        // sample y from noise map
        int samplex = (int)(vertex.x *2 + chunkPos.x)%gen->noiseMapw;
        int samplez = ((int)(vertex.z *2 + chunkPos.z)%gen->noiseMaph);
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

        vertex.y *= yAmplitude;
        vertex.y += chunkPos.y;

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


/*
    stitches chunks a and b 
    changes upto d1 x n vertices in a and d2 x n vertices in b
    influenceFactorA is the influence factor for chunk A
    determines how much of chunk A will be changed 

    p is how much of the previous terrain affects the 
    p + q = 1

    new y = oldy * inf factor + (a + (b-a) * t) * (1-inf factor)
*/
void stitchTerrain(TerrainChunk *a, TerrainChunk *b, int ndepth, float p, float influenceFactorA){
    int boundaryIndexA, boundaryIndexB;
    bool xBoundary = false;

    // find the border x or border z
    if ((int)a->chunkOrigin.x == (int)b->chunkOrigin.x){
        if (a->chunkOrigin.z > b->chunkOrigin.z){
            boundaryIndexA = 0;
            boundaryIndexB = b->sizez-1;
        }
        else{
            boundaryIndexA = a->sizez-1;
            boundaryIndexB = 0;
        }
        xBoundary = false;
    }
    else if ((int)a->chunkOrigin.z == (int)b->chunkOrigin.z){
        if (a->chunkOrigin.x > b->chunkOrigin.x){
            boundaryIndexA = 0;
            boundaryIndexB = b->sizex-1;
        }
        else{
            boundaryIndexA = a->sizex-1;
            boundaryIndexB = 0;
        }
        xBoundary = true;
    }

    float influenceFactorB = 1.0f - influenceFactorA;
    // depth in each chunk upto where the vertices are affected
    int d1 = ndepth * (influenceFactorB);
    int d2 = ndepth * (influenceFactorA);


    // if the boundary of the two chunks is x = __
    if (xBoundary){
        // calculate the x at respective depths of the chunks
        int x1 = (boundaryIndexA == 0)?d1:boundaryIndexA-d1;
        int x2 = (boundaryIndexB == 0)?d2:boundaryIndexB-d2;

        // start and end for the changes
        int startX1 = (boundaryIndexA == 0)?1:x1;
        int startX2 = (boundaryIndexB == 0)?1:x2;
        int endX1 = (boundaryIndexA == 0)?x1:boundaryIndexA-1;
        int endX2 = (boundaryIndexB == 0)?x2:boundaryIndexB-1;

        // interpolate the new values at the border
        for (int changeZ =0; changeZ<a->sizez; ++changeZ){
            float borderYA = a->vertices[boundaryIndexA + changeZ * a->sizez].y;
            float borderYB = b->vertices[boundaryIndexB + changeZ * b->sizez].y;
            float newBorderY = lerp(borderYA, borderYB, influenceFactorB);
            // float newBorderY = lerp(borderYA, borderYB, t);
            a->vertices[boundaryIndexA + changeZ * a->sizez].y = newBorderY;
            b->vertices[boundaryIndexB + changeZ * b->sizez].y = newBorderY;
        }

        // calculate the new y values for chunk A
        // a1 and b1 are the inner depth y and outer border y
        for (int changeZ =0; changeZ<a->sizez; ++changeZ){
            float a1 = a->vertices[x1 + changeZ * a->sizez].y;
            float b1 = a->vertices[boundaryIndexA + changeZ * a->sizez].y;
            for (int changeX=startX1; changeX<= endX1; ++changeX){
                float t = fabs((float)(changeX - x1)/d1);
                float oldy = a->vertices[changeX + changeZ * a->sizez].y;
                float newY = lerp(a1, b1, t);
                newY +=  + oldy*p*(1.0f-t);
                a->vertices[changeX + changeZ * a->sizez].y = newY;
            }
        }
        // calculate the new y values for chunk B
        // b2 and a2 are the outer border y and inner depth y
        for (int changeZ =0; changeZ<b->sizez; ++changeZ){
            float a2 = b->vertices[x2 + changeZ * b->sizez].y;
            float b2 = b->vertices[boundaryIndexB + changeZ * b->sizez].y;
            for (int changeX=startX2; changeX<= endX2; ++changeX){
                float t = fabs((float)(changeX - x2)/d2);
                float oldy = b->vertices[changeX + changeZ * b->sizez].y;
                float newY = lerp(a2, b2, t);
                newY +=  + oldy*p*(1.0f-t);
                b->vertices[changeX + changeZ * b->sizez].y = newY;
            }
        }
    }

}
