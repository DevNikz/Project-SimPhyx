#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 uvTransform; // x,y = offset into sheet, z,w = frame scale

out vec3 vColor;
out vec2 TexCoord;

void main()
{
    gl_Position = projection * view * transform * vec4(aPos, 1.0);
    vColor = aColor;
    TexCoord = aTexCoord * uvTransform.xy + uvTransform.zw;
}