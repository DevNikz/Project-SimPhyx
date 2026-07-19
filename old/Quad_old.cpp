#include "Quad_old.h"

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
        // pos                  // color            // texcoord
        -0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,    0.0f, 1.0f, // bottom-left
         0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,    1.0f, 1.0f, // bottom-right
         0.5f,  0.5f, 0.0f,     1.0f, 1.0f, 1.0f,    1.0f, 0.0f, // top-right
        -0.5f,  0.5f, 0.0f,     1.0f, 1.0f, 1.0f,    0.0f, 0.0f, // top-left
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

    const GLsizei stride = 8 * sizeof(float);

    // aPos - location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // aColor - location 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // aTexCoord - location 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
            return;
        }

        glGenTextures(1, &diffuse);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        if (useMipmaps)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_LINEAR if you want soft edges but crisp frames
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_NEAREST keeps pixel art crisp

        stbi_image_free(data);
    }
    else
    {
        std::cerr << "LoadTexture: failed to load texture at " << path << std::endl;
        std::cerr << "  reason: " << stbi_failure_reason() << std::endl;
        stbi_image_free(data);
    }
}

bool Quad::loadTexture(const std::string& path, int sheetColumns, int sheetRows)
{
    spriteColumns = std::max(1, sheetColumns);
    spriteRows = std::max(1, sheetRows);

    if (diffuse) glDeleteTextures(1, &diffuse);
    glGenTextures(1, &diffuse);
    glBindTexture(GL_TEXTURE_2D, diffuse);

    // Nearest filtering keeps pixel art crisp; clamp so adjacent frames
    // in the sheet never bleed into each other at the edges.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_set_flip_vertically_on_load(false);
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
    if (!data)
    {
        std::cerr << "[Quad] Failed to load texture: " << path << std::endl;
        return false;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    // Default to the first frame (top-left).
    setFrame(0, 0);
    return true;
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
    this->shader->setVec4("uvTransform", 1, GetUVTransform());

    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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

void Quad::SetSpriteSheet(int columns, int rows, int totalFrames, float fps)
{
    spriteColumns = columns;
    spriteRows = rows;
    frameCount = totalFrames;
    frameDuration = (fps > 0.f) ? (1.0f / fps) : 0.1f;
    currentFrame = 0;
    frameTimer = 0.f;
    animated = true;
}

void Quad::UpdateAnimation(float deltaTime)
{
    if (!animated || frameCount <= 1) return;

    frameTimer += deltaTime;
    while (frameTimer >= frameDuration)
    {
        frameTimer -= frameDuration;
        currentFrame = (currentFrame + 1) % frameCount;
    }
}

void Quad::SetFrame(int frameIndex)
{
    if (frameCount > 0)
        currentFrame = frameIndex % frameCount;
}

glm::vec4 Quad::GetUVTransform() const
{
    if (!animated || spriteColumns <= 0 || spriteRows <= 0)
        return glm::vec4(0.f, 0.f, 1.f, 1.f); // identity, full texture

    int col = currentFrame % spriteColumns;
    int row = currentFrame / spriteColumns;

    float uScale = 1.0f / (float)spriteColumns;
    float vScale = 1.0f / (float)spriteRows;

    float uOffset = col * uScale;
    // sheets are usually authored top-down, but v=0 is the bottom in GL UV space,
    // so flip the row to keep frame 0 = top-left visually
    float vOffset = 1.0f - vScale - (row * vScale);

    return glm::vec4(uOffset, vOffset, uScale, vScale);
}

void Quad::setFrame(int col, int row)
{
    col = std::clamp(col, 0, spriteColumns - 1);
    row = std::clamp(row, 0, spriteRows - 1);

    float scaleX = 1.0f / static_cast<float>(spriteColumns);
    float scaleY = 1.0f / static_cast<float>(spriteRows);
    float offsetX = col * scaleX;
    float offsetY = row * scaleY;

    // Matches spriteShader.vert: TexCoord = aTexCoord * uvTransform.xy + uvTransform.zw
    m_uvTransform = glm::vec4(scaleX, scaleY, offsetX, offsetY);
}

void Quad::updateAnimation(float deltaTime, int frameCount, float frameDuration, int row)
{
    if (frameCount <= 0 || frameDuration <= 0.0f) return;

    frameTimer += deltaTime;
    if (frameTimer >= frameDuration)
    {
        frameTimer -= frameDuration;
        currentFrame = (currentFrame + 1) % frameCount;
        setFrame(currentFrame, row);
    }
}
