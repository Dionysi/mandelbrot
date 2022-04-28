#include "App.h"
#include <chrono>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Shader.h"

App::App(uint width, uint height)
	: m_Width(width), m_Height(height)
{
	InitGLFW();
	InitImGui();

	m_RenderSurface = new Surface(m_Width, m_Height);

	m_InputHelper = new InputHelper(m_Window);
}

App::~App()
{
	delete m_InputHelper;
	delete m_RenderSurface;
	delete m_Window;
}

void App::Run()
{	
	// Variables for computing time passed per frame.
	std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point tc = std::chrono::system_clock::now();
	float dt = std::chrono::duration<float>(tc - tp).count() + 0.00001f;

	do {
		// Compute the time passed since last loop.
		float dt = std::chrono::duration<float>(tc - tp).count() + 0.00001f;
		tp = tc; tc = std::chrono::system_clock::now();
		// Compute the average time (smoothes out over time).
		m_AvgTime = m_AvgTime * 0.975f + dt * 0.025f;

		glClearColor(0.102f, 0.117f, 0.141f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// App logic.
		Tick(dt);
		Draw(dt);


		m_RenderSurface->Draw();
		RenderGUI(dt);


		glfwSwapBuffers(m_Window);
		glfwPollEvents();

		// Update the InputHelper.
		m_InputHelper->Update();

	} while (!glfwWindowShouldClose(m_Window) && !m_InputHelper->IsKeyPressed(Key::Escape));
}

void App::InitGLFW()
{
	// Initialize glfw.
	glewExperimental = true;

	if (!glfwInit()) FATAL_ERROR("Failed to initialize glfw.");

	// Set the window variables.
	glfwWindowHint(GLFW_SAMPLES, 4);				// 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_Window = glfwCreateWindow(m_Width, m_Height, "INFOMOV", NULL, NULL);
	if (m_Window == NULL) FATAL_ERROR("Failed to create GLFW window.");

	glfwSetWindowAspectRatio(m_Window, m_Width, m_Height);

	// Initialize glew.
	glfwMakeContextCurrent(m_Window);

	if (glewInit() != GLEW_OK) FATAL_ERROR("Failed to initialize glew.");

	// Enable OpenGL debug callbacks.
	EnableGLdebugInfo();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// Set the input modes.
	glfwSetInputMode(m_Window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glfwSwapInterval(0);
}

void App::InitImGui()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
}
