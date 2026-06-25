#version 330 core
out vec4 fragmentColor;

in vec2 textureCoordinate;
in vec4 color;

uniform sampler2D uTexture;

void main() {
    fragmentColor = texture(uTexture, textureCoordinate) * color;
}
