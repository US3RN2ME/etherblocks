#version 330 core
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTextureCoordinate;
layout (location = 2) in vec4 aColor;

out vec2 textureCoordinate;
out vec4 color;

uniform mat4 uProjection;

void main() {
    gl_Position = uProjection * vec4(aPosition, 0.0, 1.0);
    textureCoordinate = aTextureCoordinate;
    color = aColor;
}
