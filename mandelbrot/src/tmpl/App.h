#pragma once
#include "incl.h"
#include "Surface.h"
#include "InputHelper.h"

class App
{

public:
	/* Initializes the application and its window.
	* @param[in] width			Window and render width.
	* @param[in] height			Window and render height.
	*/
	App(uint width, uint height);
	/*
	* Free all allocated memory.
	*/
	~App();

	/* Start the application main-loop.
	*/
	void Run();

protected:
	/* Called every frame.
	* @param[in] dt			Time since last call.
	*/
	virtual void Tick(float dt) = 0;
	virtual void Draw(float dt) = 0;
	virtual void RenderGUI(float dt) = 0;

	/* Render width and height. */
	uint m_Width, m_Height;
	/* Average frame time. */
	float m_AvgTime = 0.0f;

	/* GLFW window reference. */
	GLFWwindow* m_Window = nullptr;
	/* Surface used for rendering. */
	Surface* m_RenderSurface = nullptr;
	
	/* Input. */
	InputHelper* m_InputHelper;

private:
	/* Initializes OpenGL and the GLFW window. */
	void InitGLFW();
	/* Initializes the Dear ImGui library.  */
	void InitImGui();
};

