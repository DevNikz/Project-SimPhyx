#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "Shader.h"
#include "tiny_obj_loader.h"
#include "stb_image.h"
using namespace std;


namespace Physics {
    class Quad {
    public:
        void Init();
        void LoadDiffuse(const std::string& path, bool flipVertically = true);
        bool loadTexture(const std::string& path, int sheetColumns = 1, int sheetRows = 1);
        void Draw();
        void Destroy();

        Physics::Shader* GetShader();
        void AssignShader(Physics::Shader* s);

        void Scale(glm::vec3 s);
        void Position(glm::vec3 p);
        void Rotation(glm::vec3 r);

        bool useMipmaps = false;

        //Animation
        void setFrame(int col, int row);
        void updateAnimation(float deltaTime, int frameCount, float frameDuration, int row = 0);

        void SetSpriteSheet(int columns, int rows, int totalFrames, float fps);
        void UpdateAnimation(float deltaTime);
        void SetFrame(int frameIndex);
        glm::vec4 GetUVTransform() const;

    private:
        unsigned int VAO = 0, VBO = 0, EBO = 0;
        Physics::Shader* shader;

        glm::vec3 scale = glm::vec3(1.f);
        glm::vec3 pos = glm::vec3(1.f);
        glm::vec3 rotation = glm::vec3(0.f);

        GLuint diffuse;
        GLuint GetDiffuse();

        //Animation
        int   spriteColumns = 1;
        int   spriteRows = 1;
        int   frameCount = 1;
        int   currentFrame = 0;
        float frameDuration = 0.1f;
        float frameTimer = 0.f;
        bool  animated = false;

        glm::vec4 m_uvTransform{ 1.0f, 1.0f, 0.0f, 0.0f };
    };
}