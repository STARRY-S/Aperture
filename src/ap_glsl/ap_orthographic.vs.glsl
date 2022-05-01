#version 300 es
precision mediump float;

layout (location = 0) in vec4 aPos; // xy, zw

uniform mat4 projection;

out vec2 TexCoord;

void main()
{
    gl_Position = projection * vec4(aPos.xy, 0.0, 1.0);
	TexCoord = aPos.zw;
}
