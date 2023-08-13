#version 440 core

in vec3 _pos;
in vec4 _color;

uniform sampler2D grass;
uniform sampler2D moss;
uniform sampler2D rock;

out vec4 out_color;




void main(){


    // float t1 = random(_pos.xz);
    // vec4 value = mix(moss1, moss2, t1);
    out_color = _color;
}