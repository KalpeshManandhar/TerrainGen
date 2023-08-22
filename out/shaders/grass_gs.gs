#version 440 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 _pos[];

out vec4 _color;

float random (in vec2 _st) {
    return fract(sin(dot(_st.xy,vec2(12.9898,78.233)))*43758.5453123);
}


void main() {
    const vec4 moss[]={
        vec4(0.19f, 0.2f, 0.0f, 1.0f),
        vec4(0.48f, 0.51f, 0.34f, 1.0f),
        vec4(0.2f, 0.21f, 0.14f, 1.0f),
        vec4(0.32f, 0.4f, 0.11f, 1.0f),
        vec4(0.56f, 0.61f, 0.45f, 1.0f),
    };

    int index = int(_pos[0].y*1000)%6;

    _color = moss[index];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    
    EndPrimitive();
}