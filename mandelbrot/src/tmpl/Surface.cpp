#include "surface.h"

#define QUAD_RENDERING

Surface::Surface(uint width, uint height)
	: m_Width(width), m_Height(height) {
	// Create our render texture.
	glGenTextures(1, &m_RenderTexture);
	glBindTexture(GL_TEXTURE_2D, m_RenderTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

#ifdef QUAD_RENDERING
	// Create the OpenGL program. 
	m_Program = LoadShader("assets/shaders/simple_tex.vert", "assets/shaders/simple_tex.frag");
	// Setup render-stuff.
	glGenVertexArrays(1, &m_VertexArrayID);
	glBindVertexArray(m_VertexArrayID);
	// Send vertices to GPU.
	glGenBuffers(1, &m_VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_RenderQuad), m_RenderQuad, GL_STATIC_DRAW);
	// Send uv-coordinates to GPU.
	glGenBuffers(1, &m_uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_UV), m_UV, GL_STATIC_DRAW);
#else

	/** Setup rendering via copying buffer to the framebuffer. */
	glGenFramebuffers(1, &m_FrameBufferID);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FrameBufferID);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, m_FrameBufferID, 0);
#endif

}

Surface::~Surface() {
	glDeleteBuffers(1, &m_VertexBuffer);
	glDeleteProgram(m_Program);
	glDeleteVertexArrays(1, &m_VertexArrayID);
}

void Surface::Draw() {

#ifdef QUAD_RENDERING
	/** Render via a texture quad.*/
	glUseProgram(m_Program);
	glBindVertexArray(m_VertexArrayID);
	glBindTexture(GL_TEXTURE_2D, m_RenderTexture);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
#else
	/** Render by directly copying our texture to the framebuffer. */
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, m_Width, m_Height, 0, 0, m_Width, m_Height,
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
#endif
}

void Surface::PlotPixels(Color* colors) {
	glBindTexture(GL_TEXTURE_2D, m_RenderTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_FLOAT, (GLvoid*)colors);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFinish();
}

void Surface::PlotPixels(Color* colors, uint dx, uint dy, uint width, uint height) {
	glBindTexture(GL_TEXTURE_2D, m_RenderTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, dx, dy, width, height, GL_RGBA, GL_FLOAT, (GLvoid*)colors);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFinish();
}