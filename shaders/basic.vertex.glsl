#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTextureCoordinate;
layout (location = 2) in vec3 aNormal;

out vec2 textureCoordinate;
out vec3 normal;
out vec3 worldPosition;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {
    vec4 position = uModel * vec4(aPosition, 1.0);
    gl_Position = uProjection * uView * position;
    textureCoordinate = aTextureCoordinate;
    normal = aNormal;
    worldPosition = position.xyz;
}
