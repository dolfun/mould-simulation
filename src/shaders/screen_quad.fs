#version 450 core
out vec4 fragColor;

in vec2 texCoord;

void main() {
  fragColor = vec4(texCoord.x, texCoord.y, 0.0, 1.0);
}