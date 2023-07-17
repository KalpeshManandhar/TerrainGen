#pragma once

#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include "math/vec.h"

#define ERROR_CHECK(cond, message)  (!(cond) && printf(message))
#define glGetUniformLocation   glGetUniformLocation_ex


static int glGetUniformLocation_ex(uint32_t shaderID, const char *name){
    int check = glad_glGetUniformLocation(shaderID, name);
    (check == -1)&&fprintf(stderr,"Uniform not found %s\n",name);
    return(check);
}


struct Shader {
    uint32_t id;

    void useShader(){
        glUseProgram(id);
    }

    void setInt(const char *name, int value){
        glUniform1d(glGetUniformLocation_ex(id, name), value);
    }
    void setFloat(const char *name, float value){
        glUniform1f(glGetUniformLocation_ex(id, name), value);
    }
    void setVec3(const char *name, Vec3f value){
        glUniform3f(glGetUniformLocation_ex(id, name), value.x, value.y, value.z);
    }
    void setMat4(const char *name, Mat4 value){
        Mat4 valuetranspose = transpose(value);
        glUniformMatrix4fv(glGetUniformLocation_ex(id, name), 1,GL_FALSE, &valuetranspose.arr[0][0]);
    }

};


Shader compileShader(const char* vertexShaderPath, const char* fragmentShaderPath);
