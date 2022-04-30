#version 300 es
precision mediump float;

in vec2 TexCoord;
out vec4 FragColor;

// texture samplers
uniform sampler2D texture1;
uniform vec4 color;

void main()
{
    vec4 font_color = vec4(1.0);
    if (color != vec4(0.0)) {
        font_color = color;
    }

    FragColor = vec4(
        font_color.xyz, texture(texture1, TexCoord).r * font_color.w);
}
