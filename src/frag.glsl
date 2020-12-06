#version 330 core

precision mediump float;

in vec3 VERT_OUT_COLOR;

#define K 3

void main() {
    gl_FragColor = vec4(1.0 - (round(VERT_OUT_COLOR * K) / K), 1.0);
}
