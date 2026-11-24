#version 460

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) flat out uint fragTextureIndex;

struct Sprite {
    vec2 position;
    vec2 scale;
    float rotation;
    uint textureIndex;
    float depth;
    float padding;
};

layout(push_constant) uniform Push {
    vec2 position;
    vec2 zoom;
    float aspect;
} camera;

layout(set = 0, binding = 0) readonly buffer SpriteData { Sprite sprites[]; };

void main() {
    vec2 inTexCoord = vec2(float(gl_VertexIndex & 1), float(gl_VertexIndex >> 1));
    float rotation = radians(sprites[gl_InstanceIndex].rotation);
    gl_Position = vec4(mat2(cos(rotation), -sin(rotation), sin(rotation), cos(rotation)) * (((inTexCoord - 0.5) * sprites[gl_InstanceIndex].scale) / camera.zoom) + ((sprites[gl_InstanceIndex].position + camera.position) / camera.zoom), sprites[gl_InstanceIndex].depth, 1.0);
    gl_Position[0] /= camera.aspect;

    fragTexCoord = inTexCoord;
    fragTextureIndex = sprites[gl_InstanceIndex].textureIndex;
}
