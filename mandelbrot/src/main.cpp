#include "tmpl/App.h"
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
		colors = new Color[width * height];
	}
	~DemoApp() {
		delete[] colors;
	}

protected:

	/*
	* Zoom-level for computing the Mandelbrot set.
	*/
	float zoom = 1.0f, zoomModifier = -0.1f;
	/*
	* Color-buffer for computing the Mandelbrot set.
	*/
	Color* colors = nullptr;

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

		if (zoom < 0.01f) zoomModifier = 0.1f;
		if (zoom > 1.0f) zoomModifier = -0.1f;
		zoom += zoomModifier * dt;
		
#pragma omp parallel for schedule(dynamic, NUM_THREADS)
		for (int x = 0; x < WIDTH; x++)
			for (int y = 0; y < HEIGHT; y++) {
				// Scale initial values for the 'seahorse' valley
				double x0 = -0.75 + ((double)x / (double)WIDTH - 0.5) * 2.0 * (double)zoom; // [-2.5, 1.0]
				double y0 = 0.1 + ((double)y / (double)HEIGHT - 0.5) * 2.0 * (double)zoom; // [-0.9, 1.1]

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
					colors[x + y * m_Width] = GetColor(-1);
				else
					colors[x + y * m_Width] = GetColor(iteration);
			}

	}
	void Draw(float dt) override {
		m_RenderSurface->PlotPixels(colors);
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
		ImGui::Text("Frame-time: %.1f", dt * 1000.0f);
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