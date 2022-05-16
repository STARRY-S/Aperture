#version 300 es
precision mediump float;

in vec2 TexCoord;
out vec4 FragColor;

// The texture 1 is used for rendering font or single color shapes
// (by specifing color uniform variable)
// texture 2 is used for rendering images,
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec4 color;
uniform int texture_num;

void main()
{
    vec4 tex1 = texture(texture1, TexCoord.xy);
    vec4 tex2 = texture(texture2, TexCoord.xy);
    vec4 fg_color;

    if (texture_num == 0) {
        // texture1 only have RED and Alpha color value, and other
        // color values (Green and Blue) is always zero
        fg_color = color;
        fg_color.a *= tex1.r;
    } else {
        fg_color = tex2;
    }
    FragColor = fg_color;
}
