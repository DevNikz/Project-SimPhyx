#include "Model.h"

GLfloat UV[]
{
0.f, 1.f,
0.f, 0.f,
1.f, 1.f,
1.f, 0.f,
1.f, 1.f,
1.f, 0.f,
0.f, 1.f,
0.f, 0.f
};


namespace Physics {
    Shader* Model::GetShader() {
        return this->shader;
    }

    void Model::AssignShader(Physics::Shader* s) {
        this->shader = s;
    }

    Model::Model() {
        this->modelName = "";
        this->texName = "";
        this->normName = "";
        this->overlayName = "";
        this->baseMtl = "";

        this->diffuseTexture = 0;
        this->normalTexture = 0;
        this->overlayTexture = 0;
    }

    Model::Model(string _name, string _tex, string _file, string _norm, string _normFile, string _over, string _overFile, string _mtlPath) {
        modelName = "3D/" + _name + ".obj";
        texName = "3D/" + _tex + _file;
        normName = "3D/" + _norm + _normFile;
        overlayName = "3D/" + _over + _overFile;
        baseMtl = _mtlPath;
    }

    void Model::DrawModel() {
        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, pos);
        m = glm::scale(m, scale);
        this->shader->setMat4("transform", 1, m);
        this->shader->setVec3("objectColor", 1, color);

        glBindVertexArray(vao);
        //Vertex Data Method
        glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 8);
    }

    void Model::DrawLine(glm::vec3 start, glm::vec3 end) {
        float updatedVertices[] = {
            start.x, start.y, start.z,
            end.x, end.y, end.z
        };

        // 2. Bind the VBO and update the data
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(updatedVertices), updatedVertices);

        // 3. Render the line
        glBindVertexArray(this->vao);
        glDrawArrays(GL_LINES, 0, 2);

        // 4. Unbind
        glBindVertexArray(0);
        this->shader->setVec3("objectColor", 1, color);
    }

    void Model::InitLine(const glm::vec3& start, const glm::vec3& end) {
        float vertices[] = {
            start.x, start.y, start.z,
            end.x, end.y, end.z
        };

        //(SHADERS) Generate vertices and buffers
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        //(POSITIONS) VBO
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        //Using fullvertexdata
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            vertices,
            GL_DYNAMIC_DRAW
        );
        glVertexAttribPointer(
            0, // Index / buffer index
            3, // x y z
            GL_FLOAT, // array of floats
            GL_FALSE, // if its normalized
            3 * sizeof(GLfloat), // size of data per vertex
            (void*)0
        );
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Model::Scale(glm::vec3 s) {
        scale = s;
    }

    void Model::Position(glm::vec3 p)
    {
        pos = p;
    }

    void Model::Color(glm::vec3 c)
    {
        color = c;
    }
         
    void Model::DeleteBuffers() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }

    void Model::InitDefaultMesh() {
        //Load Object. If success, it loads
        bool success = tinyobj::LoadObj(
            &attributes,
            &shapes,
            &materials,
            &error,
            modelName.c_str()
        );

        
        for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
            mesh_indices.push_back(
                shapes[0].mesh.indices[i].vertex_index
            );
        }

        //(SHADERS) Generate vertices and buffers
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &vbo_uv);
        glGenBuffers(1, &ebo);

        //(POSITIONS) VBO
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(GL_FLOAT) * attributes.vertices.size(),
            &attributes.vertices[0],
            GL_DYNAMIC_DRAW
        );

        //EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            sizeof(GLuint) * mesh_indices.size(),
            mesh_indices.data(),
            GL_DYNAMIC_DRAW
        );
        glVertexAttribPointer(
            //0 = Position
            0, // Index / buffer index
            3, // x y z
            GL_FLOAT, // array of floats
            GL_FALSE, // if its normalized
            3 * sizeof(float), // size of data per vertex
            (void*)0
        );
        glEnableVertexAttribArray(0);

        //UV
        glBindBuffer(GL_ARRAY_BUFFER, vbo_uv);
        glBufferData(GL_ARRAY_BUFFER,
            sizeof(float) * (sizeof(UV) / sizeof(UV[0])),
            &UV[0],
            GL_DYNAMIC_DRAW
        );

        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            2 * sizeof(float),
            (void*)0
        );
        glEnableVertexAttribArray(2);
        glBindVertexArray(0);
        
    }

    
    void Model::InitModel() {
        //Load Object. If success, it loads
        bool success = tinyobj::LoadObj(
            &attributes,
            &shapes,
            &materials,
            &error,
            modelName.c_str()
        );



        for (int i = 0; i < shapes.size(); i++) {
            for (int j = 0; j < shapes[i].mesh.indices.size(); j += 3) {
                tinyobj::index_t vData1 = shapes[i].mesh.indices[j];
                tinyobj::index_t vData2 = shapes[i].mesh.indices[j + 1];
                tinyobj::index_t vData3 = shapes[i].mesh.indices[j + 2];

                //Pos
                glm::vec3 v1 = glm::vec3(
                    attributes.vertices[vData1.vertex_index * 3],
                    attributes.vertices[vData1.vertex_index * 3 + 1],
                    attributes.vertices[vData1.vertex_index * 3 + 2]
                );

                glm::vec3 v2 = glm::vec3(
                    attributes.vertices[vData2.vertex_index * 3],
                    attributes.vertices[vData2.vertex_index * 3 + 1],
                    attributes.vertices[vData2.vertex_index * 3 + 2]
                );

                glm::vec3 v3 = glm::vec3(
                    attributes.vertices[vData3.vertex_index * 3],
                    attributes.vertices[vData3.vertex_index * 3 + 1],
                    attributes.vertices[vData3.vertex_index * 3 + 2]
                );

                //UV
                glm::vec2 uv1 = glm::vec2(
                    attributes.texcoords[(vData1.texcoord_index * 2)],
                    attributes.texcoords[(vData1.texcoord_index * 2) + 1]
                );

                glm::vec2 uv2 = glm::vec2(
                    attributes.texcoords[(vData2.texcoord_index * 2)],
                    attributes.texcoords[(vData2.texcoord_index * 2) + 1]
                );

                glm::vec2 uv3 = glm::vec2(
                    attributes.texcoords[(vData3.texcoord_index * 2)],
                    attributes.texcoords[(vData3.texcoord_index * 2) + 1]
                );

                glm::vec3 deltaPos1 = v2 - v1;
                glm::vec3 deltaPos2 = v3 - v1;

                glm::vec2 deltaUV1 = uv2 - uv1;
                glm::vec2 deltaUV2 = uv3 - uv1;

                float r = 1.0f / ((deltaUV1.x * deltaUV2.y) - (deltaUV1.y * deltaUV2.x));
                glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
                glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

                tangents.push_back(tangent);
                tangents.push_back(tangent);
                tangents.push_back(tangent);

                bitangents.push_back(bitangent);
                bitangents.push_back(bitangent);
                bitangents.push_back(bitangent);
            }
        }

        for (int i = 0; i < shapes.size(); i++) {
            for (int j = 0; j < shapes[i].mesh.indices.size(); j++) {
                tinyobj::index_t vData = shapes[i].mesh.indices[j];
                fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3)]); //X
                fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3) + 1]); //Y
                fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3) + 2]); //Z
                fullVertexData.push_back(attributes.normals[(vData.normal_index * 3)]);
                fullVertexData.push_back(attributes.normals[(vData.normal_index * 3) + 1]);
                fullVertexData.push_back(attributes.normals[(vData.normal_index * 3) + 2]);
                fullVertexData.push_back(attributes.texcoords[(vData.texcoord_index * 2)]); //U
                fullVertexData.push_back(attributes.texcoords[(vData.texcoord_index * 2) + 1]); //V

                fullVertexData.push_back(tangents[i].x);
                fullVertexData.push_back(tangents[i].y);
                fullVertexData.push_back(tangents[i].z);
                fullVertexData.push_back(bitangents[i].x);
                fullVertexData.push_back(bitangents[i].y);
                fullVertexData.push_back(bitangents[i].z);
            }
        }


        GLintptr normalPtr = 3 * sizeof(float);
        GLintptr uvPtr = 6 * sizeof(float);
        GLintptr tangentPtr = 8 * sizeof(float);
        GLintptr bitangentPtr = 11 * sizeof(float);

        //(SHADERS) Generate vertices and buffers
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        //(POSITIONS) VBO
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        //Using fullvertexdata
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(GLfloat) * fullVertexData.size(),
            fullVertexData.data(),
            GL_DYNAMIC_DRAW
        );
        glVertexAttribPointer(
            0, // Index / buffer index
            3, // x y z
            GL_FLOAT, // array of floats
            GL_FALSE, // if its normalized
            14 * sizeof(GLfloat), // size of data per vertex
            (void*)0
        );
        glEnableVertexAttribArray(0);

        //Normal
        glVertexAttribPointer(
            1,
            3, // x y z
            GL_FLOAT, // array of floats
            GL_FALSE, // if its normalized
            14 * sizeof(GLfloat), // size of data per vertex
            (void*)normalPtr
        );
        glEnableVertexAttribArray(1);

        //UV
        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            14 * sizeof(GLfloat),
            (void*)uvPtr
        );
        glEnableVertexAttribArray(2);

        //Tangent
        glVertexAttribPointer(
            3,
            3,
            GL_FLOAT,
            GL_FALSE,
            14 * sizeof(GLfloat),
            (void*)tangentPtr
        );
        glEnableVertexAttribArray(3);

        //Bitangent
        glVertexAttribPointer(
            4,
            3,
            GL_FLOAT,
            GL_FALSE,
            14 * sizeof(GLfloat),
            (void*)bitangentPtr
        );
        glEnableVertexAttribArray(4);


        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}