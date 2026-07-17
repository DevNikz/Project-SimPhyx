#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 fragPos;
out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 transform;
uniform mat4 view;

void main()
{
    // Extract world position (translation) from transform
    vec3 worldPos = vec3(transform[3]);

    // Extract scale so billboard size still matches your transform
    vec3 scale = vec3(
        length(vec3(transform[0])),
        length(vec3(transform[1])),
        length(vec3(transform[2]))
    );

    // Camera right/up vectors, pulled from the view matrix's rows
    vec3 camRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 camUp    = vec3(view[0][1], view[1][1], view[2][1]);

    // Build the billboarded vertex position (assumes quad lies in XY plane)
    vec3 billboardPos = worldPos
        + camRight * aPos.x * scale.x
        + camUp    * aPos.y * scale.y;

    fragPos = billboardPos;
    gl_Position = projection * view * vec4(billboardPos, 1.0);
    ourColor = aColor;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}