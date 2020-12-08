#version 330 core

precision mediump float;

in vec3 VERT_OUT_VERTEX;
in vec3 VERT_OUT_NORMAL;
in vec3 VERT_OUT_POSITION;
in vec3 VERT_OUT_COLOR;

void main() {
    vec3 normal = normalize(VERT_OUT_NORMAL);
    vec3 direction = normalize(VERT_OUT_POSITION - VERT_OUT_VERTEX);
    vec3 color = (max(dot(normal, direction), 0.25) + 0.25) * VERT_OUT_COLOR;
    gl_FragColor = vec4(color, 1.0);
}
