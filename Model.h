#pragma once
#include <string>
#include <vector>
#include "Shader.h"
#include "tiny_obj_loader.h"
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

            Physics::Shader* shader;

        public:
            string modelName;
            string texName;
            string normName;
            string overlayName;
            string baseMtl;

            Model(string _name, string _tex, string _file, string _norm = "", string _normFile = "", string _over = "", string _overFile = "", string _mtlPath = "3D/");
            void InitModel();
            void InitDefaultMesh();
            //void InitCustomModel();
            //void InitCube();
            //void DrawCube();
            void DrawModel();
            void DeleteBuffers();
            //void InitTextures();
            //void InitOverlayMap();
            //void InitNormals();

            void Scale(glm::vec3 s);
            glm::vec3 GetScale();
            void Position(glm::vec3 p);
            void Color(glm::vec3 c);

            //GLuint GetDiffuse();
            //GLuint GetNormal();
            //GLuint GetOverlay();
            Physics::Shader* GetShader();
            void AssignShader(Physics::Shader* s);
    };
}