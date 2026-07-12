#include "Camera.h"

namespace Physics {
	Camera::Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        MouseSensitivity = 0.1f;
        updateCameraVectors();
    }

    Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        MouseSensitivity = 0.1f;
        updateCameraVectors();
    }

    glm::mat4 Camera::GetPerspectiveViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    glm::mat4 Camera::GetPerspectiveViewMatrix(glm::vec3 target)
    {
        return glm::lookAt(Position, target, glm::vec3(0.f, 1.f, 0.f));
    }

    glm::mat4 Camera::GetOrthoViewMatrix()
    {
        //return glm::lookAt(Position, Position + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1));
        return glm::lookAt(Position, Position + Front, Up);
    }

    void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {

    }

    void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }
        updateCameraVectors();
    }

    void Camera::updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));

        /*
        Position.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Position.y = sin(glm::radians(Pitch));
        Position.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

        Position.x = objectPosRef.x + OrbitRadius * cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Position.y = objectPosRef.y + OrbitRadius * sin(glm::radians(Pitch));
        Position.z = objectPosRef.z + OrbitRadius * sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));


        Front = glm::normalize(Position);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
        */
    }
    void Camera::ResetOrthoCam()
    {
        this->Position = glm::vec3(0.f, 0.0f, 1.f);
        this->Up = glm::vec3(0.f, 1.0f, 0.0f);
        this->Yaw = Physics::YAW;
        this->Pitch = Physics::PITCH;
        updateCameraVectors();
    }
}