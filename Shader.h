#pragma once
#include "Camera.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

namespace Physics {
	class Shader 
	{
	    public:
		    unsigned int ID;
		    Shader(const char* vertexPath, const char* fragmentPath);

		    void use();
		    void setBool(const std::string& name, bool value) const;
            void setInt(const std::string& name, int value) const;
            void setFloat(const std::string& name, float value) const;
            void setVec3(const std::string& name, int i, glm::vec3 value) const;
            void setMat4(const std::string& name, int i, glm::mat4 value) const;
            void setVec4(const std::string& name, int i, glm::vec3 value) const;
            void LoadBillboardTex(GLuint diffuse);
            void LoadTexture(GLuint diffuse);
            void LoadOverlay(GLuint overlay);
            void LoadNormal(GLuint normal);

            void PassSkybox(Camera cam);

            void PassOrthoSkybox(Camera cam);

            void PassSkybox(Camera cam, glm::vec3 target);

            void passPerspectiveCamera(Camera cam);

            void passPerspectiveCamera(Camera cam, glm::vec3 target);

            void passOrthoCamera(Physics::Camera cam);

            void passPointLight(const std::string& name, glm::vec3 position, glm::vec3 color,
                float strength, float ambientStr, glm::vec3 ambientColor,
                float specularStr, float specularPhong, float constant,
                float linear, float quadratic, float range);

            void passDirLight(const std::string& name, glm::vec3 direction, glm::vec3 color,
                float strength, float ambientStr, glm::vec3 ambientColor,
                float specularStr, float specularPhong);


        private:
            void checkCompileErrors(unsigned int shader, std::string type);
	};
}