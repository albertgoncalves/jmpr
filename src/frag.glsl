#version 330 core

precision mediump float;

in vec3 VERT_OUT_VERTEX;
in vec3 VERT_OUT_NORMAL;
in vec3 VERT_OUT_POSITION;
in vec3 VERT_OUT_COLOR;

layout(location = 0) out vec4 FRAG_OUT_COLOR;

void main() {
    vec3  normal = normalize(VERT_OUT_NORMAL);
    vec3  direction = normalize(VERT_OUT_POSITION - VERT_OUT_VERTEX);
    float brightness = (max(dot(normal, direction), 0.35)) + 0.35;
    FRAG_OUT_COLOR = vec4(brightness * VERT_OUT_COLOR, 1.0);
}
