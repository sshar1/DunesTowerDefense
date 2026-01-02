#version 330 core

layout (location = 0) in int depth;

out vec3 vertexColor;

uniform int gridWidth;
uniform int gridHeight;
uniform int boxDepth;

const vec3 boxColor = vec3(0.f, 0.f, 0.f);
const vec3 otherColor = vec3(1.f, 1.f, 1.f);
const float range = 30;

void main() {
    int xidx = gl_VertexID % gridWidth;
    int yidx = gl_VertexID / gridWidth;

    float xNorm = float(xidx) / gridWidth;
    float yNorm = float(yidx) / gridHeight;

    float xPos = (xNorm * 2) - 1;
    float yPos = (yNorm * 2) - 1;

    gl_Position = vec4(xPos, -yPos, 0.f, 1.f);

    float depthNorm = abs(depth - boxDepth);
    float mask = step(depthNorm, range);
    vertexColor = mask * otherColor;
}