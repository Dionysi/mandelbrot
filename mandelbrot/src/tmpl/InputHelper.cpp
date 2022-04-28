#include "inputhelper.h"
#include <map>

#include <bitset>

#define NUM_KEYS 1 << 9

/** Mapping from input-helper to the key-maps.*/
std::map<GLFWwindow*, KeyState*> currentKeys = std::map<GLFWwindow*, KeyState*>();
std::map<GLFWwindow*, KeyState*> previousKeys = std::map<GLFWwindow*, KeyState*>();

KeyState operator&(KeyState a, KeyState b) {
	return (KeyState)((int)a & (int)b);
}
KeyState operator|(KeyState a, KeyState b) {
	return (KeyState)((int)a | (int)b);
}

/** Keyboard callback function.
* @param[in] window The window that received the event.
* @param[in] key The [keyboard key](@ref keys) that was pressed or released.
* @param[in] scancode The system-specific scancode of the key.
* @param[in] action `GLFW_PRESS`, `GLFW_RELEASE` or `GLFW_REPEAT`.
* @param[in] mods Bit field describing which [modifier keys] held down.
*/
void InputKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Key is released. 
	if (action == GLFW_RELEASE) currentKeys[window][key] = KeyState::Pressed;
	else if (action == GLFW_REPEAT) currentKeys[window][key] = KeyState::KeyDown;
	else if (action == GLFW_PRESS) currentKeys[window][key] = KeyState::KeyDown;
}

InputHelper::InputHelper(GLFWwindow* window) :
	m_Window(window) {
	// Set the callback for the input helper.
	glfwSetKeyCallback(window, InputKeyCallback);

	// Add a new mapping to the map of mappings.
	KeyState* currentKeyStates = (KeyState*)malloc(sizeof(KeyState) * NUM_KEYS);
	KeyState* previousKeyStates = (KeyState*)malloc(sizeof(KeyState) * NUM_KEYS);

	for (int i = 0; i < NUM_KEYS; i++) currentKeyStates[i] = (KeyState)GLFW_RELEASE, previousKeyStates[i] = (KeyState)GLFW_RELEASE;

	currentKeys.insert(std::pair< GLFWwindow*, KeyState*>(window, currentKeyStates));
	previousKeys.insert(std::pair< GLFWwindow*, KeyState*>(window, previousKeyStates));

	// Set the cursor position.
	glfwGetCursorPos(window, &m_px, &m_py);
	glfwGetCursorPos(window, &m_cx, &m_cy);

	m_MouseLeftCurrent = m_MouseRightCurrent = m_MouseLeftPrevious = m_MouseRightPrevious = KeyState::Release;
}

void InputHelper::Update() {

	int* previousStates = (int*)previousKeys[m_Window];
	int* currentStates = (int*)currentKeys[m_Window];

	static const int lSize = NUM_KEYS >> 2;

	// Update the keyboard state.
	for (int i = 0; i < lSize; i++) {
		((int*)previousStates)[i] = currentStates[i];

		static const int mask = ((int)KeyState::Pressed) << 24 | ((int)KeyState::Pressed) << 16 | ((int)KeyState::Pressed) << 8 | ((int)KeyState::Pressed);
		currentStates[i] &= (currentStates[i] ^ mask);
	}

	// Update mouse position.
	m_px = m_cx, m_py = m_cy;
	glfwGetCursorPos(m_Window, &m_cx, &m_cy);

	// Update mouse buttons.
	m_MouseLeftPrevious = m_MouseLeftCurrent, m_MouseRightPrevious = m_MouseRightCurrent;

	int state = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS) m_MouseLeftCurrent = KeyState::KeyDown;
	else {
		if ((int)(m_MouseLeftCurrent & KeyState::KeyDown)) m_MouseLeftCurrent = KeyState::Pressed;
		else m_MouseLeftCurrent = KeyState::Release;
	}

	state = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_RIGHT);
	if (state == GLFW_PRESS) m_MouseRightCurrent = KeyState::KeyDown;
	else {
		if ((int)(m_MouseRightCurrent & KeyState::KeyDown)) m_MouseRightCurrent = KeyState::Pressed;
		else m_MouseRightCurrent = KeyState::Release;
	}
}

bool InputHelper::IsKeyDown(Key key) {
	return (int)(previousKeys[m_Window][(int)key] & (KeyState::KeyDown | KeyState::Pressed));
}

bool InputHelper::IsKeyPressed(Key key) {
	return (int)(previousKeys[m_Window][(int)key] & KeyState::Pressed);
}

bool InputHelper::IsKeyUp(Key key) {
	return (int)(previousKeys[m_Window][(int)key] & KeyState::Release);
}

KeyState InputHelper::GetKeyState(Key key) {
	return KeyState(previousKeys[m_Window][(int)key]);
}

glm::ivec2 InputHelper::GetCursorPosition() {
	return glm::ivec2((int)m_cx, (int)m_cy);
}

glm::vec2 InputHelper::GetCursorMovement() {
	return glm::vec2(m_cx - m_px, m_cy - m_py);
}

bool InputHelper::MouseLeftButtonDown() {
	return (int)(m_MouseLeftPrevious & (KeyState::KeyDown | KeyState::Pressed));
}

bool InputHelper::MouseLeftButtonClick() {
	return (int)(m_MouseLeftPrevious & KeyState::Pressed);
}

bool InputHelper::MouseRightButtonDown() {
	return (int)(m_MouseRightPrevious & (KeyState::KeyDown | KeyState::Pressed));
}

bool InputHelper::MouseRightButtonClick() {
	return (int)(m_MouseRightPrevious & KeyState::Pressed);
}

