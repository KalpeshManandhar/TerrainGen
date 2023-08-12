#include "./terrain.h"
#include "./Allocator/allocator.h"

#include <thread>

#include "debug.h"


#define ALLOCATOR_INITIAL 51200 // 200mb 
#define PAGE_SIZE 4096

#define CHUNK_GRID_DIM 256


TerrainGenerator::TerrainGenerator(int dim){
    initFreeList(&allocator, ALLOCATOR_INITIAL * PAGE_SIZE, CHUNK_SIZE*CHUNK_SIZE*sizeof(Vec3f));
    
    chunkGrid = (TerrainChunk *)malloc(CHUNK_GRID_DIM*CHUNK_GRID_DIM * sizeof(*chunkGrid));
    memset(chunkGrid, 0, CHUNK_GRID_DIM*CHUNK_GRID_DIM * sizeof(*chunkGrid));
    sizex = sizez = dim;
    scaleX = scaleZ = 0.1f;

    chunkObjects.reserve(64*64*sizeof(Object3D));

    indices = getIndices(sizex, sizez);
}

TerrainGenerator::~TerrainGenerator(){
    free(chunkGrid);
    destroy(&allocator);
}


// doesnt actually add to the generator 
// just returns the generated chunk
TerrainChunk addChunk(TerrainGenerator *gen, Vec3f chunkPos, float amplMultiplier){
    TerrainChunk chunk;
    chunk.vertices = (Vec3f *)freeListAlloc(&gen->allocator);

    // n_time sttime = get_current_time();

    auto sampleY = [&](int startIndex, int endIndex){
        for (int i=startIndex; i< endIndex; i++){
            Vec3f vertex;
            vertex.x = i%gen->sizex;
            vertex.z = i/gen->sizex;

            // sample y from noise map
            int samplex = (int)(vertex.x *2 + chunkPos.x)%gen->noiseMapw;
            int samplez = ((int)(vertex.z *2 + chunkPos.z)%gen->noiseMaph);
            vertex.y = 1.0f - gen->noiseMap[(samplex + samplez * gen->noiseMapw)];
            vertex.y = vertex.y * 2.0f - 1.0f;

            vertex.x *= gen->scaleX;
            vertex.z *= gen->scaleZ;

            vertex.y *= amplMultiplier;
            vertex.y += chunkPos.y;

            chunk.vertices[i] = vertex;
        }
    };

    int total = gen->sizex * gen->sizez;
    sampleY(0, total);
    // std::thread t1(sampleY, 0, total/2);
    // std::thread t2(sampleY, total/2, total);
    // t1.join();
    // t2.join();
    
    // n_time endtime = get_current_time();
    // fprintf(stdout, "Time taken %lf", t_diff(sttime, endtime));

    // for (int i=0; i< gen->sizex * gen->sizez; i++){
    //     Vec3f vertex;
    //     vertex.x = i%gen->sizex;
    //     vertex.z = i/gen->sizex;

    //     // sample y from noise map
    //     int samplex = (int)(vertex.x *2 + chunkPos.x)%gen->noiseMapw;
    //     int samplez = ((int)(vertex.z *2 + chunkPos.z)%gen->noiseMaph);
    //     vertex.y = 1.0f - gen->noiseMap[(samplex + samplez * gen->noiseMapw)];
    //     vertex.y = vertex.y * 2.0f - 1.0f;

    //     vertex.x *= gen->scaleX;
    //     vertex.z *= gen->scaleZ;

    //     vertex.y *= amplMultiplier;
    //     vertex.y += chunkPos.y;

    //     chunk.vertices[i] = vertex;
    // }


    chunk.chunkOrigin = chunkPos;
    return chunk;
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

    t = |(current - inner)/(inner - border)|
    newY = lerp(inner, border, t) + oldy*p*(1.0f-t);

*/
void stitchTerrain(TerrainGenerator *gen, TerrainChunk *a, TerrainChunk *b, int ndepth, float p, float influenceFactorA){
    int boundaryIndexA, boundaryIndexB;
    bool xBoundary = false;

    // find the border x or border z
    if ((int)a->chunkOrigin.x == (int)b->chunkOrigin.x){
        if (a->chunkOrigin.z > b->chunkOrigin.z){
            boundaryIndexA = 0;
            boundaryIndexB = gen->sizez-1;
        }
        else{
            boundaryIndexA = gen->sizez-1;
            boundaryIndexB = 0;
        }
        xBoundary = false;
    }
    else if ((int)a->chunkOrigin.z == (int)b->chunkOrigin.z){
        if (a->chunkOrigin.x > b->chunkOrigin.x){
            boundaryIndexA = 0;
            boundaryIndexB = gen->sizex-1;
        }
        else{
            boundaryIndexA = gen->sizex-1;
            boundaryIndexB = 0;
        }
        xBoundary = true;
    }

    float influenceFactorB = 1.0f - influenceFactorA;
    // depth in each chunk upto where the vertices are affected
    int d1 = ndepth * (influenceFactorB);
    int d2 = ndepth * (influenceFactorA);


    // if the boundary of the two chunks is x 
    if (xBoundary){
        // calculate the x at respective depths of the chunks
        int x1 = (boundaryIndexA == 0)?d1:boundaryIndexA-d1;
        int x2 = (boundaryIndexB == 0)?d2:boundaryIndexB-d2;

        // compute the new values at the border
        for (int changeZ =0; changeZ<gen->sizez; ++changeZ){
            float borderYA = a->vertices[boundaryIndexA + changeZ * gen->sizez].y;
            float borderYB = b->vertices[boundaryIndexB + changeZ * gen->sizez].y;

            float newBorderY = lerp(borderYA, borderYB, influenceFactorB);
            
            a->vertices[boundaryIndexA + changeZ * gen->sizez].y = newBorderY;
            b->vertices[boundaryIndexB + changeZ * gen->sizez].y = newBorderY;
        }

        // calculate the new y values for chunk A
        if (d1 > 0){
            for (int changeZ =0; changeZ<gen->sizez; ++changeZ){
                // stitching shape is triangle
                // t1 is t value for line from center to corner
                int zDistFromMid = abs(changeZ - gen->sizez/2);
                float t1 = (float)zDistFromMid*2.0f/gen->sizez;

                // start and end for the changes
                int startX1 = (boundaryIndexA == 0)?1:x1 + t1 * d1+1;
                int endX1 = (boundaryIndexA == 0)?x1 - t1 * d1:boundaryIndexA;
                int xStride = endX1 - startX1;

                // inner depth point
                int depthPoint = (boundaryIndexA == 0)?endX1:startX1;

                // a = inner point value
                // b = boundary point value
                float a1 = a->vertices[depthPoint + changeZ * gen->sizez].y;
                float b1 = a->vertices[boundaryIndexA + changeZ * gen->sizez].y;

                for (int changeX=startX1; changeX< endX1; ++changeX){
                    float t = fabs((float)(changeX - depthPoint)/xStride);
                    float oldy = a->vertices[changeX + changeZ * gen->sizez].y;
                    float newY = lerp(a1, b1, t);
                    newY += oldy*p*0.01f*(1.0f-t);
                    a->vertices[changeX + changeZ * gen->sizez].y = newY;
                }
            }
        }
        // calculate the new y values for chunk B
        if (d2 >0){
            for (int changeZ =0; changeZ<gen->sizez; ++changeZ){
                // stitching shape is triangle
                // t1 is t value for line from center to corner
                int zDistFromMid = abs(changeZ - gen->sizez/2);
                float t2 = (float)zDistFromMid*2.0f/gen->sizez;

                // start and end for the changes
                int startX2 = (boundaryIndexB == 0)?1:x2 + t2 * d2;
                int endX2 = (boundaryIndexB == 0)?x2 - t2 * d2:boundaryIndexB;
                int xStride = endX2 - startX2;

                // inner depth point
                int depthPoint = (boundaryIndexB == 0)?endX2:startX2;
                
                // a = inner point value
                // b = boundary point value
                float a2 = b->vertices[depthPoint + changeZ * gen->sizez].y;
                float b2 = b->vertices[boundaryIndexB + changeZ * gen->sizez].y;

                for (int changeX=startX2; changeX< endX2; ++changeX){
                    float t = fabs((float)(changeX - depthPoint)/xStride);
                    float oldy = b->vertices[changeX + changeZ * gen->sizez].y;
                    float newY = lerp(a2, b2, t);
                    newY += oldy*p*0.01f*(1.0f-t);
                    b->vertices[changeX + changeZ * gen->sizez].y = newY;
                }
            }
        }
    }
    // for z boundary
    else {
        // calculate the z at respective inner depths of the chunks
        int z1 = (boundaryIndexA == 0)?d1:boundaryIndexA-d1;
        int z2 = (boundaryIndexB == 0)?d2:boundaryIndexB-d2;

        // compute the new values at the border
        for (int changeX =0; changeX<gen->sizez; ++changeX){
            float borderYA = a->vertices[changeX + boundaryIndexA * gen->sizez].y;
            float borderYB = b->vertices[changeX + boundaryIndexB * gen->sizez].y;

            float newBorderY = lerp(borderYA, borderYB, influenceFactorB);
            
            a->vertices[changeX + boundaryIndexA * gen->sizez].y = newBorderY;
            b->vertices[changeX + boundaryIndexB * gen->sizez].y = newBorderY;
        }

        // calculate the new y values for chunk A
        if (d1 > 0){
            for (int changeX =0; changeX<gen->sizez; ++changeX){
                // stitching shape is triangle
                // t1 is t value for line from center to corner
                int xDistFromMid = abs(changeX - gen->sizex/2);
                float t1 = (float)xDistFromMid*2.0f/gen->sizex;

                // start and end for the changes
                int startZ1 = (boundaryIndexA == 0)?1:z1 + t1 * d1;
                int endZ1 = (boundaryIndexA == 0)?z1 - t1 * d1:boundaryIndexA;
                int zStride = endZ1 - startZ1;

                // inner depth point
                int depthPoint = (boundaryIndexA == 0)?endZ1:startZ1;
                
                // a is value at inner depth
                // b is value at border
                float a1 = a->vertices[changeX + depthPoint * gen->sizez].y;
                float b1 = a->vertices[changeX + boundaryIndexA  * gen->sizez].y;


                for (int changeZ=startZ1; changeZ< endZ1; ++changeZ){
                    float t = fabs((float)(changeZ - depthPoint)/zStride);
                    float oldy = a->vertices[changeX + changeZ * gen->sizez].y;
                    float newY = lerp(a1, b1, t);
                    newY += oldy*p*0.01f*(1.0f-t);
                    a->vertices[changeX + changeZ * gen->sizez].y = newY;
                }
            }
        }
        // calculate the new y values for chunk B
        if (d2 > 0){
            for (int changeX =0; changeX<gen->sizex; ++changeX){
                // stitching shape is triangle
                // t is t value for line from center to corner
                int xDistFromMid = abs(changeX - gen->sizex/2);
                float t2 = (float)xDistFromMid*2.0f/gen->sizex;

                // start and end for the changes
                int startZ2 = (boundaryIndexB == 0)?1:z2 + t2 * d2;
                int endZ2 = (boundaryIndexB == 0)?z2 - t2 * d2:boundaryIndexB;
                int zStride = endZ2 - startZ2;

                // inner depth point
                int depthPoint = (boundaryIndexB == 0)?endZ2:startZ2;
                
                // a is value at inner depth
                // b is value at border
                float a2 = b->vertices[changeX + depthPoint * gen->sizez].y;
                float b2 = b->vertices[changeX + boundaryIndexB * gen->sizez].y;

                for (int changeZ=startZ2; changeZ< endZ2; ++changeZ){
                    float t = fabs((float)(changeZ - depthPoint)/zStride);
                    float oldy = b->vertices[changeX + changeZ * gen->sizez].y;
                    float newY = lerp(a2, b2, t);
                    newY += oldy*p*0.01f*(1.0f-t);
                    b->vertices[changeX + changeZ * gen->sizez].y = newY;
                }
            }
        }
    }
}



void proceduralGenerate(TerrainGenerator *gen, Vec3f cameraPos, Vec3f cameraFront){
    
    Vec2i gridPosCamera = {cameraPos.x/(gen->scaleX * gen->sizex), cameraPos.z/(gen->scaleZ*gen->sizez)};
    
    const int viewChunkRange = 4;
    
    
    for (int i = 0; i<viewChunkRange; i++){
        for (int j = 0; j<viewChunkRange; j++){
            Vec2i gridPos = {gridPosCamera.x + i, gridPosCamera.y + j}; // actually represents the x and z
            
            if (gridPos.x < 0 || gridPos.y < 0)
                continue;

            // if chunk is already generated
            if (gen->isChunkGenerated[gridPos.x][gridPos.y])
                continue;
            

            float yAmplitude = 15.0f;
            float yHeight = 10.0f;
            Vec3f chunkPos = {
                gridPos.x * gen->sizex,
                yHeight,
                gridPos.y * gen->sizez, 
            };
            Vec3f chunkPosWScale = {
                gridPos.x * gen->sizex * gen->scaleX,
                yHeight,
                gridPos.y * gen->sizez * gen->scaleZ, 
            };
            
            // if chunk lies outside a threshold of the view of camera
            // camera front is TOWARDS the camera not the direction camera is looking in
            Vec3f cameraToChunk = normalize(cameraPos - chunkPosWScale);
            const float similarityThreshold = 0.5;
            // if (dotProduct(cameraToChunk, cameraFront) < similarityThreshold)
            //     return;
            

            gen->chunkGrid[gridPos.x + gridPos.y * gen->sizez] = addChunk(gen, chunkPos, yAmplitude);
            TerrainChunk *newChunk = &gen->chunkGrid[gridPos.x + gridPos.y * gen->sizez];            
            
            // stitching chunks
            // right
            if ((gridPos.x + 1)>=0 && gen->isChunkGenerated[gridPos.x+1][gridPos.y]){
                TerrainChunk *right = &gen->chunkGrid[(gridPos.x + 1)  + gridPos.y * gen->sizez];
                stitchTerrain(gen, right, newChunk, gen->sizex/2, 0.01f, 1.0f);
            }
            // left
            if ((gridPos.x - 1)>=0 && gen->isChunkGenerated[gridPos.x-1][gridPos.y]){
                TerrainChunk *left = &gen->chunkGrid[(gridPos.x - 1)  + gridPos.y * gen->sizez];
                stitchTerrain(gen, left, newChunk, gen->sizex/2, 0.01f, 1.0f);
            }
            // // front
            // if ((gridPos.y + 1)>=0 && gen->isChunkGenerated[gridPos.x][gridPos.y+1]){
            //     TerrainChunk *front = &gen->chunkGrid[gridPos.x  + (gridPos.y+1) * gen->sizez];
            //     stitchTerrain(gen, front, newChunk, gen->sizex/2, 0.01f, 1.0f);
            // }
            // // back
            // if ((gridPos.y - 1)>=0 && gen->isChunkGenerated[gridPos.x][gridPos.y-1]){
            //     TerrainChunk *back = &gen->chunkGrid[gridPos.x  + (gridPos.y-1) * gen->sizez];
            //     stitchTerrain(gen, back, newChunk, gen->sizex/2, 0.01f, 1.0f);
            // }
            

            // generate mesh for new chunk
            Object3D newChunkObj; 
            createMesh(&newChunkObj.mesh, newChunk->vertices, gen->sizex * gen->sizez, gen->indices, (gen->sizex-1)*(gen->sizez-1)*6);
            newChunkObj.origin = chunkPosWScale;


            gen->chunkObjects.push_back(newChunkObj);
            gen->isChunkGenerated[gridPos.x][gridPos.y] = true;
            return;
        }
    }
}