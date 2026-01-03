#version 330 core

layout (location = 0) in int depth;

out vec3 vertexColor;

uniform int gridWidth;
uniform int gridHeight;
uniform int tallestDepth;
uniform int midDepth;
uniform int shortestDepth;
uniform mat3 warpMatrix;

const vec3 tallColor = vec3(228.f/255, 14.f/255, 36.f/255);     // Red
const vec3 midColor = vec3(255.f/255, 244.f/255, 0.f/255);      // Yellow
const vec3 shortColor = vec3(19.f/255, 241.f/255, 0.f/255);     // Green

void main() {
    int xidx = gl_VertexID % gridWidth;
    int yidx = gl_VertexID / gridWidth;

    float xNorm = float(xidx) / gridWidth;
    float yNorm = float(yidx) / gridHeight;

    vec3 warpedHomogenous = warpMatrix * vec3(xNorm, yNorm, 1.f);
    vec2 warped = warpedHomogenous.xy / warpedHomogenous.z;

    float xPos = (warped.x * 2) - 1;
    float yPos = (warped.y * 2) - 1;

    gl_Position = vec4(xPos, -yPos, 0.f, 1.f);

    float range = shortestDepth - tallestDepth;
    float depthNorm = float(depth - tallestDepth) / range;
    depthNorm = clamp(depthNorm, 0.f, 1.f);
    vertexColor = mix(tallColor, shortColor, depthNorm);
}