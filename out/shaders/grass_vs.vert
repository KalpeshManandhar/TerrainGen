#version 440 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;

out vec3 _pos;
out vec4 _color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const vec4 moss1 = vec4(0.22f, 0.31f, 0f, 1f);
const vec4 moss2 = vec4(0.48f, 0.51f, 0.34f, 1f);
const vec4 moss3_dark = vec4(0.19f, 0.2f, 0f, 1f);
const vec4 moss4_light = vec4(0.91f, 0.98f, 0.76f, 1f);

int count = 0;
int index = 0;

float random (in vec2 _st) {
    return fract(sin(dot(_st.xy,vec2(12.9898,78.233)))*43758.5453123);
}

void main(){

    const vec4 moss[] = {
        moss1, moss2, moss3_dark, moss4_light
    };

    vec4 pos = vec4(in_pos, 1);
    
    gl_Position = projection*view*model * pos;
    _pos = in_pos;
    _color = moss[index];
    count++;
    if (count % 3 ==0)
        index++;
}