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
    gen.noiseMaph = 8*CHUNK_SIZE;
    gen.noiseMapw = 8*CHUNK_SIZE;
    gen.noiseMap = (float*) malloc(gen.noiseMaph * gen.noiseMapw * sizeof(*gen.noiseMap));

    uint32_t *ptable = getPermutationTable(PTABLE_SIZE);

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

        glm::mat4x4 projs = glm::perspective(glm::radians(45.0f), (float)r.width/r.height, 0.1f,150.0f);
        Mat4 p = *((Mat4*)&projs);
        Mat4 proj = transpose(p);


        int cameraChunkPosX = camera.pos.x/(CHUNK_SIZE*0.1f);
        int cameraChunkPosZ = camera.pos.z/(CHUNK_SIZE*0.1f);
        
        proceduralGenerate(&gen, camera.pos, camera.front);
        


        for (int i =0; i<gen.chunkObjects.size(); i++){
            Vec3f worldCoords = gen.chunkObjects[i].origin;
            
            if (dotProduct(camera.pos, worldCoords) < 1024*1024){
                Mat4 model = translate(worldCoords.x,0, worldCoords.z)*scaleAboutOrigin(1,1,1);
                drawMesh(&r, &gen.chunkObjects[i].mesh, &terrainShader, model, view, proj);
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