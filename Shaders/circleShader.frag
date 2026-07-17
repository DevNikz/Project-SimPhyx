#version 330 core
out vec4 FragColor;
in vec2 vLocalPos;

// Uniforms for easy customization from C++
uniform vec3 circleColor;
uniform vec3 outlineColor;
uniform float thickness;

float radius = 1.0;
float edgeSoftness = 0.01;

void main() {
    float dist = length(vLocalPos);

    // Outer Alpha Masking (Handles edges)
    float outerAlpha = 1.0 - smoothstep(radius - edgeSoftness, radius, dist);

    // Outline Math
    float innerEdge = radius - thickness;
    float outlineFactor = smoothstep(innerEdge - edgeSoftness, innerEdge, dist);

    if (outerAlpha == 0.0) {
        discard;
    }

    vec3 finalColor = mix(circleColor, outlineColor, outlineFactor);
    FragColor = vec4(finalColor, outerAlpha);
}