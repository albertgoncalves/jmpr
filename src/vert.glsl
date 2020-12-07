#version 330 core

precision mediump float;

layout(location = 0) in vec3 IN_VERTEX;
layout(location = 1) in mat4 IN_TRANSLATE;
layout(location = 5) in vec4 IN_COLOR;

// NOTE: `U_TIME` unused!
uniform float U_TIME;
uniform mat4  U_PROJECTION;
uniform mat4  U_VIEW;

out vec4 VERT_OUT_COLOR;

void main() {
    VERT_OUT_COLOR = IN_COLOR;
    gl_Position = U_PROJECTION * U_VIEW * IN_TRANSLATE * vec4(IN_VERTEX, 1.0);
}
