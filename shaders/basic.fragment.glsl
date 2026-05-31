#version 330 core
out vec4 fragmentColor;

in vec2 textureCoordinate;

uniform sampler2D uTexture;
uniform bool uUseOverrideColor;
uniform vec4 uOverrideColor;

void main() {
    if (uUseOverrideColor) {
        fragmentColor = uOverrideColor;
        return;
    }

    fragmentColor = texture(uTexture, textureCoordinate);
}
