#version 440 core

in float _t;

uniform sampler2D grass;
uniform sampler2D moss;
uniform sampler2D rock;

out vec4 out_color;




void main(){

    vec4 dark[] = {
        vec4(0.18f, 0.19f, 0.16f, 1),
        vec4(0.1f, 0.12f, 0.11f, 1),
        vec4(0.25f, 0.31f, 0.27f, 1),
        vec4(0.09f, 0.11f, 0.09f, 1),
    };

    vec4 brown[] = {
        vec4(0.29f, 0.23f, 0.17f, 1),
        vec4(0.3f, 0.23f, 0.16f, 1),
        vec4(0.24f, 0.2f, 0.12f, 1),
        vec4(0.43f, 0.29f, 0.25f, 1)
    };

    vec4 green[] ={
        vec4(0.32f, 0.4f, 0.11f, 1),
        vec4(0.25f, 0.42f, 0.12f, 1),
        vec4(0.43f, 0.5f, 0.24f, 1),
    };

    

    vec4 blue[] = {
        vec4(0.24f, 0.39f, 0.4f, 1),  
        vec4(0.16f, 0.31f, 0.4f, 1),  
        vec4(0.24f, 0.41f, 0.52f, 1),  
        vec4(0.1f, 0.17f, 0.22f, 1),  
    };

    vec4 light[] ={
        vec4(0.7f, 0.74f, 0.76f, 1),
        vec4(0.84f, 0.89f, 0.91f, 1),
        vec4(0.92f, 0.96f, 1, 1),
        vec4(0.82f, 0.75f, 0.8f, 1),
    };

    int index = int(_t * 1000);
    vec4 brownSample = brown[index%4];
    vec4 greenSample = green[index%3];
    vec4 darkSample = dark[index%4];
    vec4 blueSample = blue[index%4];
    vec4 lightSample = light[index%4];


    vec4 value = mix(lightSample, mix(mix(brownSample, darkSample, _t), mix(blueSample, greenSample, _t), _t), _t);

    out_color = value;
}