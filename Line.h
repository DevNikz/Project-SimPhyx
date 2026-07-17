#pragma once
#include <string>
#include <vector>
#include "Shader.h"
#include "tiny_obj_loader.h"
#include "stb_image.h"
using namespace std;

namespace Physics {
	class Line {
	public:
		Line(const glm::vec3& start, const glm::vec3& end, Shader* s);
		void Init(const glm::vec3& start, const glm::vec3& end);
		void Draw(const glm::vec3& start, const glm::vec3& end);
		void Destroy();

		void Color(glm::vec3 c);

		//Shader
		Shader* GetShader();
		void AssignShader(Physics::Shader* s);

	private:
		GLuint vao, vbo;

		glm::vec3 color = glm::vec3(1.f);

		Shader* shader;
	};
}