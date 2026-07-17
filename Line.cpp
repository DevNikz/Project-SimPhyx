#include "Line.h"
using namespace std;
using namespace Physics;

Line::Line(const glm::vec3& start, const glm::vec3& end, Physics::Shader* s)
{
    Init(start, end);
	AssignShader(s);
}

void Line::Init(const glm::vec3& start, const glm::vec3& end) {
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

void Line::Draw(const glm::vec3& start, const glm::vec3& end) {
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

void Line::Color(glm::vec3 c) { color = c; }

Shader* Line::GetShader() { return this->shader; }
void Line::AssignShader(Shader* s) { this->shader = s; }
