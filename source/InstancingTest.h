#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "Window.h"
#include <GL/glew.h>

struct Mesh
{
	Mesh() {}

	GLuint vbo{ 0 };
	GLuint instanceVBO{ 0 };
	GLuint vao{ 0 };

	GLuint vertexCount{ 0 };

	GLenum renderMode{ 0 };
};

struct NormalWrapper
{
	~NormalWrapper();

	void render(const glm::mat4& viewProj);
	void setup();

	GLuint shaderProgram;
	Mesh mesh;
	glm::mat4 model;
};

struct InstanceWrapper
{
	~InstanceWrapper();

	void setup();
	void render(const glm::mat4& viewProj);

	GLuint shaderProgram;
	Mesh mesh;
};

class InstancingTest
{
public:
    InstancingTest();

    void init();
    void update();
    bool running() { return m_running; }
    void shutdown();

private:
    bool m_running;
    bool m_initialized;

    std::unique_ptr<Window> m_window;

	NormalWrapper m_normal1;
	InstanceWrapper m_instance;

	glm::mat4 m_viewProj;
};
