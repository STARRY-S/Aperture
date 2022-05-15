#version 300 es
precision mediump float;

in vec2 TexCoord;
out vec4 FragColor;

// texture samplers
uniform sampler2D texture1;
uniform vec4 color;

void main()
{
    vec4 fg_color = color;
    fg_color.a *= texture(texture1, TexCoord.xy).r;
    FragColor = fg_color;
    // FragColor = vec4(color.rgba);
}
