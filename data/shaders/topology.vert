#version 330 core

layout (location = 0) in int depth;

out vec3 vertexColor;

uniform int gridWidth;
uniform int gridHeight;
uniform int tallestDepth;
uniform int shortestDepth;
uniform mat3 warpMatrix;

const vec3 c_red    = vec3(0.9, 0.1, 0.1);  // Peaks
const vec3 c_orange = vec3(1.0, 0.6, 0.0);  // High ground
const vec3 c_yellow = vec3(1.0, 1.0, 0.0);  // Mid ground
const vec3 c_green  = vec3(0.1, 0.8, 0.2);  // Low ground
const vec3 c_cyan   = vec3(0.0, 0.8, 0.9);  // Shallow water
const vec3 c_blue   = vec3(0.0, 0.0, 0.6);  // Deep water

const int mmetersPerXCoord = 500;
const int mmetersPerYCoord = 300;

float remap(float x) {
    return 1 / (1 + exp(-2 * (x - 0.5)));
}

vec3 getColor(float t) {
    float scaled = t * 5.0;

    float idx = floor(scaled);
    float fractPart = fract(scaled);

    fractPart = smoothstep(0.0, 1.0, fractPart);

    if (idx < 1.0) return mix(c_red, c_orange, fractPart);
    if (idx < 2.0) return mix(c_orange, c_yellow, fractPart);
    if (idx < 3.0) return mix(c_yellow, c_green, fractPart);
    if (idx < 4.0) return mix(c_green, c_cyan, fractPart);
    if (idx < 5.0) return mix(c_cyan, c_blue, fractPart);
}

// This is not perfect, it is just an estimate
// Pos goes from -1 to 1.
int getRealDepth(int rawDepth, vec2 pos) {
//    float xPos = mmetersPerXCoord * pos.x;
//    float yPos = mmetersPerYCoord * (pos.y + 1) / 2;
//    float flatMagSquared = (xPos * xPos) + (yPos * yPos);
//
//    return int(sqrt((rawDepth * rawDepth) - flatMagSquared));
    return rawDepth;
}

void main() {
    int xidx = gl_VertexID % gridWidth;
    int yidx = gl_VertexID / gridWidth;

    float xNorm = float(xidx) / gridWidth;
    float yNorm = float(yidx) / gridHeight;

    vec3 warpedHomogenous = warpMatrix * vec3(xNorm, yNorm, 1.f);
    vec2 warped = warpedHomogenous.xy / warpedHomogenous.z;

    float xPos = (warped.x * 2) - 1;
    float yPos = (warped.y * 2) - 1;
    vec2 pos = vec2(xPos, -yPos);

    gl_Position = vec4(pos, 0.f, 1.f);

    float range = shortestDepth - tallestDepth;
    int realDepth = getRealDepth(depth, pos);
    float depthNorm = float(realDepth - tallestDepth) / range;
    depthNorm = clamp(depthNorm, 0.f, 1.f);

    vertexColor = getColor(depthNorm);
//    vertexColor = mix(tallColor, shortColor, remap(depthNorm));

//    int difference = depth - realDepth; // How many mm are being shaved off?
//
//    // Normalize the difference (e.g., 0mm to 100mm becomes 0.0 to 1.0)
//    float debugNorm = float(difference) / 100.0;
//    vertexColor = vec3(debugNorm);
}