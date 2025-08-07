#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) flat in uint fragTextureIndex;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D texSampler[128];

void main() { outColor = texture(texSampler[fragTextureIndex], fragTexCoord) * fragColor; }