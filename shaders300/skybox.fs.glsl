#version 330 core

in vec3 vTexture;

uniform samplerCube uCubemap;

out vec4 fFragColor;

void main() {
    fFragColor = texture(uCubemap, vTexture);
}
