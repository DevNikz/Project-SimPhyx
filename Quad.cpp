#include "Quad.h"
#include <unordered_map>
using namespace std;
using namespace Physics;

Quad::Quad()
{
    setupMesh();
}

Quad::Quad(int tile) {
    m_tiling = tile;
    setupMesh();
}

Quad::Quad(glm::vec2 t) {
    tile = t;
    setupMesh();
}

//Quad::~Quad()
//{
//    releaseGLResources();
//}

Quad::Quad(Quad&& other) noexcept
{
    *this = std::move(other);
}

Quad& Quad::operator=(Quad&& other) noexcept
{
    if (this == &other) return *this;
    releaseGLResources();

    m_VAO = other.m_VAO; m_VBO = other.m_VBO; m_EBO = other.m_EBO;
    m_textureID = other.m_textureID;
    m_textureWidth = other.m_textureWidth;
    m_textureHeight = other.m_textureHeight;
    m_ownsTexture = other.m_ownsTexture;
    m_position = other.m_position;
    m_scale = other.m_scale;
    m_rotationDeg = other.m_rotationDeg;
    m_color = other.m_color;
    m_transform = other.m_transform;
    m_dirty = other.m_dirty;
    m_sheetColumns = other.m_sheetColumns;
    m_sheetRows = other.m_sheetRows;
    m_uvTransform = other.m_uvTransform;
    m_animTimer = other.m_animTimer;
    m_animFrame = other.m_animFrame;

    other.m_VAO = other.m_VBO = other.m_EBO = 0;
    other.m_textureID = 0;
    other.m_ownsTexture = false;
    return *this;
}

void Quad::releaseGLResources()
{
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    //if (m_textureID) glDeleteTextures(1, &m_textureID);
    if (m_textureID && m_ownsTexture) glDeleteTextures(1, &m_textureID);
    m_VAO = m_VBO = m_EBO = m_textureID = 0;
}

void Quad::DeleteBuffers() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    if (m_textureID && m_ownsTexture) glDeleteTextures(1, &m_textureID);
}

void Quad::setupMesh()
{
    // Unit quad centered on origin, in the XY plane, facing +Z.
    // Layout per vertex: pos(3) color(3) texcoord(2)
    // TexCoord v is flipped (1 - y) so the top of the image maps to the
    // top of the quad — stb_image loads rows top-to-bottom.
    float vertices[] = {
        // pos                  // color            // texcoord
        -0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,    0.0f, 1.0f * tile.y, // bottom-left
         0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,    1.0f * tile.x, 1.0f * tile.y, // bottom-right
         0.5f,  0.5f, 0.0f,     1.0f, 1.0f, 1.0f,    1.0f * tile.x, 0.0f, // top-right
        -0.5f,  0.5f, 0.0f,     1.0f, 1.0f, 1.0f,    0.0f, 0.0f, // top-left
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    const GLsizei stride = 8 * sizeof(float);

    // aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    // aColor
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // aTexCoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Physics::Quad::loadWhiteTexture()
{
    unsigned int whitePixel = 0xFFFFFFFF;
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &whitePixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

bool Quad::loadTexture(const std::string& path, int sheetColumns, int sheetRows)
{
    m_sheetColumns = std::max(1, sheetColumns);
    m_sheetRows = std::max(1, sheetRows);

    if (m_textureID) glDeleteTextures(1, &m_textureID);
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    // Nearest filtering keeps pixel art crisp; clamp so adjacent frames
    // in the sheet never bleed into each other at the edges.
    if (this->isTiled == false) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
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

    m_textureWidth = width;
    m_textureHeight = height;

    // Default to the first frame (top-left).
    setFrame(0, 0);
    return true;
}

Quad::TextureHandle Quad::LoadTextureCached(const std::string& path)
{
    // Simple path -> handle cache shared by every Quad in the process.
    // Lives for the lifetime of the program (GL context); not individually
    // reference-counted since sprite textures are typically loaded once
    // up front and kept for the whole session.
    static std::unordered_map<std::string, TextureHandle> cache;

    auto it = cache.find(path);
    if (it != cache.end()) return it->second;

    TextureHandle tex;
    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

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
        glDeleteTextures(1, &id);
        return tex; // id == 0, caller should check
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    tex.id = id;
    tex.width = width;
    tex.height = height;
    cache[path] = tex;
    return tex;
}

void Quad::setTexture(const TextureHandle& tex, int sheetColumns, int sheetRows)
{
    if (m_textureID && m_ownsTexture) glDeleteTextures(1, &m_textureID);

    m_textureID = tex.id;
    m_textureWidth = tex.width;
    m_textureHeight = tex.height;
    m_ownsTexture = false; // shared/cached texture — Quad doesn't own it

    m_sheetColumns = std::max(1, sheetColumns);
    m_sheetRows = std::max(1, sheetRows);

    setFrame(0, 0);
}

void Quad::applyColorToVBO()
{
    // aColor is a per-vertex attribute in your shader (not a uniform), so
    // tinting means overwriting the color floats (offset 3 of each 8-float
    // vertex) for all 4 vertices via glBufferSubData.
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    for (int i = 0; i < 4; ++i)
    {
        GLintptr offset = i * 8 * sizeof(float) + 3 * sizeof(float);
        glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(glm::vec3), &m_color[0]);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Quad::setFrame(int col, int row)
{
    col = std::clamp(col, 0, m_sheetColumns - 1);
    row = std::clamp(row, 0, m_sheetRows - 1);

    float scaleX = 1.0f / static_cast<float>(m_sheetColumns);
    float scaleY = 1.0f / static_cast<float>(m_sheetRows);
    float offsetX = col * scaleX;
    float offsetY = row * scaleY;

    /*m_textureWidth = scaleX;
    m_textureHeight = scaleY;*/

    // Matches spriteShader.vert: TexCoord = aTexCoord * uvTransform.xy + uvTransform.zw
    m_uvTransform = glm::vec4(scaleX, scaleY, offsetX, offsetY);
}

void Quad::updateAnimation(float deltaTime, int frameCount, float frameDuration, int row)
{
    if (frameCount <= 0 || frameDuration <= 0.0f) return;

    m_animTimer += deltaTime;
    if (m_animTimer >= frameDuration)
    {
        m_animTimer -= frameDuration;
        m_animFrame = (m_animFrame + 1) % frameCount;
        setFrame(m_animFrame, row);
    }
}

void Quad::updateTransform()
{
    m_transform = glm::translate(glm::mat4(1.0f), m_position);
    m_transform = glm::rotate(m_transform, glm::radians(m_rotationDeg), glm::vec3(0.0f, 0.0f, 1.0f));
    m_transform = glm::scale(m_transform, glm::vec3(m_scale.x * facingScale, m_scale.y, 1.0f));
    m_dirty = false;
}

void Quad::draw(Shader& shader)
{
    if (m_dirty) updateTransform();

    shader.setMat4("transform", 1, m_transform);
    shader.setVec4("uvTransform", 1, m_uvTransform);
    /*shader.setMat4("transform", m_transform);
    shader.setVec4("uvTransform", m_uvTransform);*/

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    //shader.setFloat("tiling", m_tiling);
    shader.setInt("diffuseMap", 0);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Quad::draw()
{
    if (m_dirty) updateTransform();

    shader->setMat4("transform", 1, m_transform);
    shader->setVec4("uvTransform", 1, m_uvTransform);
    /*shader.setMat4("transform", m_transform);
    shader.setVec4("uvTransform", m_uvTransform);*/

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    shader->setFloat("tiling", m_tiling);
    shader->setInt("diffuseMap", 0);
    shader->setFloat("alpha", m_alpha);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


void Quad::setFrameByPixel(int pixelX, int pixelY, int tileWidth, int tileHeight)
{
    // Requires you've stored the loaded texture's pixel dimensions;
    // see the tweak to loadTexture() below.
    float scaleX = static_cast<float>(tileWidth) / m_textureWidth;
    float scaleY = static_cast<float>(tileHeight) / m_textureHeight;
    float offsetX = static_cast<float>(pixelX) / m_textureWidth;
    float offsetY = static_cast<float>(pixelY) / m_textureHeight;

    m_uvTransform = glm::vec4(scaleX, scaleY, offsetX, offsetY);
}