#pragma once

#include <sstream>
#include <iostream>
#include <GL/glew.h>

#ifndef EMSCRIPTEN
#include <GL/glu.h>
#endif

class Logger
{
public:
    Logger();

    static void log(const std::string&  msg) noexcept;
    static std::ostream& stream() noexcept { return std::cout; }
    static std::ostream& errorStream() noexcept { return std::cerr; }

	static void shaderErrorCheck(GLuint shader, const std::string& shaderPath);
	static void programErrorCheck(GLuint program, const std::string& vsPath, const std::string& fsPath);
};

#define LOG_INFO_ATTACHMENT ":\nFUNCTION: " << __func__ << "\nFILE: " << __FILE__ << "\nLINE: " << __LINE__
#define ERROR(M) do {Logger::errorStream() << M << LOG_INFO_ATTACHMENT << "\n\n";} while(0)
#define EXIT(M) do {Logger::errorStream() << M << LOG_INFO_ATTACHMENT << "\n\n"; exit(1);} while(0)
#define LOG(M) do {Logger::stream() << M << "\n";} while(0)

#ifndef EMSCRIPTEN
#define GL_ERROR_CHECK() do {                    \
	GLenum errCode;							     \
	const GLubyte *errString;                    \
	if ((errCode = glGetError()) != GL_NO_ERROR) \
	{											 \
		errString = gluErrorString(errCode);     \
		ERROR("OpenGL Error: " << errString);	 \
	}										     \
} while (0)
#else
#define GL_ERROR_CHECK() do {} while(0)
#endif
