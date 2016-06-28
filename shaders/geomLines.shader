#version 150

layout(lines) in;
layout(line_strip, max_vertices = 2) out;

void main() {
    gl_PointSize = 2.0;
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    EndPrimitive();
}
