#pragma once
#include <string>
#include <vector>
#include "Shader.h"
#include "tiny_obj_loader.h"
#include "stb_image.h"
using namespace std;

namespace Physics {
    class Model
    {
        private:
            std::vector<tinyobj::shape_t> shapes;
            std::vector<tinyobj::material_t> materials;
            std::string error;
            tinyobj::attrib_t attributes;
            std::vector<GLuint> mesh_indices;
            GLuint vao, vbo, ebo, vbo_uv;
            std::vector<GLfloat> fullVertexData;
            GLuint diffuseTexture;
            GLuint normalTexture;
            GLuint overlayTexture;
            std::vector<glm::vec3> tangents;
            std::vector<glm::vec3> bitangents;

            glm::vec3 scale = glm::vec3(1.f);
            glm::vec3 pos = glm::vec3(1.f);
            glm::vec3 color = glm::vec3(1.f);
            glm::vec3 rotation = glm::vec3(0.f);

            Physics::Shader* shader;

        public:
            string modelName;
            string diffuseName;

            Model();
            Model(string _model, string _tex, Physics::Shader* s);
            void InitModel();
            void InitDefaultMesh();
            void DrawModel();
            void DeleteBuffers();
            void InitTexture(const std::string& path);

            void Scale(glm::vec3 s);
            void Position(glm::vec3 p);
            void Color(glm::vec3 c);
            void Rotation(glm::vec3 r);

            GLuint GetDiffuse();
            Physics::Shader* GetShader();
            void AssignShader(Physics::Shader* s);
    };
}