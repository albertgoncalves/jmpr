#version 330 core

precision mediump float;

layout(location = 0) in vec3 IN_VERTEX;
layout(location = 1) in mat4 IN_TRANSLATE;

uniform mat4  U_MODEL;
uniform float U_TIME;
uniform mat4  U_PROJECTION;
uniform mat4  U_VIEW;

void main() {
    gl_Position =
        U_PROJECTION * U_VIEW * IN_TRANSLATE * U_MODEL * vec4(IN_VERTEX, 1.0);
}
