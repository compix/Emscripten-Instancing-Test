#pragma once
#include <SDL.h>
#include <string>

class Window
{
public:
    Window();
    ~Window();

    void flip();

    void setTitle(const std::string& title) { SDL_SetWindowTitle(m_window, title.c_str()); }
private:
    bool init();

    void sdlErrorCheck();

private:
    SDL_Window* m_window;
    SDL_GLContext m_context;
};
