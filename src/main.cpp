#include "terrain.h"
#include "noise.h"
#include "./Graphics/mesh.h"
#include "./Graphics/renderer.h"
#include "./Graphics/camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

#define BMP_USE_VALUES
#include "bmp.h"

#include <stdio.h>
#include <stdint.h>

#define PTABLE_SIZE (4096 * 2)

#define WINDOW_H  720
#define WINDOW_W  1280

#define WINDOW_TITLE "terrain"





uint32_t cameraUpdate(GLFWwindow *window);

Camera3D camera;



void mouseMoveCallback(GLFWwindow* window, double xPosIn, double yPosIn){
    static float prevx = xPosIn;
    static float prevy = yPosIn;
    float xPos = (float)xPosIn;
    float yPos = (float)yPosIn;

    float xOffset = xPos - prevx;
    float yOffset = yPos - prevy;

    prevx = xPos;
    prevy = yPos;

    camera.updateOrientation(xOffset, yOffset);

}



int main(){

    TerrainGenerator gen;
    gen.noiseMaph = 4*CHUNK_SIZE;
    gen.noiseMapw = 4*CHUNK_SIZE;
    gen.noiseMap = (float*) malloc(gen.noiseMaph * gen.noiseMapw * sizeof(*gen.noiseMap));

    uint32_t *ptable = getPermutationTable(PTABLE_SIZE);
    uint32_t *buffer = (uint32_t*) malloc(gen.noiseMaph * gen.noiseMapw * sizeof(*buffer));

    // smaller the multiplier the range is smaller, ie more zoom
    float multiplier = 0.005f;
    for (int i = 0; i< gen.noiseMaph * gen.noiseMapw;i++){
        Vec2f pos;
        pos.x = (i%gen.noiseMapw) * multiplier;
        pos.y = (i/gen.noiseMapw) * multiplier;
        gen.noiseMap[i] = fractionalBrownianMotion2D(pos, ptable, PTABLE_SIZE,4);
    }

    Renderer r;
    initRenderer(&r,WINDOW_TITLE, WINDOW_W, WINDOW_H);

    // glfwSetKeyCallback(r.window, keyPressCallback);
    glfwSetCursorPosCallback(r.window, mouseMoveCallback);
    glfwSetInputMode(r.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    
    Array<Object3D> chunkObjects;
    // addChunk(&gen, {n,0,n}, CHUNK_HILL);
    uint32_t *chunkIndices = getIndices(CHUNK_SIZE, CHUNK_SIZE);
    
    
    
    Shader terrainShader = compileShader("./shaders/terrain_vs.vert", "./shaders/terrain_fs.frag");


    
    camera.init({0,0,1},{0,1,0},{1,0,0}, 0.1f,100.0f, r.width, r.height);
    camera.pos = {0,0,3};

    Object3D m;
    addChunk(&gen, Vec3f{0,20,0}, CHUNK_HILL);
    addChunk(&gen, Vec3f{256,20,0}, CHUNK_HILL);
    addChunk(&gen, Vec3f{256,40,256}, CHUNK_HILL);
    addChunk(&gen, Vec3f{256,50,512}, CHUNK_HILL);
    addChunk(&gen, Vec3f{512,30,256}, CHUNK_HILL);

    chunkObjects.push_back(m);
    chunkObjects.push_back(m);
    chunkObjects.push_back(m);
    chunkObjects.push_back(m);
    chunkObjects.push_back(m);
    // chunkObjects.push_back(m);
    // chunkObjects.push_back(m);
    // createMesh(&chunkObjects[0].mesh, gen.chunks[0].vertices, CHUNK_SIZE*CHUNK_SIZE, chunkIndices, (CHUNK_SIZE-1)*(CHUNK_SIZE-1)*6);
    // createMesh(&chunkObjects[1].mesh, gen.chunks[1].vertices, CHUNK_SIZE*CHUNK_SIZE, chunkIndices, (CHUNK_SIZE-1)*(CHUNK_SIZE-1)*6);
    // chunkObjects[0].origin = gen.chunks[0].chunkOrigin;
    // chunkObjects[1].origin = gen.chunks[1].chunkOrigin;
    // stitchTerrain(&gen.chunks[0],&gen.chunks[1], 100, 0.5f, 0.5f);
    // stitchTerrain(&gen.chunks[2],&gen.chunks[1], 100, 0.5f, 0.5f);
    // stitchTerrain(&gen.chunks[3],&gen.chunks[2], 100, 0.5f, 0.5f);
    // // stitchTerrain(&gen.chunks[3],&gen.chunks[1], 100, 0.5f, 1.0f);
    // stitchTerrain(&gen.chunks[2],&gen.chunks[4], 100, 0.5f, 0.5f);
    createMesh(&chunkObjects[0].mesh, gen.chunks[0].vertices, CHUNK_SIZE*CHUNK_SIZE, chunkIndices, (CHUNK_SIZE-1)*(CHUNK_SIZE-1)*6);
    createMesh(&chunkObjects[1].mesh, gen.chunks[1].vertices, CHUNK_SIZE*CHUNK_SIZE, chunkIndices, (CHUNK_SIZE-1)*(CHUNK_SIZE-1)*6);
    createMesh(&chunkObjects[2].mesh, gen.chunks[2].vertices, CHUNK_SIZE*CHUNK_SIZE, chunkIndices, (CHUNK_SIZE-1)*(CHUNK_SIZE-1)*6);
    createMesh(&chunkObjects[3].mesh, gen.chunks[3].vertices, CHUNK_SIZE*CHUNK_SIZE, chunkIndices, (CHUNK_SIZE-1)*(CHUNK_SIZE-1)*6);
    createMesh(&chunkObjects[4].mesh, gen.chunks[4].vertices, CHUNK_SIZE*CHUNK_SIZE, chunkIndices, (CHUNK_SIZE-1)*(CHUNK_SIZE-1)*6);
    chunkObjects[0].origin = gen.chunks[0].chunkOrigin;
    chunkObjects[1].origin = gen.chunks[1].chunkOrigin;
    chunkObjects[2].origin = gen.chunks[2].chunkOrigin;
    chunkObjects[3].origin = gen.chunks[3].chunkOrigin;
    chunkObjects[4].origin = gen.chunks[4].chunkOrigin;
    
    // gen.chunkGrid[chunkNox][chunkNoz] = true;

    
    while (!glfwWindowShouldClose(r.window)){
        glfwPollEvents();
        if (glfwGetKey(r.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            break;


        clearScreen(&r, Vec4f{0,0.560,0.85,0});

        uint32_t cameraMov = cameraUpdate(r.window);
        

        camera.updatePos(cameraMov, 100.0f);

        Mat4 view = camera.lookat(camera.pos - camera.front, Vec3f{0,1,0});

        glm::mat4x4 projs = glm::perspective(glm::radians(45.0f), (float)r.width/r.height, 0.1f,150.0f);
        Mat4 p = *((Mat4*)&projs);
        Mat4 proj = transpose(p);


        int cameraChunkPosX = camera.pos.x/(CHUNK_SIZE*0.1f);
        int cameraChunkPosZ = camera.pos.z/(CHUNK_SIZE*0.1f);
        // printf("Currently above: %d, %d \n", cameraChunkPosX, cameraChunkPosZ);
        
        // check if a 3x3 
        // for (int i = -1; i<=1; i++){
        //     int chunkNox = cameraChunkPosX+i;
        //     if (chunkNox < 0 || chunkNox > 255) continue;

        //     for (int j = -1; j<=1; j++){
        //         int chunkNoz = cameraChunkPosZ+j;

        //         if (chunkNoz < 0 || chunkNoz > 255) continue;
        //         // if chunk isnt generated 
        //         if (!gen.chunkGrid[chunkNox][chunkNoz]){
        //             // generate chunk data
        //             addChunk(&gen, Vec3f{(float)chunkNox * CHUNK_SIZE, 0, (float)chunkNoz *CHUNK_SIZE}, CHUNK_HILL);

        //             // add new object
        //             Object3D m;
        //             chunkObjects.push_back(m);
                    
        //             int totalChunks = chunkObjects.size();
        //             createMesh(&chunkObjects[totalChunks-1].mesh, gen.chunks[totalChunks-1].vertices, CHUNK_SIZE*CHUNK_SIZE, chunkIndices, (CHUNK_SIZE-1)*(CHUNK_SIZE-1)*6);
        //             chunkObjects[totalChunks - 1].origin = gen.chunks[totalChunks - 1].chunkOrigin;
        //             gen.chunkGrid[chunkNox][chunkNoz] = true;
        //         }
        //     }
        // }

        // add new object
        


        for (int i =0; i<chunkObjects.size(); i++){
        // for (int i =0; i<1; i++){
            Vec3f worldCoords = chunkObjects[i].origin;
            

            if (dotProduct(camera.pos, worldCoords) < 1024*1024){
                Mat4 model = translate(worldCoords.x*0.1f,0, worldCoords.z*0.1f)*scaleAboutOrigin(0.1,1,0.1);
                // Mat4 model = translate(0,0,0);
                drawMesh(&r, &chunkObjects[i].mesh, &terrainShader, model, view, proj);
            }
        }
        glfwSwapBuffers(r.window);
    }


    cleanup();
    




    return 0;
}




uint32_t cameraUpdate(GLFWwindow *window){
    uint32_t movement = 0;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        movement |= CameraMovement::FRONT;
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        movement |= CameraMovement::BACK;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        movement |= CameraMovement::LEFT;
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        movement |= CameraMovement::RIGHT;

    return(movement);
}