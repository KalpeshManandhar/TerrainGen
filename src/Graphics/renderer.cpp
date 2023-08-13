#include "renderer.h"
#include "mesh.h"


#include <stdio.h>


int initRenderer(Renderer *r, const char *title, int width, int height, bool vsyncEnable){
    glfwInit();
    // opengl version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window
    r->window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (r->window == NULL){
        fprintf(stderr, "Couldn't create window");
        return(-1);
    }

    glfwMakeContextCurrent(r->window);
    

    // glad load the os specific function pointers for GL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD");
        return -1;
    }

    // set the render window size
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1);


    r->width = width;
    r->height = height;
    return 0;

}

int cleanup(){
    glfwTerminate();
    return 0;
}


int clearScreen(Renderer *r, Vec4f color){
    glClearColor(color.x, color.y, color.z, color.w);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    return 0;
}


int drawMesh(Renderer *r, Mesh *m, Shader *shader,Mat4 model, Mat4 view, Mat4 projection){
    shader->useShader();
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    
    glBindVertexArray(m->vao);
    glDrawElements(GL_TRIANGLES, m->indices.size(), GL_UNSIGNED_INT, (void*)(0*sizeof(float)));
    glBindVertexArray(0);
    return 0;
}