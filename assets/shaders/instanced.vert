#version 100
precision mediump float;

attribute vec3 in_pos;
attribute vec3 in_sPos;

uniform mat4 viewProj;

void main()
{
    gl_Position = viewProj * vec4(in_sPos + in_pos, 1.0);
}
