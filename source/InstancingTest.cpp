#include "InstancingTest.h"
#include "util/Logger.h"
#include <string>
#include <glm/ext.hpp>
#include <fstream>
#include <vector>

std::string readAsString(const std::string& path)
{
    std::string fileAsString = "";
    std::ifstream stream(path, std::ios::in);

    if (!stream.is_open())
    {
        Logger::stream() << "Could not open file: " << path << std::endl;
        return "";
    }

    std::string line = "";
    while (getline(stream, line))
        fileAsString += line + "\n";

    stream.close();

    return fileAsString;
}

GLuint loadShaderProgram(const std::string& vsPath, const std::string& fsPath, const std::vector<std::string>& attributes)
{
    GLuint vsID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fsID = glCreateShader(GL_FRAGMENT_SHADER);

    // Compile Vertex Shader
    std::string source = readAsString(vsPath);
    char const* vsSource = source.c_str();
    glShaderSource(vsID, 1, &vsSource, nullptr);
    glCompileShader(vsID);

    Logger::shaderErrorCheck(vsID, vsPath);

    // Compile Fragment Shader
    source = readAsString(fsPath);
    char const* fsSource = source.c_str();
    glShaderSource(fsID, 1, &fsSource, nullptr);
    glCompileShader(fsID);

    Logger::shaderErrorCheck(fsID, fsPath);

    GLuint programID = glCreateProgram();

    glAttachShader(programID, vsID);
    glAttachShader(programID, fsID);

    GLuint index = 0;
    for (auto& attribute : attributes)
        glBindAttribLocation(programID, index++, attribute.c_str());

    glLinkProgram(programID);

    Logger::programErrorCheck(programID, vsPath, fsPath);

    glDeleteShader(vsID);
    glDeleteShader(fsID);

    GL_ERROR_CHECK();

    return programID;
}

NormalWrapper::~NormalWrapper()
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteProgram(shaderProgram);
}

void NormalWrapper::render(const glm::mat4& viewProj)
{
    glm::mat4 modelViewProj = viewProj * model;
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelViewProj"), 1, GL_FALSE, &modelViewProj[0][0]);

    glBindVertexArray(mesh.vao);
    glDrawArrays(mesh.renderMode, 0, mesh.vertexCount);
}

void NormalWrapper::setup()
{
    shaderProgram = loadShaderProgram("assets/shaders/normal.vert", "assets/shaders/normal.frag", { "in_pos", "in_color" });

    float posColorVertices[]{
        -0.25f, -0.25f, 0.f, 0.f, 0.f, 1.f, 1.f,
        0.25f, -0.25f, 0.f, 0.f, 0.f, 1.f, 1.f,
        -0.25f, 0.25f, 0.f, 0.f, 0.f, 1.f, 1.f,
        0.25f, 0.25f, 0.f, 0.f, 0.f, 1.f, 1.f
    };

    constexpr size_t vertexSize = sizeof(float) * 7;
    constexpr uint8_t vertexCount = 4;
    mesh.renderMode = GL_TRIANGLE_STRIP;

    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, &posColorVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<const void*>(12));

    mesh.vertexCount = vertexCount;

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GL_ERROR_CHECK();
}

InstanceWrapper::~InstanceWrapper()
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.instanceVBO);
    glDeleteProgram(shaderProgram);
}

void InstanceWrapper::setup()
{
    shaderProgram = loadShaderProgram("assets/shaders/instanced.vert", "assets/shaders/instanced.frag", { "in_sPos", "in_pos" });

    float posVertices[]{
        -0.25f, -0.25f, 0.f,
        0.25f, -0.25f, 0.f,
        -0.25f, 0.25f, 0.f,
        0.25f, 0.25f, 0.f
    };

    constexpr size_t vertexSize = sizeof(float) * 3;
    constexpr uint8_t vertexCount = 4;
    mesh.renderMode = GL_TRIANGLE_STRIP;

    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // Instanced
    constexpr size_t instanceSize = sizeof(float) * 3;
    glGenBuffers(1, &mesh.instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.instanceVBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, instanceSize, 0);
    glVertexAttribDivisor(0, 1);

    // Normal
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, &posVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, 0);

    mesh.vertexCount = vertexCount;

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GL_ERROR_CHECK();
}

void InstanceWrapper::render(const glm::mat4& viewProj)
{
    constexpr uint8_t instNum = 6;
    constexpr size_t instanceSize = sizeof(glm::vec3);
    glm::vec3 instData[instNum];
    for (uint8_t i = 0; i < instNum; ++i)
        instData[i] = glm::vec3(i + 1, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instanceSize * instNum, &instData, GL_DYNAMIC_DRAW);

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewProj"), 1, GL_FALSE, &viewProj[0][0]);

    glBindVertexArray(mesh.vao);
    glDrawArraysInstanced(mesh.renderMode, 0, mesh.vertexCount, instNum);
}

InstancingTest::InstancingTest()
    :m_running(true), m_initialized(false)
{
}

void InstancingTest::init()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        exit(1);

    m_window = std::make_unique<Window>();

    m_normal1.setup();
    m_normal1.model = glm::translate(glm::vec3(0));

    m_instance.setup();

    m_viewProj = glm::perspective(45.f, 4.f / 3.f, 0.5f, 100.f) * glm::lookAt(glm::vec3(3, 0, 5), glm::vec3(3, 0, 0), glm::vec3(0, 1, 0));

    m_initialized = true;
}

void InstancingTest::update()
{
    SDL_Event sdlEvent;

    while (SDL_PollEvent(&sdlEvent))
    {
        if (sdlEvent.type == SDL_QUIT)
        {
            m_running = false;
        }
    }

    if (!m_initialized)
    {
        Logger::log("InstancingTest is not initialized. Shutting down...");
        exit(1);
    }

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_normal1.render(m_viewProj);
    m_instance.render(m_viewProj);

    m_window->flip();
}

void InstancingTest::shutdown()
{
    SDL_Quit();
}