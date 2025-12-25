#version 330 core

out vec3 vertexColor;

uniform int gridWidth;
uniform int gridHeight;

void main() {
//    gl_PointSize(5.f);
    int xidx = gl_VertexID % gridWidth;
    int yidx = gl_VertexID / gridWidth;

    float xNorm = float(xidx) / gridWidth;
    float yNorm = float(yidx) / gridHeight;

    float xPos = (xNorm * 2) - 1;
    float yPos = (yNorm * 2) - 1;

    gl_Position = vec4(xPos, yPos, 0.f, 1.f);
    vertexColor = vec3(1.0f, 0.f, 0.f);
}