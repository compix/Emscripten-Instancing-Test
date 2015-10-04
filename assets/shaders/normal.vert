#version 100
precision mediump float;

attribute vec3 in_pos;
attribute vec4 in_color; 

uniform mat4 modelViewProj;

varying vec4 color;

void main()
{
    gl_Position = modelViewProj * vec4(in_pos, 1.0);
    color = in_color;
}
