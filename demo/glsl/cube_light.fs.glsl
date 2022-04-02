#version 300 es
precision mediump float;

in vec2 TexCoord;
out vec4 FragColor;

// texture samplers
uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);
}
