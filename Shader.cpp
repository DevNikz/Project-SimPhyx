#include "Shader.h"


namespace Physics {
	Shader::Shader(const char* vertexPath, const char* fragmentPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void Shader::use()
    {
        glUseProgram(ID);
    }

    void Shader::setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void Shader::setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void Shader::setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void Shader::setVec3(const std::string& name, int i, glm::vec3 value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), i, glm::value_ptr(value));
    }

    void Shader::setMat4(const std::string& name, int i, glm::mat4 value) {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), i, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::LoadTexture(GLuint diffuse) {
        glActiveTexture(GL_TEXTURE0);
        GLuint tex0 = glGetUniformLocation(ID, "diffuseMap");
        glUniform1i(tex0, 0);
        glBindTexture(GL_TEXTURE_2D, diffuse);
    }

    void Shader::LoadOverlay(GLuint overlay) {
        glActiveTexture(GL_TEXTURE2);
        GLuint tex2 = glGetUniformLocation(ID, "overlayMap");
        glUniform1i(tex2, 2);
        glBindTexture(GL_TEXTURE_2D, overlay);
    }

    void Shader::LoadNormal(GLuint normal) {
        glActiveTexture(GL_TEXTURE1);
        GLuint tex1 = glGetUniformLocation(ID, "normalMap");
        glUniform1i(tex1, 1);
        glBindTexture(GL_TEXTURE_2D, normal);

    }

    void Shader::PassSkybox(Physics::Camera cam) {
        setMat4("projection", 1, cam.Projection);
        setMat4("view", 1, glm::mat4(glm::mat3(cam.GetPerspectiveViewMatrix())));
    }

    void Shader::PassOrthoSkybox(Physics::Camera cam) {
        setMat4("projection", 1, cam.Projection);
        setMat4("view", 1, glm::mat4(glm::mat3(cam.GetOrthoViewMatrix())));
    }

    void Shader::PassSkybox(Physics::Camera cam, glm::vec3 target) {
        setMat4("projection", 1, cam.Projection);
        setMat4("view", 1, glm::mat4(glm::mat3(cam.GetPerspectiveViewMatrix(target))));
    }

    void Shader::passPerspectiveCamera(Physics::Camera cam) {
        setMat4("projection", 1, cam.Projection);
        setMat4("view", 1, cam.GetPerspectiveViewMatrix());
        setVec3("cameraPos", 1, cam.Position);
    }

    void Shader::passPerspectiveCamera(Physics::Camera cam, glm::vec3 target) {
        setMat4("projection", 1, cam.Projection);
        setMat4("view", 1, cam.GetPerspectiveViewMatrix(target));
        setVec3("cameraPos", 1, cam.Position);
    }

    void Shader::passOrthoCamera(Physics::Camera cam) {
        setMat4("projection", 1, cam.Projection);
        setMat4("view", 1, cam.GetOrthoViewMatrix());
        setVec3("cameraPos", 1, cam.Position);
    }

    void Shader::passPointLight(const std::string& name, glm::vec3 position, glm::vec3 color,
        float strength, float ambientStr, glm::vec3 ambientColor,
        float specularStr, float specularPhong, float constant,
        float linear, float quadratic, float range) {
        setVec3(name + ".position", 1, position);
        setVec3(name + ".color", 1, color);
        setFloat(name + ".strength", strength);
        setFloat(name + ".ambientStr", ambientStr);
        setVec3(name + ".ambientColor", 1, ambientColor);
        setFloat(name + ".specularStr", specularStr);
        setFloat(name + ".specularPhong", specularPhong);
        setFloat(name + ".constant", constant);
        setFloat(name + ".linear", linear);
        setFloat(name + ".quadratic", quadratic);
        setFloat(name + ".range", range);


    }

    void Shader::passDirLight(const std::string& name, glm::vec3 direction, glm::vec3 color,
        float strength, float ambientStr, glm::vec3 ambientColor,
        float specularStr, float specularPhong) {
        setVec3(name + ".direction", 1, glm::normalize(direction));
        setVec3(name + ".color", 1, color);
        setFloat(name + ".strength", strength);
        setFloat(name + ".ambientStr", ambientStr);
        setVec3(name + ".ambientColor", 1, ambientColor);
        setFloat(name + ".specularStr", specularStr);
        setFloat(name + ".specularPhong", specularPhong);


    }

    void Shader::checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
}