#version 440 core

in vec3 color;
in float y;

out vec4 out_color;

void main(){
    out_color = vec4(color * y * 0.1f, 1.0f);
    // out_color = vec4(1.0f,1.0f,1.0f, 1.0f);
}