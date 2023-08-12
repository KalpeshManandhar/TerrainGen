#include "terrain.h"
#include "noise.h"
#include "./Graphics/mesh.h"
#include "./Graphics/renderer.h"
#include "./Graphics/camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

#include <stdio.h>
#include <stdint.h>
#include <thread>

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


void generateNoiseMap(TerrainGenerator *gen){
    gen->noiseMap = (float*) malloc(gen->noiseMaph * gen->noiseMapw * sizeof(*gen->noiseMap));

    uint32_t *ptable = getPermutationTable(PTABLE_SIZE);

    // smaller the multiplier the range is smaller, ie more zoom
    float multiplier = 0.005f;

    auto generateNoise = [&](int startIndex, int endIndex){
        for (int i = startIndex; i< endIndex;i++){
            Vec2f pos;
            pos.x = (i%gen->noiseMapw) * multiplier;
            pos.y = (i/gen->noiseMapw) * multiplier;
            gen->noiseMap[i] = fractionalBrownianMotion2D(pos, ptable, PTABLE_SIZE,4);
        }
    };

    int total = gen->noiseMaph * gen->noiseMapw;
    
    std::thread t1[8];
    for (int threadNo=0; threadNo<8; threadNo++){
        int start = threadNo * total/8;
        int end = start + total/8;
        t1[threadNo] = std::thread(generateNoise, start, end); 
    }
    for (int threadNo=0; threadNo<8; threadNo++){
        t1[threadNo].join(); 
    }

}


int main(){
    TerrainGenerator gen(CHUNK_SIZE, 0.3f,0.3f);
    gen.noiseMaph = 8*CHUNK_SIZE;
    gen.noiseMapw = 8*CHUNK_SIZE;
    

    generateNoiseMap(&gen);

    Renderer r;
    initRenderer(&r,WINDOW_TITLE, WINDOW_W, WINDOW_H);

    // glfwSetKeyCallback(r.window, keyPressCallback);
    glfwSetCursorPosCallback(r.window, mouseMoveCallback);
    glfwSetInputMode(r.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    
    
    
    
    Shader terrainShader = compileShader("./shaders/terrain_vs.vert", "./shaders/terrain_fs.frag");


    
    camera.init({0,0,1},{0,1,0},{1,0,0}, 0.1f,100.0f, r.width, r.height);
    camera.pos = {0,0,3};
    
    while (!glfwWindowShouldClose(r.window)){
        glfwPollEvents();
        if (glfwGetKey(r.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            break;


        clearScreen(&r, Vec4f{0,0.560,0.85,0});

        uint32_t cameraMov = cameraUpdate(r.window);
        
        camera.updatePos(cameraMov, 100.0f);

        Mat4 view = camera.lookat(camera.pos - camera.front, Vec3f{0,1,0});

        glm::mat4x4 projs = glm::perspective(glm::radians(45.0f), (float)r.width/r.height, 0.1f,300.0f);
        Mat4 p = *((Mat4*)&projs);
        Mat4 proj = transpose(p);

        proceduralGenerate(&gen, camera.pos, camera.front);
        


        for (int i =0; i<gen.chunkObjects.size(); i++){
            Vec3f worldCoords = gen.chunkObjects[i].origin;
            Vec3f chunkToCamera = normalize(camera.pos - worldCoords);
            
            if (dotProduct(chunkToCamera, camera.front) < 0.0f) 
                continue;
            Mat4 model = translate(worldCoords.x,-10.0f, worldCoords.z);
            drawMesh(&r, &gen.chunkObjects[i].mesh, &terrainShader, model, view, proj);
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