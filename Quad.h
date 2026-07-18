#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include "Shader.h"
#include "stb_image.h"
#include <iostream>
#include <algorithm>

// A textured, sprite-sheet-capable quad matching spriteShader.vert/.frag:
//   layout 0: aPos      (vec3)
//   layout 1: aColor    (vec3)
//   layout 2: aTexCoord (vec2)
//
// Uniforms driven by this class: transform, uvTransform, diffuseMap.
// (view/projection are expected to be set once per frame by the caller,
// since they're shared across every quad drawn with the same shader.)
namespace Physics {
    class Quad
    {
    public:
        Quad();
        ~Quad();

        // Non-copyable (owns GL objects); movable.
        Quad(const Quad&) = delete;
        Quad& operator=(const Quad&) = delete;
        Quad(Quad&& other) noexcept;
        Quad& operator=(Quad&& other) noexcept;

        // Loads a texture from disk (PNG/JPG/etc via stb_image) and binds it
        // to this quad. sheetColumns/sheetRows describe how many frames the
        // sprite sheet is divided into (e.g. 4x1 for your Owlet idle sheet).
        bool loadTexture(const std::string& path, int sheetColumns = 1, int sheetRows = 1);

        // Selects which frame of the sprite sheet to display.
        // col/row are 0-indexed, (0,0) = top-left frame.
        void setFrame(int col, int row);

        // Convenience: advances through columns 0..frameCount-1 on a fixed
        // timer, wrapping around. Call once per frame with deltaTime.
        void updateAnimation(float deltaTime, int frameCount, float frameDuration, int row = 0);

        // Transform (world space)
        void setPosition(const glm::vec3& pos) { m_position = pos; m_dirty = true; }
        void setRotationDegrees(float degrees) { m_rotationDeg = degrees; m_dirty = true; }
        void setScale(const glm::vec2& scale) { m_scale = scale; m_dirty = true; }
        void setColor(const glm::vec3& color) { m_color = color; applyColorToVBO(); }

        const glm::vec3& getPosition() const { return m_position; }

        // Draws the quad using the given shader. The shader must already be
        // `use()`d and have view/projection set by the caller before this.
        void draw(class Shader& shader);

        unsigned int getTextureID() const { return m_textureID; }

    private:
        void setupMesh();
        void updateTransform();
        void releaseGLResources();
        void applyColorToVBO();

        unsigned int m_VAO = 0, m_VBO = 0, m_EBO = 0;
        unsigned int m_textureID = 0;

        glm::vec3 m_position{ 0.0f };
        glm::vec2 m_scale{ 1.0f, 1.0f };
        float m_rotationDeg = 0.0f;
        glm::vec3 m_color{ 1.0f, 1.0f, 1.0f };

        glm::mat4 m_transform{ 1.0f };
        bool m_dirty = true;

        // Sprite sheet state
        int m_sheetColumns = 1;
        int m_sheetRows = 1;
        glm::vec4 m_uvTransform{ 1.0f, 1.0f, 0.0f, 0.0f }; // xy = frame scale, zw = frame offset

        // Animation state
        float m_animTimer = 0.0f;
        int m_animFrame = 0;
    };
}
