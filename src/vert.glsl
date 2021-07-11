#version 330 core

precision mediump float;

layout(location = 0) in vec3 IN_VERTEX;
layout(location = 1) in vec3 IN_NORMAL;
layout(location = 2) in mat4 IN_TRANSLATE;
layout(location = 6) in vec3 IN_COLOR;

// NOTE: `TIME` unused!
uniform float TIME;
uniform vec3  POSITION;
uniform mat4  PROJECTION;
uniform mat4  VIEW;

out vec3 VERT_OUT_VERTEX;
out vec3 VERT_OUT_NORMAL;
out vec3 VERT_OUT_POSITION;
out vec3 VERT_OUT_COLOR;

void main() {
    VERT_OUT_VERTEX = vec3(IN_TRANSLATE * vec4(IN_VERTEX, 1.0));
    VERT_OUT_NORMAL = mat3(transpose(inverse(IN_TRANSLATE))) * IN_NORMAL;
    VERT_OUT_POSITION = POSITION;
    VERT_OUT_COLOR = IN_COLOR;
    gl_Position = PROJECTION * VIEW * IN_TRANSLATE * vec4(IN_VERTEX, 1.0);
}
