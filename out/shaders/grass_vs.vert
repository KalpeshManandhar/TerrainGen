#version 440 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;

out vec3 _pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;



void main(){
    vec4 pos = vec4(in_pos, 1);
    _pos = in_pos;
    gl_Position = projection*view*model * pos;
}