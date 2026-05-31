#version 330 core

uniform int uType;
uniform float uSize;
uniform vec4 uColor;

out vec4 fragColor;

void main() {
    vec2 centerOffset = gl_PointCoord - vec2(0.5);
    float distance = length(centerOffset);
    float arm = 0.08;
    float radius = 0.45;

    bool visible = false;

    if (uType == 0) {
        visible = distance < 0.3;
    } else if (uType == 1) {
        visible = abs(centerOffset.x) < arm || abs(centerOffset.y) < arm;
    } else if (uType == 2) {
        visible = distance > radius - arm && distance < radius;
    }

    if (!visible)
    discard;

    fragColor = uColor;
}
