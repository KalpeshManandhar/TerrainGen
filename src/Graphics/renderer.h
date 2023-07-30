#pragma once

#include <stdint.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "./shader.h"
#include "./mesh.h"

#include "../math/vec.h"

struct Renderer{
    GLFWwindow *window;
    uint32_t height;
    uint32_t width;

    Shader *shader;
};


struct Object3D{
    Mesh mesh;
    Shader shader;
    Vec3f origin;
};



int initRenderer(Renderer *r, const char *title, int width, int height);
int clearScreen(Renderer *r, Vec4f color);
int cleanup();
int drawMesh(Renderer *r, Mesh *m, Shader *shader, Mat4 model, Mat4 view, Mat4 projection);


