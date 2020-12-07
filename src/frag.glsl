#version 330 core

precision mediump float;

in vec4 VERT_OUT_COLOR;

#define K 3

void main() {
    gl_FragColor = VERT_OUT_COLOR;
}
