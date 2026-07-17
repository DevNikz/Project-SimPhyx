#include "Quad.h"
using namespace Physics;

Shader* Quad::GetShader() {
    return this->shader;
}

void Quad::AssignShader(Physics::Shader* s) {
    this->shader = s;
}

GLuint Quad::GetDiffuse() {
    return this->diffuse;
}

void Quad::Init()
{
    float vertices[] = {
        // positions          // colors           // texcoords
        -0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,    0.0f, 0.0f, // bottom-left
         0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,    1.0f, 0.0f, // bottom-right
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,    1.0f, 1.0f, // top-right
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,    0.0f, 1.0f, // top-left
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    int stride = 8 * sizeof(float);

    // aPos - location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // aColor - location 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // aTexCoord - location 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Quad::LoadDiffuse(const std::string& path, bool flipVertically)
{
    
    stbi_set_flip_vertically_on_load(flipVertically);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        GLenum format{};
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else
        {
            std::cerr << "LoadTexture: unexpected channel count (" << nrChannels << ") for " << path << std::endl;
            stbi_image_free(data);
        }

        glGenTextures(1, &diffuse);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Wrapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cerr << "LoadTexture: failed to load texture at " << path << std::endl;
        std::cerr << "  reason: " << stbi_failure_reason() << std::endl;
        stbi_image_free(data);
    }
}

void Quad::Draw()
{
    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, pos);
    m = glm::scale(m, scale);
    m = glm::rotate(m, rotation.x, glm::vec3(1.f, 0.f, 0.f));
    m = glm::rotate(m, rotation.y, glm::vec3(0.f, 1.f, 0.f));
    m = glm::rotate(m, rotation.z, glm::vec3(0.f, 0.f, 1.f));
    this->shader->setMat4("transform", 1, m);
    this->shader->LoadTexture(GetDiffuse());

    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    //glBindVertexArray(0);
}

void Quad::Destroy()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Quad::Scale(glm::vec3 s) {
    scale = s;
}

void Quad::Position(glm::vec3 p)
{
    pos = p;
}

void Quad::Rotation(glm::vec3 r)
{
    rotation = r;
}
