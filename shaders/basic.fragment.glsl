#version 330 core
out vec4 fragmentColor;

in vec2 textureCoordinate;
in vec3 normal;
in vec3 worldPosition;

uniform sampler2D uTexture;
uniform bool uUseOverrideColor;
uniform vec4 uOverrideColor;
uniform vec4 uSkyLight;
uniform vec4 uGroundLight;
uniform vec4 uCameraPosition;
uniform vec4 uFogColor;
uniform float uFogStart;
uniform float uFogEnd;

void main() {
    if (uUseOverrideColor) {
        fragmentColor = uOverrideColor;
        return;
    }

    vec4 texel = texture(uTexture, textureCoordinate);
    float skyWeight = normal.y * 0.5 + 0.5;
    vec3 light = mix(uGroundLight.rgb, uSkyLight.rgb, skyWeight);
    vec3 litColor = texel.rgb * light;
    float d = length(worldPosition - uCameraPosition.xyz);
    float fog = clamp((d - uFogStart) / (uFogEnd - uFogStart), 0.0, 1.0);
    fog *= fog;
    fragmentColor = vec4(mix(litColor, uFogColor.rgb, fog), texel.a);
}
