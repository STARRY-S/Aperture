#version 300 es
precision mediump float;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

// optimize_zdepth
out float flogz;
out float Fcoef;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float view_distance;

float far = 100.0;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(FragPos, 1.0);

    // refer: http://sirlis.cn/depth-buffer-and-z-fighting/
    Fcoef = 2.0 / log2(view_distance + 1.0);
    flogz = 1.0 + gl_Position.w;
}