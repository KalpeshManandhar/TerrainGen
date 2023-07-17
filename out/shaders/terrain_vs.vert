#version 440 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;

out vec3 color;
out float y;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    vec4 pos = vec4(in_pos, 1);
    gl_Position = projection*view*model * pos;
    y = pos.y;

    color = normalize(in_pos);
}