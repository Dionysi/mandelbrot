#pragma once
#include "incl.h"

void EnableGLdebugInfo();
void DisableGLdebugInfo();

class glBuffer {

public:
	/* Initializes the buffer.
	* @param[in] type			Type of buffer.
	* @param[in] size			Size of the buffer in bytes.
	*/
	glBuffer(GLenum type, size_t size);
	~glBuffer();

	/* Bind the buffer to the current context. */
	void Bind();
	/* Unbinds the buffer. */
	void Unbind();

	/* Writes data to the gpu.
	* @param[in] size			Size of the data to copy in bytes.
	* @param[in] src			Pointer to the data.
	* @param[in] usage			Defines how the data is used. Either: GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or GL_DYNAMIC_COPY.
	*/
	void Write(size_t size, const void* src, GLenum usage);
	/* Writes data to part of the buffer.
	* @param[in] size			Size of the data to copy in bytes.
	* @param[in] offset			Offset of the data in the buffer on the gpu.
	* @param[in] src			Pointer to the data.
	*/
	void Write(size_t size, size_t offset, const void* src);

private:
	/* Befriend the shader class. */
	friend class glShader;
	/* Vertex buffer. */
	GLuint m_Buffer = 0;
	/* Buffer type. */
	GLenum m_Type;
};

class glShader {

public:
	glShader(const char* vertexPath, const char* fragmentPath);
	glShader(std::string vertexSource, std::string fragmentSource);
	~glShader();

	void SetBufferFloat1(glBuffer* buffer, uint idx, size_t stride = 0);
	void SetBufferFloat2(glBuffer* buffer, uint idx, size_t stride = 0);
	void SetBufferFloat3(glBuffer* buffer, uint idx, size_t stride = 0);
	void SetBufferFloat4(glBuffer* buffer, uint idx, size_t stride = 0);

	void SetBufferInt1(glBuffer* buffer, uint idx, size_t stride = 0);
	void SetBufferInt2(glBuffer* buffer, uint idx, size_t stride = 0);
	void SetBufferInt3(glBuffer* buffer, uint idx, size_t stride = 0);
	void SetBufferInt4(glBuffer* buffer, uint idx, size_t stride = 0);

	void SetBufferUint1(glBuffer* buffer, uint idx, size_t stride = 0);
	void SetBufferUint2(glBuffer* buffer, uint idx, size_t stride = 0);
	void SetBufferUint3(glBuffer* buffer, uint idx, size_t stride = 0);
	void SetBufferUint4(glBuffer* buffer, uint idx, size_t stride = 0);

	void SetUniformFloat(const char* name, float val);
	void SetUniformVec2(const char* name, glm::vec2 val);
	void SetUniformVec3(const char* name, glm::vec3 val);
	void SetUniformVec4(const char* name, glm::vec4 val);
	void SetUniformInt(const char* name, int val);
	void SetUniformVec2i(const char* name, glm::ivec2 val);
	void SetUniformVec3i(const char* name, glm::ivec3 val);
	void SetUniformVec4i(const char* name, glm::ivec4 val);
	void SetUniformMat4(const char* name, glm::mat4 mat);

	void Activate();
	void Deactivate();

	void DrawLines(size_t count, glBuffer* indexBuffer, GLenum idxType);
	void DrawTriangles(size_t count, glBuffer* indexBuffer, GLenum idxType);

	static void Finish() { glFinish(); }

private:
	/* OpenGL program. */
	GLuint m_Program = 0;
	/* Vertex array object to store the state for the current shader. */
	GLuint m_VAO = 0;

};