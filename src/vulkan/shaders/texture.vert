#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) flat out uint fragTextureIndex;

struct SpriteData {
    vec2 position;
    vec2 scale;
    mat2 rotationMatrix;
    vec4 color;
    uint textureIndex;
    float rotation;
    uint ID;
};

layout(push_constant) uniform PushConstants { 
    vec2 camera;
} push;

layout(set = 0, binding = 0) readonly buffer SpriteDataBuffer { SpriteData sprites[100000]; };

void main() {
    float rotation = radians(sprites[gl_InstanceIndex].rotation);
    gl_Position = vec4((mat2(cos(rotation), -sin(rotation), sin(rotation), cos(rotation)) * (inPosition * sprites[gl_InstanceIndex].scale) + (sprites[gl_InstanceIndex].position + push.camera)), 0.0, 1.0);
    fragColor = sprites[gl_InstanceIndex].color;
    fragTexCoord = inTexCoord;
    fragTextureIndex = sprites[gl_InstanceIndex].textureIndex;
}