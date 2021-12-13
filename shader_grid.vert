#version 330 core

uniform mat4 vieww;
uniform mat4 vieww_i;
uniform mat4 proj;
uniform mat4 proj_i;

layout(location = 0) in vec3 pos;

out vec3 wpos;


void main(){
    gl_PointSize = 4.0f;
    gl_Position = proj * vieww * vec4(pos, 1.0);
    wpos = (vec4(pos, 1.0)).xyz;
}