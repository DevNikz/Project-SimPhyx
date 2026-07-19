#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct PlayerInput {
    glm::vec2 moveDir{ 0.0f, 0.0f };
    bool jumpPressed = false;   // true only on the frame the key goes down
    bool jumpHeld = false;      // true while held (for variable jump height)
    bool facingLeft = false;

private:
    bool jumpWasDown = false;   // internal state to detect edge press
};

inline void UpdatePlayerInput(GLFWwindow* window, PlayerInput& input) {
    float x = 0.0f, y = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        x -= 1.0f;
        input.facingLeft = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        x += 1.0f;
        input.facingLeft = false;
    }
    //if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    //    //y += 50.0f;
    //}
    //if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    //    y -= 1.0f;

    input.moveDir = glm::vec2(x, y);
    if (glm::length(input.moveDir) > 0.0f)
        input.moveDir = glm::normalize(input.moveDir);

    // --- Jump (edge-detected) ---
    //bool jumpDown = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    bool jumpDown = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    input.jumpPressed = jumpDown && !input.jumpHeld; // rising edge
    input.jumpHeld = jumpDown;
}