#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) flat out uint fragTextureIndex;

struct SpriteData {
    vec2 position;
    vec2 scale;
    mat2 rotationMatrix;
    uint textureIndex;
    uint ID;
    float rotation;
};

layout(push_constant) uniform PushConstants {
    vec2 position;
    vec2 zoom;
} camera;

layout(set = 0, binding = 0) readonly buffer SpriteDataBuffer { SpriteData sprites[]; };

void main() {
    gl_Position = vec4(sprites[gl_InstanceIndex].rotationMatrix * ((inPosition * sprites[gl_InstanceIndex].scale) / camera.zoom) + ((sprites[gl_InstanceIndex].position + camera.position) / camera.zoom), 0.0, 1.0);
    fragTexCoord = inTexCoord;
    fragTextureIndex = sprites[gl_InstanceIndex].textureIndex;
}
