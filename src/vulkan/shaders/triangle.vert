#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) flat out uint textureID;

struct SpriteData {
    vec2 translation;
    vec2 scale;
    mat2 rotationMatrix;
    vec4 color;
    uint textureIndex;
    float rotation;
};

layout(push_constant) uniform PushConstants { mat4 projection; } push;
layout(set = 0, binding = 0) readonly buffer SpriteDataBuffer { SpriteData sprites[]; };

void main() {
    float rotation = radians(sprites[gl_InstanceIndex].rotation);
    vec2 transformedPos = mat2(cos(rotation), -sin(rotation), sin(rotation), cos(rotation)) * (inPosition * sprites[gl_InstanceIndex].scale) + sprites[gl_InstanceIndex].translation;

    gl_Position = push.projection * vec4(transformedPos, 0.0, 1.0);
    fragColor = sprites[gl_InstanceIndex].color;
    fragTexCoord = inTexCoord;
    textureID = sprites[gl_InstanceIndex].textureIndex;
}