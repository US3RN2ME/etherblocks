#version 330 core
out vec4 fragmentColor;

in vec4 color;

void main() {
    fragmentColor = color;
}
