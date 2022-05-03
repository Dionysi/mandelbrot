#include "tmpl/App.h"
#include <chrono>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#define WIDTH 1080
#define HEIGHT 720
#define MAX_ITERATIONS 1 << 8

#define NUM_THREADS 12	// Adjust to your own machine. Number of virtual cores is usually a good value.

class DemoApp : public App {

public:
	DemoApp(uint width, uint height) : App(width, height) {
		// Reserve memory for our color array.
		m_Colors = new Color[width * height];
	}
	~DemoApp() {
		delete[] m_Colors;
	}

protected:

	/*
	* Zoom-level for computing the Mandelbrot set.
	*/
	float m_Zoom = 1.0f, m_ZoomModifier = -0.1f;
	/*
	* Color-buffer for computing the Mandelbrot set.
	*/
	Color* m_Colors = nullptr;
	/*
	* Average time to compute a frame (in seconds).
	*/
	float m_AvgFrameTime = 1.0f;
	/*
	* Time it took to render the last computed frame (in seconds).
	*/
	float m_LastFrame = 0.0f;


	/*
	* Computes the color for the number of iterations it took to compute the Mandelbrot value.
	* @param[in] iteration			Number of iterations it took to calculate the Mandelbrot value.
	* @returns						Color corresponding to the value.
	*/
	inline Color GetColor(int iteration) {
		if (iteration < 0) return Color(0.0f);
		else if (iteration == 0) return Color(1.0f, 0.0f, 0.0f);
		else if (iteration < 16) return Color(16.0f, 0.0f, 16.0f * iteration - 1.0f) / 255.0f;
		else if (iteration < 32) return Color(0.0f, 16.0f * (iteration - 16.0f), 16.0f * (32.0f - iteration) - 1.0f) / 255.0f;
		else if (iteration < 64) return Color(8.0f * (iteration - 32.0f), 8.0f * (64.0f - iteration) - 1.0f, 0.0f) / 255.0f;
		else return Color(255.0f - (iteration - 64.0f) * 4.0f, 0.0f, 0.0f) / 255.0f;
	}

	/*
	* Compute the Mandelbrot set for the screen-texture space.
	*/
	void Tick(float dt) override {

		if (m_Zoom < 0.01f) m_ZoomModifier = 0.1f;
		if (m_Zoom > 1.0f) m_ZoomModifier = -0.1f;
		m_Zoom += m_ZoomModifier * dt;

		auto sTime = std::chrono::system_clock::now();

#pragma omp parallel for schedule(dynamic, NUM_THREADS)
		for (int x = 0; x < WIDTH; x++)
			for (int y = 0; y < HEIGHT; y++) {
				// Scale initial values for the 'seahorse' valley
				double x0 = -0.75 + ((double)x / (double)WIDTH - 0.5) * 2.0 * (double)m_Zoom; // [-2.5, 1.0]
				double y0 = 0.1 + ((double)y / (double)HEIGHT - 0.5) * 2.0 * (double)m_Zoom; // [-0.9, 1.1]

				double xi = 0.0;
				double yi = 0.0;

				double iteration = 0;
				double xTemp;

				while (pow(xi, 2) + pow(yi, 2) <= 4.0 && iteration < MAX_ITERATIONS) {
					xTemp = pow(xi, 2) - pow(yi, 2) + x0;
					yi = 2 * xi * yi + y0;
					xi = xTemp;
					iteration++;
				}

				if (iteration >= MAX_ITERATIONS)
					m_Colors[x + y * m_Width] = GetColor(-1);
				else
					m_Colors[x + y * m_Width] = GetColor(iteration);
			}

		auto eTime = std::chrono::system_clock::now();
		m_LastFrame = std::chrono::duration<float>(eTime - sTime).count();

		m_AvgFrameTime = m_AvgFrameTime * 0.95f + m_LastFrame * 0.05f;
	}
	void Draw(float dt) override {
		m_RenderSurface->PlotPixels(m_Colors);
	}
	void RenderGUI(float dt) override {
		// feed inputs to dear imgui, start new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		const static char* windowTitle = "Debug";
		static bool display = true;
		ImGui::Begin(windowTitle, &display, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::SetWindowFontScale(1.5f);
		ImGui::Text("avg frame: %.1f", m_AvgFrameTime * 1000.0f);
		ImGui::Text("last frame: %.1f", m_LastFrame * 1000.0f);
		ImGui::End();

		// Render dear imgui into screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
};

int main() {
	DemoApp* app = new DemoApp(WIDTH, HEIGHT);
	app->Run();

	return 0;
}