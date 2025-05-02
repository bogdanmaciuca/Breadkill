#version 450

//#define DEBUG


layout (location = 0) flat in uint oTexIdx;
layout (location = 1) in vec2 oTexCoord;

layout (location = 0) out vec4 FragColor;

layout (set = 2, binding = 0) uniform sampler2D sampler0;
layout (set = 2, binding = 1) uniform sampler2D sampler1;

void main() {
#ifdef DEBUG
    uint debugColorIdx = oTexIdx & 3;
    vec3 debugColors[4];
    debugColors[0] = vec3(0.2, 0.4, 0.6);
    debugColors[1] = vec3(0.6, 0.4, 0.2);
    debugColors[2] = vec3(0.2, 0.8, 0.4);
    debugColors[3] = vec3(0.8, 0.2, 0.4);
    FragColor = vec4(debugColors[debugColorIdx], 1);
#else
    switch (oTexIdx) {
        case 0: FragColor = texture(sampler0, oTexCoord); break;
        case 1: FragColor = texture(sampler1, oTexCoord); break;
    }
#endif
}

