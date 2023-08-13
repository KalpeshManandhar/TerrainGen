#version 440 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;

out float _t;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main(){
    vec4 pos = vec4(in_pos, 1);
    vec4 a = model*pos;
    float t = a.y;
    t /= 1000.0f;
    _t = t;
    gl_Position = projection*view*a;

}