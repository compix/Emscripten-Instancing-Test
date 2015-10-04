#include "Logger.h"
#include <algorithm>
#include <vector>

Logger::Logger()
{

}

void Logger::log(const std::string& msg) noexcept
{
    stream() << msg << std::endl;
}

void Logger::shaderErrorCheck(GLuint shader, const std::string& shaderPath)
{
    GLint result = GL_FALSE;
    GLint infoLogLength;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> shaderLog(std::max(infoLogLength, 1));
    glGetShaderInfoLog(shader, infoLogLength, nullptr, &shaderLog[0]);

    if (result == GL_FALSE)
        EXIT("Shader comilation failed for " << shaderPath);

    if (infoLogLength > 0)
        fprintf(stdout, "%s\n", &shaderLog[0]);
}

void Logger::programErrorCheck(GLuint program, const std::string& vsPath, const std::string& fsPath)
{
    GLint result = GL_FALSE;
    GLint infoLogLength;

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> programLog(std::max(infoLogLength, 1));
    glGetProgramInfoLog(program, infoLogLength, nullptr, &programLog[0]);

    if (result == GL_FALSE)
        EXIT("Linking the program failed for " << vsPath << " and " << fsPath);

    if (infoLogLength > 0)
        fprintf(stdout, "%s\n", &programLog[0]);
}