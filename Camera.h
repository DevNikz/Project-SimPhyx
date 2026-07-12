#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Physics {
    enum Camera_Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    // Default camera values
    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 25.0f;
    const float SENSITIVITY = 0.05f;
    const float ZOOM = 45.0f;

	class Camera {
    public:
        // camera Attributes
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;
        glm::mat4 Projection;
        // euler Angles
        float Yaw;
        float Pitch;
        // camera options
        float MovementSpeed;
        float MouseSensitivity;

        // constructor with vectors
        Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);
        // constructor with scalar values
        Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

        glm::mat4 GetPerspectiveViewMatrix();

        glm::mat4 GetPerspectiveViewMatrix(glm::vec3 target);

        glm::mat4 GetOrthoViewMatrix();

        void ProcessKeyboard(Camera_Movement direction, float deltaTime);

        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

        void updateCameraVectors();
        void ResetOrthoCam();
	};
}