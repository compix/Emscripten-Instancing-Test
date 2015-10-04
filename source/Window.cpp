#include "Window.h"
#include <GL/glew.h>
#include "util/Logger.h"

Window::Window()
    :m_window(nullptr), m_context(nullptr)
{
    if(!init())
        EXIT("Window initialization failed.");
}

Window::~Window()
{
    if(m_context)
    {
        SDL_GL_DeleteContext(m_context);
        m_context = nullptr;
    }

    if(m_window)
    {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
}

void Window::flip()
{
    SDL_GL_SwapWindow(m_window);
}

bool Window::init()
{
    m_window = SDL_CreateWindow(
		"Emscripten Instancing Test",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		800, 600,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (!m_window)
    {
        sdlErrorCheck();
        return false;
    }

#ifndef EMSCRIPTEN
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    try
    {
        m_context = SDL_GL_CreateContext(m_window);
        sdlErrorCheck();
    } catch(const std::runtime_error& ex)
    {
        Logger::stream() << "Failed to create OpenGL context. Make sure your GPU supports OpenGL 3.3: " << ex.what() << std::endl;
        return false;
    }

    if(SDL_GL_SetSwapInterval(-1))
		SDL_GL_SetSwapInterval(1);

    glewExperimental = GL_TRUE;
    glewInit();
    sdlErrorCheck();

	GL_ERROR_CHECK();

    return true;
}

void Window::sdlErrorCheck()
{
    const char* error = SDL_GetError();
	if (*error != '\0')
		LOG("SDL: " << error);

    SDL_ClearError();
}
