#include "renderer.h"
#include "mesh.h"


#include <stdio.h>


int initRenderer(Renderer *r, const char *title, int width, int height){
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


    // r->width = width;
    // r->height = height;
    // r->font = NULL;
    // r->textShader = compileShader("./shaders/text_vs.vs","./shaders/text_fs.fs");

    // glGenVertexArrays(1, &r->textVAO);
    // glBindVertexArray(r->textVAO);

    // glGenBuffers(1, &r->textVBO);
    // glBindBuffer(GL_ARRAY_BUFFER, r->textVBO);
    // glBufferData(GL_ARRAY_BUFFER, 7*sizeof(float)*50, 0, GL_STATIC_DRAW);

    // glGenBuffers(1, &r->textEBO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->textEBO);

    // uint32_t indices[] = {
    //     0,1,2,
    //     2,3,0,
    // };
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // /*
    //     vertex      -> vec2  2
    //     texoffset    -> float 1
    //     pos         -> vec2  2
    //     texcoords   -> vec2 2
    // */
    // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), 0);    
    // glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(2*sizeof(float)));
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(3*sizeof(float)));
    // glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(5*sizeof(float)));

    // glEnableVertexAttribArray(0);
    // glEnableVertexAttribArray(1);
    // glEnableVertexAttribArray(2);
    // glEnableVertexAttribArray(3);
    // glBindVertexArray(0);



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