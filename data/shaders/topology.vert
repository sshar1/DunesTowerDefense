#version 330 core

layout (location = 0) in int depth;

out vec3 vertexColor;

uniform int gridWidth;
uniform int gridHeight;
uniform int tallestDepth;
uniform int shortestDepth;

const vec3 tallColor = vec3(194.f/255, 26.f/255, 14.f/255);   // Red
const vec3 shortColor = vec3(83.f/255, 217.f/255, 11.f/255);  // Green

void main() {
    int xidx = gl_VertexID % gridWidth;
    int yidx = gl_VertexID / gridWidth;

    float xNorm = float(xidx) / gridWidth;
    float yNorm = float(yidx) / gridHeight;

    float xPos = (xNorm * 2) - 1;
    float yPos = (yNorm * 2) - 1;

    gl_Position = vec4(xPos, -yPos, 0.f, 1.f);

    float range = shortestDepth - tallestDepth;
    float depthNorm = float(depth - tallestDepth) / range;
    depthNorm = clamp(depthNorm, 0.f, 1.f);
    vertexColor = mix(tallColor, shortColor, depthNorm);
}