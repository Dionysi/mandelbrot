#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <iostream>

#include <glew/glew.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

struct Color {
	Color() : r(0.0f), g(0.0f), b(0.0f), a(0.0f) {}
	Color(float val) : r(val), g(val), b(val), a(1.0f) {}
	Color(float r, float g, float b) : r(r), g(g), b(b), a(1.0f) {}
	Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

	Color operator/(float rhs) {
		return Color(r / rhs, g / rhs, b / rhs, a / rhs);
	}
	Color operator/=(float rhs) {
		r /= rhs, g /= rhs, b /= rhs, a /= rhs;
	}
	Color operator*(float rhs) {
		return Color(r * rhs, g * rhs, b * rhs, a * rhs);
	}
	Color operator*=(float rhs) {
		r *= rhs, g *= rhs, b *= rhs, a *= rhs;
	}
	Color operator+(float rhs) {
		return Color(r + rhs, g + rhs, b + rhs, a + rhs);
	}
	Color operator+=(float rhs) {
		r += rhs, g += rhs, b += rhs, a += rhs;
	}
	Color operator-(float rhs) {
		return Color(r - rhs, g - rhs, b - rhs, a - rhs);
	}
	Color operator-=(float rhs) {
		r -= rhs, g -= rhs, b -= rhs, a -= rhs;
	}

	Color operator/(Color rhs) {
		return Color(r / rhs.r, g / rhs.g, b / rhs.b, a / rhs.a);
	}
	Color operator/=(Color rhs) {
		r /= rhs.r, g /= rhs.g, b /= rhs.b, a /= rhs.a;
	}
	Color operator*(Color rhs) {
		return Color(r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a);
	}
	Color operator*=(Color rhs) {
		r *= rhs.r, g *= rhs.g, b *= rhs.b, a *= rhs.a;
	}
	Color operator+(Color rhs) {
		return Color(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
	}
	Color operator+=(Color rhs) {
		r += rhs.r, g += rhs.g, b += rhs.b, a += rhs.a;
	}
	Color operator-(Color rhs) {
		return Color(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a);
	}
	Color operator-=(Color rhs) {
		r -= rhs.r, g -= rhs.g, b -= rhs.b, a -= rhs.a;
	}

	float r, g, b, a;
};

void FATAL_ERROR(const char* format, ...);

/** Reads the contents of a file.
* @param[in] Path to the file.
* @returns Contents of the file.
*/
std::string readFile(const char* filePath);
/** Creates an OpenGL program from a vertex- and fragment shader.
* @param[in] Path to the vertex shader.
* @param[in] Path to the fragment shader.
* @return OpenGL program.
*/
GLuint LoadShader(const char* vertexPath, const char* fragmentPath);