#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) flat out uint textureID;

layout(push_constant) uniform PushConstants {
    mat4 projection;
} push;

struct SpriteData {
    vec2 translation;
    vec2 scale;
    vec4 color;
    uint textureIndex;
    float rotation;
};

layout(set = 0, binding = 0) readonly buffer SpriteDataBuffer {
    SpriteData sprites[];
};

void main() {
    uint instanceIndex = gl_InstanceIndex;

    vec2 translation = sprites[instanceIndex].translation;
    vec2 scale = sprites[instanceIndex].scale;
    float rotation = radians(sprites[instanceIndex].rotation);

    mat2 rotMatrix = mat2(
        cos(rotation), -sin(rotation),
        sin(rotation), cos(rotation)
    );

    vec2 transformedPos = rotMatrix * (inPosition * scale) + translation;

    gl_Position = push.projection * vec4(transformedPos, 0.0, 1.0);
    fragColor = sprites[instanceIndex].color;
    fragTexCoord = inTexCoord;
    textureID = sprites[instanceIndex].textureIndex;
}