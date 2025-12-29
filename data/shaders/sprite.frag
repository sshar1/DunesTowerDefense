#version 330 core

out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D tex;

void main() {
//    FragColor = vec4(0.5f, 0.f, 0.5f, 1.f);//texture(tex, texCoord);
    FragColor = texture(tex, texCoord);
}