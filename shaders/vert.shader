#version 150

in vec3 coords;
uniform mat4 mvpMatrix;

void main() {
    gl_Position = mvpMatrix * vec4(coords, 1.0);
}
