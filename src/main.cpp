#include "terrain.h"
#include "./Graphics/renderer.h"
#include "./Graphics/camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

#include <stdio.h>
#include <stdint.h>


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
    TerrainGenerator gen(CHUNK_SIZE, 0.3f,0.3f);
    // generate noise map
    generateNoiseMap(&gen, 8*CHUNK_SIZE, 8*CHUNK_SIZE);

    // initialize renderer
    Renderer r;
    initRenderer(&r,WINDOW_TITLE, WINDOW_W, WINDOW_H, true);

    glfwSetCursorPosCallback(r.window, mouseMoveCallback);
    glfwSetInputMode(r.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    
    Shader terrainShader = compileShader("./shaders/terrain_vs.vert", "./shaders/terrain_fs.frag");


    
    camera.init({0,0,1},{0,1,0},{1,0,0}, 0.1f,100.0f, r.width, r.height);
    camera.pos = {0,0,3};
    
    while (!glfwWindowShouldClose(r.window)){
        glfwPollEvents();
        if (glfwGetKey(r.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            break;

        // clear screen
        clearScreen(&r, Vec4f{0,0.560,0.85,0});

        // update camera
        camera.updatePos(cameraUpdate(r.window), 100.0f);
        // get lookat matrix
        Mat4 view = camera.lookat(camera.pos - camera.front, Vec3f{0,1,0});
        
        // projection matrix 
        // (wanted to implement this myself tara didnt work for some reason)
        // (so used glm to get the matrix (column major) and convert it to Mat4 (row major)) 
        glm::mat4x4 projs = glm::perspective(glm::radians(45.0f), (float)r.width/r.height, 0.1f,300.0f);
        Mat4 p = *((Mat4*)&projs);
        Mat4 proj = transpose(p);


        // generate chunks procedurally
        proceduralGenerate(&gen, camera.pos, camera.front);
        

        // draw the chunk meshes
        for (int i =0; i<gen.chunkObjects.size(); i++){
            Vec3f worldCoords = gen.chunkObjects[i].origin;
            Vec3f chunkToCamera = camera.pos - worldCoords;

            // compare direction with camera front
            bool inView = dotProduct(normalize(chunkToCamera), camera.front) > 0.0f;
            
            // see if chunk is within threshold 
            const float threshold = 200.0f;            
            bool withinRadius = dotProduct(chunkToCamera, chunkToCamera) < threshold * threshold;
            
            if (inView || withinRadius) {
                Mat4 model = translate(worldCoords.x,-10.0f, worldCoords.z);
                drawMesh(&r, &gen.chunkObjects[i].mesh, &terrainShader, model, view, proj);
            }
        }
        glfwSwapBuffers(r.window);
    }


    cleanup();
    
    return 0;
}




uint32_t cameraUpdate(GLFWwindow *window){

    auto iskeyPressed = [&](int keyCode)-> bool
    {
        return glfwGetKey(window, keyCode) == GLFW_PRESS;
    };

    uint32_t movement = 0;
    if (iskeyPressed(GLFW_KEY_W))
        movement |= CameraMovement::FRONT;
    else if (iskeyPressed(GLFW_KEY_S))
        movement |= CameraMovement::BACK;

    if (iskeyPressed(GLFW_KEY_A))
        movement |= CameraMovement::LEFT;
    else if (iskeyPressed(GLFW_KEY_D))
        movement |= CameraMovement::RIGHT;

    return(movement);
}