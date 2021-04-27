#include <glew/glew.h>
#include <glfw/glfw3.h>
#include <exception>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#define WIDTH 600
#define HEIGHT 600

#define MAXITERATIONS 128

#define MAXZOOM 0.1f
#define MINZOOM 1.0f

struct color { 
	float r, g, b;

	color(float c) :
		r(c), g(c), b(c) {}

	color(float r, float g, float b) :
		r(r), g(g), b(b) {}

	color operator/(float d) {
		return color(r / d, g / d, b / d);
	}
};


GLFWwindow* CreateWindow() {

	// Initialise glfw.
	glewExperimental = true; // Needed for core-profile.

	if (!glfwInit()) throw std::exception("Failed to initialize glfw.");

	// Set the window variables.
	glfwWindowHint(GLFW_SAMPLES, 4);				// 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Mandelbrot set", NULL, NULL);

	if (window == NULL) throw std::exception("Failed to create window.");

	// Initialize glew.
	glfwMakeContextCurrent(window);
	
	if (glewInit() != GLEW_OK) throw std::exception("Failed to initialize glew");


	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// Set the input mode.
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	return window;
}

std::string readFile(const char* filePath) {
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if (!fileStream.is_open()) {
		std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

GLuint LoadShader(const char* vertex_path, const char* fragment_path) {
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Read shaders
	std::string vertShaderStr = readFile(vertex_path);
	std::string fragShaderStr = readFile(fragment_path);
	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	GLint result = GL_FALSE;
	int logLength;

	// Compile vertex shader
	printf("Compiling vertex shader...\n");
	glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
	glCompileShader(vertShader);

	// Check vertex shaders
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> vertShaderError((logLength > 1) ? logLength : 1);
	glGetShaderInfoLog(vertShader, logLength, NULL, &vertShaderError[0]);
	std::cout << &vertShaderError[0] << std::endl;

	// Compile fragment shader
	printf("Compiling fragment shader...\n");
	glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
	glCompileShader(fragShader);

	// Check fragment shader
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> fragShaderError((logLength > 1) ? logLength : 1);
	glGetShaderInfoLog(fragShader, logLength, NULL, &fragShaderError[0]);
	std::cout << &fragShaderError[0] << std::endl;

	printf("Linking program...\n");
	GLuint program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> programError((logLength > 1) ? logLength : 1);
	glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
	std::cout << &programError[0] << std::endl;

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return program;
}

/* Retrieves a color from the mandelbrot iteration number. 
 * @param[in] iteration		Iteration number.
 * @returns					Color from the pallette.
 */
color GetColor(int iteration) {

	if (iteration < 0) return color(0.0f);
	if (iteration == 0) return color(1.0f, 0.0f, 0.0f);
	if (iteration < 16) return color(16.0f, 0.0f, 16.0f * iteration - 1.0f) / 255.0f;
	if (iteration < 32) return color(0.0f, 16.0f * (iteration - 16.0f), 16.0f * (32.0f - iteration) - 1.0f) / 255.0f;
	if (iteration < 64) return color(8.0f * (iteration - 32.0f), 8.0f * (64.0f - iteration) - 1.0f, 0.0f) / 255.0f;
	return color(255.0f - (iteration - 64.0f) * 4.0f, 0.0f, 0.0f) / 255.0f;
}

/* Compute the mandelbrot set. 
 * @paramp[in] zoom		Zoom factor in the mandelbrot set.
 * @param[out] colors	Array of colors to store the computed image in.
 */
void ComputeMandelbrot(float zoom, color* colors) {

	// Loop for all pixels.
#pragma omp parallel for
	for (int y = 0; y < HEIGHT; y++)
		for (int x = 0; x < WIDTH; x++) {

			// Scale initial values for the 'seahorse' valley
			double x0 = -0.75 + ((double)x / (double)WIDTH - 0.5) * 2.0 * zoom; // [-2.5, 1.0]
			double y0 = 0.1 + ((double)y / (double)HEIGHT - 0.5) * 2.0 * zoom; // [-0.9, 1.1]
			
			// Iteration variables.
			double xi = 0.0;
			double yi = 0.0;

			double iteration = 0;
			double xTemp;

			while (pow(xi, 2) + pow(yi, 2) <= 4.0 && iteration < MAXITERATIONS) {
				xTemp = pow(xi, 2) - pow(yi, 2) + x0;
				yi = 2 * xi * yi + y0;
				xi = xTemp;
				iteration++;
			}

			if (iteration >= MAXITERATIONS)
				colors[WIDTH * y + x] = GetColor(-1);
			else
				colors[WIDTH * y + x] = GetColor(iteration);
		}
}

int main() {
	// Create our window.
	GLFWwindow* window = CreateWindow();

	// Setup renderstuff.
	GLuint programID = LoadShader("shader.vert", "shader.frag");

	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);
	// Our triangles.
	static const GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f
	};
	static const GLfloat uv[]{
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f
	};

	// Send vertices to GPU.
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint uvBuffer;
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv), uv, GL_STATIC_DRAW);

	// Array to render the colors to. 
	color* colors = (color*)malloc(sizeof(color) * WIDTH * HEIGHT);

	// Get a handle for "textureSampler" uniform
	GLuint TextureID = glGetUniformLocation(programID, "textureSampler");
	// Create our texture.
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, (GLvoid*)colors);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Time variables.
	double tCurr, tPrev;
	tCurr = tPrev = glfwGetTime();
	float dt;

	static int index = 0;
	int nextIndex = 0;

	float zoom = MINZOOM;
	bool zoomIn = true;

	// Main loop.
	do {
		// Update time. 
		tCurr = glfwGetTime();
		dt = (float)(tCurr - tPrev);
		tPrev = tCurr;

		std::cout << dt * 1000.0f << std::endl;

		index = (index + 1) % 2;
		nextIndex = (index + 1) % 2;

		// Reset render-loop.
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Compute the mandelbrot set.
		ComputeMandelbrot(zoom, colors);
		zoom = zoomIn ? zoom * 0.95f : zoom * 1.05f;
		if (zoom < MAXZOOM || zoom > MINZOOM)
			zoomIn = !zoomIn;

		glUseProgram(programID);

		// Update texture.
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, (GLvoid*)colors);



		// Render our triangles.
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &vertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	// Delete the colors array.
	free(colors);

	return -1;
}