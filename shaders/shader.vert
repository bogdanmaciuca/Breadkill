#version 450

layout(std140, set = 1, binding = 0) uniform Projection {
    mat4 uProj;
};

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in uint aTexIdx;

layout(location = 0) out uint oTexIdx;
layout(location = 1) out vec2 oTexCoord;

layout(location = 0) out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position = uProj * vec4(aPos, 0.0, 1.0);
    oTexIdx = aTexIdx;
    oTexCoord = aTexCoord;
}

