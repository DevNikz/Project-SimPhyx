#version 330 core
in vec3 vColor;
in vec2 TexCoord;

uniform sampler2D diffuseMap;

out vec4 FragColor;

void main()
{
    vec4 texColor = texture(diffuseMap, TexCoord);
    if (texColor.a < 0.01)
        discard; // skip fully transparent pixels, useful for sprite cutouts

    FragColor = texColor * vec4(vColor, 1.0);
}