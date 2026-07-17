#pragma once
#include <string>
#include <vector>
#include "Shader.h"
#include "tiny_obj_loader.h"
#include "stb_image.h"
using namespace std;


namespace Physics {
    class Quad {
    public:
        void Init();
        void LoadDiffuse(const std::string& path, bool flipVertically = true);
        void Draw();
        void Destroy();

        Physics::Shader* GetShader();
        void AssignShader(Physics::Shader* s);

        void Scale(glm::vec3 s);
        void Position(glm::vec3 p);
        void Rotation(glm::vec3 r);

    private:
        unsigned int VAO = 0, VBO = 0, EBO = 0;
        Physics::Shader* shader;

        glm::vec3 scale = glm::vec3(1.f);
        glm::vec3 pos = glm::vec3(1.f);
        glm::vec3 rotation = glm::vec3(0.f);

        GLuint diffuse;
        GLuint GetDiffuse();
    };
}