#version 450 core
layout (location = 0) out vec4 fragColor;
layout (location = 0) in vec2 texCoord;

layout (binding = 0) uniform sampler2D tex;

void main() {
  fragColor = vec4(texture(tex, texCoord).rgb, 1.0);
}