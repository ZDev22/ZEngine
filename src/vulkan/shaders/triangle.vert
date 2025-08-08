#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) flat out uint fragTextureIndex;
layout(location = 3) flat out uint fragTextureArraySize;

struct SpriteData {
    vec2 position;
    vec2 scale;
    mat2 rotationMatrix;
    vec4 color;
    vec2 uvOffset;
    vec2 uvScale;
    uint textureIndex;
    float rotation;
};

layout(push_constant) uniform PushConstants { 
    mat4 projection;
    vec2 camera;
} push;

layout(set = 0, binding = 0) readonly buffer SpriteDataBuffer { SpriteData sprites[1000]; };

void main() {
    float rotation = radians(sprites[gl_InstanceIndex].rotation);
    vec2 transformedPos = sprites[gl_InstanceIndex].rotationMatrix * (inPosition * sprites[gl_InstanceIndex].scale) + (sprites[gl_InstanceIndex].position + push.camera);

    gl_Position = push.projection * vec4(transformedPos, 0.0, 1.0);
    fragColor = sprites[gl_InstanceIndex].color;
    fragTexCoord = sprites[gl_InstanceIndex].uvOffset + inTexCoord * sprites[gl_InstanceIndex].uvScale;
    fragTextureIndex = sprites[gl_InstanceIndex].textureIndex;
}