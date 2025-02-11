#include "../include/BackgroundUtil.h"

// Global Variables
// ----------------
glm::vec3 cameraPos(0.0f, 1.0f, 7.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Mouse Variables
// ---------------
bool firstMouse = true;
bool leftMouseDown = false;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float yaw = -90.0f;					// Yaw rotates left/right
float pitch = 0.0f;					// Pitch rotates up/down
float sensitivity = 0.1f;
glm::vec3 front(0.0f, 0.0f, -1.0f);

/**
 * Callback function for Mouse Movement (Cursor Position)
 * @param window Window
 * @param xpos X-Position
 * @param ypos Y-Position
 * @return Front Vector
 */
glm::vec3 mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (leftMouseDown)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // y-coordinates from bottom to top

		// Update last X and Y
		// -------------------
		lastX = xpos;
		lastY = ypos;

		xoffset *= sensitivity;
		yoffset *= sensitivity;

		// Update yaw and pitch
		// --------------------
		yaw += xoffset;
		pitch += yoffset;

		// Constrain Pitch to prevent Gimbal Lock (Pitch > 89°)
		// ----------------------------------------------------
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(front);
	}
	cameraFront = front;
	return front;
}

/**
 * Callback function for Mouse Button
 * @param window Window
 * @param button Button pressed
 * @param action Action
 * @param mods Mods (Shift, Ctrl, Alt - currently not used)
 */
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			leftMouseDown = true;
			firstMouse = true;
		}
		else if (action == GLFW_RELEASE)
			leftMouseDown = false;
	}
}

/**
 * Process Input
 * @param window Window
 */
void processInput(GLFWwindow* window)
{
	// Camera Movement (time diff between frames)
	// ------------------------------------------
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	float cameraSpeed = 2.5f * deltaTime;

	// Camera Movement on button press
	// -------------------------------
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
