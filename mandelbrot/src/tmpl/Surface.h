#pragma once
#include "incl.h"

/** Surface used for rendering to the screen.An OpenGL texture is used to render to a quad. */
class Surface {

public:

	/** Initialize the surface according to the specified size.
	* @param[in] width		Surface width.
	* @param[in] height		Surface height.
	*/
	Surface(uint width, uint height);
	~Surface();

	/** Renders the render texture to the current OpenGL context. */
	void Draw();

	/** Plots the pixel array to the surface. <b>Note:</b> must be of size width * height.
	* @param[in] colors		Array of size width * height.
	*/
	void PlotPixels(Color* colors);
	/** Write pixels to the surface.
	* @param[in] colors		Colors to write to the surface.
	* @param[in] dx			x-offset.
	* @param[in] dy			y-offset.
	* @param[in] width		Number of pixels in x-direction.
	* @param[in] height		Number of pixels in y-direction.
	*/
	void PlotPixels(Color* colors, uint dx, uint dy, uint width, uint height);

	inline GLuint& GetRenderTexture() { return m_RenderTexture; }
	inline uint GetWidth() { return m_Width; }
	inline uint GetHeight() { return m_Height; }

private:
	/** Surface dimensions.*/
	uint m_Width, m_Height;

	/** Texture used for rendering. */
	GLuint m_RenderTexture;
	/** OpenGL program used for rendering. */
	GLuint m_Program;
	/** Render stuff. */
	GLuint m_VertexArrayID, m_VertexBuffer, m_uvBuffer;

	/** Quad used for rendering the texture to. */
	const GLfloat m_RenderQuad[18] = {
		-1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f
	};
	/** UV-coordinates for sampling the render texture. */
	const GLfloat m_UV[12] = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f
	};

	GLuint m_FrameBufferID = 0;
};
